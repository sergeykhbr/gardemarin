/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <fwapi.h>
#include <uart.h>
#include <FreeRTOS.h>
#include "ManagementClass.h"

ManagementClass::ManagementClass(TaskHandle_t taskHandle)
    : FwObject("man"),
    requestToService_("RequestToService"),
    lastWatering_("LastWatering"),
    estate_(WaitInit),
    taskHandle_(taskHandle) {
    btnClick_ = false;
    epochCnt_ = 0;
    epochMarker_ = 0;
    sewer_gram_ = 0;
    plants_gram_ = 0;
    mix_gram_ = 0;
    confirmCnt_ = 0;

    requestToService_.make_int8(0);
    lastWatering_.make_uint32(0);
}

void ManagementClass::Init() {
    RegisterAttribute(&requestToService_);
    RegisterAttribute(&lastWatering_);
}

void ManagementClass::PostInit() {
    KeyInterface *iface = reinterpret_cast<KeyInterface *>(
            fw_get_object_interface("ubtn0", "KeyInterface"));
    if (iface) {
        iface->registerKeyListener(
            static_cast<KeyListenerInterface *>(this));
    }
}


void ManagementClass::update() {
    int btnClick = btnClick_;
    btnClick_ = 0;

    epochCnt_++;

    if (btnClick || requestToService_.to_int8()) {
        requestToService_.make_int8(0);
        if (estate_ != Servicing) {
            switchToService();
        } else {
            switchToNormal();
        }
    }

    switch (estate_) {
    case WaitInit:
        if (isPeriodExpired(60)) {
            switchToState(DrainBefore);
        }
        break;
    case CheckMoisture:
        if (isPeriodExpired(30 * 60)
            || (isPeriodExpired(10 * 60) && getMoisture() < 750)) {
            switchToState(DrainBefore);
            write_int8("hbrg0", "dc0_duty", 100);
            confirmCnt_ = 0;
        }
        break;
    case DrainBefore:
        // Drain speed ~22 gram/sec
        if ((plants_gram_ - getPlantWeight()) < 10.0f) {
            confirmCnt_++;
        } else {
            confirmCnt_ = 0;
        }
        // 240 sec * 22 = 5280 grams watchdog
        if (confirmCnt_ > 2 || isPeriodExpired(240)) {
            switchToState(OxygenSaturation);
            write_int8("hbrg0", "dc0_duty", 0);
            write_int8("hbrg2", "dc0_duty", 100);
        }
        break;
    case OxygenSaturation:
        if (isPeriodExpired(30)) {
            switchToState(Watering);
            confirmCnt_ = 0;
            lastWatering_.make_uint32(epochCnt_);
            write_int8("hbrg2", "dc0_duty", 0);
            write_int8("relais0", "State", 1);
        }
        break;
    case Watering:
        // Watering rate ~14 gram/sec
        if ((mix_gram_ - getMixWeight()) < 5.0f) {
            // no water in mix tank
            confirmCnt_++;
        } else {
            confirmCnt_ = 0;
        }
        // 240 sec * 14 = 3360 grams of water
        if (getMoisture() > 980
            || confirmCnt_ > 2
            || isPeriodExpired(240)) {
            write_int8("relais0", "State", 0);
            write_int8("hbrg0", "dc0_duty", 100);
            switchToState(DrainAfter);
            confirmCnt_ = 0;
        }
        break;
    case DrainAfter:
        // Drain speed ~22 gram/sec
        if ((plants_gram_ - getPlantWeight()) < 10.0f) {
            confirmCnt_++;
        } else {
            confirmCnt_ = 0;
        }
        // 240 sec * 22 = 5280 grams watchdog
        if (confirmCnt_ > 2 || isPeriodExpired(240)) {
            write_int8("hbrg0", "dc0_duty", 0);
            switchToState(AdjustLights);
        }
        break;
    case AdjustLights:
        setDayLights(Lattuce, Vegetative, getTimeOfDay());
        switchToState(CheckMoisture);
        break;

    case Servicing:
        // Do nothing
        write_int8("uled0", "state", static_cast<int8_t>(epochCnt_ & 0x1));
        break;
    default:
        estate_ = WaitInit;
    }

    sewer_gram_ = read_float32("scales", "gram0");
    plants_gram_ = read_float32("scales", "gram1");
    mix_gram_ = read_float32("scales", "gram2");
}

void ManagementClass::keyPressed() {
    xTaskNotify(taskHandle_,
                0,
                eNoAction);
    btnClick_ = true;
}

void ManagementClass::waitKeyPressed() {
    uint32_t notifiedValue = 0;
    xTaskNotifyStateClear(taskHandle_);
    xTaskNotifyWait(0x00,   // don't clear any notification bits on entry
                    0xffffffffUL,  // Reset the notification value to 0 on exit
                    &notifiedValue,
                    portMAX_DELAY); // Block indefinetly
}

bool ManagementClass::isPeriodExpired(uint32_t period) {
    uint32_t dt = epochCnt_ - epochMarker_;
    if (dt >= period) {
        return true;
    }
    return false;
}

void ManagementClass::switchToState(EState newstate) {
    epochMarker_ = epochCnt_;
    estate_ = newstate;
}

void ManagementClass::switchToService() {
    normal_.estate = estate_;
    write_int8("relais0", "State", 0);  // watering
    write_int8("hbrg0", "dc0_duty", 0); // sewer
    write_int8("hbrg2", "dc0_duty", 0); // oxygen
    write_int8("hbrg2", "dc1_duty", 0); // lights up/down
    write_int8("uled0", "state", 0);
    estate_ = Servicing;
    uart_printf("[%d] Switching to Service\r\n", xTaskGetTickCount());
}

void ManagementClass::switchToNormal() {
    estate_ = normal_.estate;
    write_int8("uled0", "state", 1);
    uart_printf("[%d] Switching to Normal\r\n", xTaskGetTickCount());
}

void ManagementClass::setDayLights(EPlantType plant,
                                   EPlantStage stage,
                                   uint32_t tow) {
    const ProfileItemType *prf = &PlantProfile_[plant][stage];
    if (tow < prf->dayStartSec || tow > prf->dayEndSec) {
        write_int8("ledrbw", "duty0", 0);   // blue
        write_int8("ledrbw", "duty1", 0);   // unused
        write_int8("ledrbw", "duty2", 0);   // white
        write_int8("ledrbw", "duty3", 0);   // red/blue
    } else {
        write_int8("ledrbw", "duty0", prf->dutyBlue);   // blue
        write_int8("ledrbw", "duty2", prf->dutyWhite);   // white
        write_int8("ledrbw", "duty3", prf->dutyRed);   // red/blue
    }
}


void ManagementClass::write_obj_attribute(const char *objname,
                         const char *atrname,
                         char *buf,
                         int sz) {
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute(objname, atrname));
    if (atr) {
        atr->write(buf, sz, false);
    } else {
        uart_printf("[%d] %s:%s not found\r\n",
            xTaskGetTickCount(), objname, atrname);
    }
}

void ManagementClass::write_int8(const char *objname,
                                const char *atrname,
                                int8_t v) {
    char buf[2] = {static_cast<char>(v), 0};
    write_obj_attribute(objname, atrname, buf, 1);
}

int8_t ManagementClass::read_int8(const char *objname,
                                   const char *atrname) {
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute(objname, atrname));
    if (atr) {
        return atr->to_int8();
    }
    return 0;
}

uint16_t ManagementClass::read_uint16(const char *objname,
                                      const char *atrname) {
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute(objname, atrname));
    if (atr) {
        return atr->to_uint16();
    }
    return 0;
}

uint32_t ManagementClass::read_uint32(const char *objname,
                                      const char *atrname) {
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute(objname, atrname));
    if (atr) {
        return atr->to_uint32();
    }
    return 0;
}

float ManagementClass::read_float32(const char *objname,
                                    const char *atrname) {
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute(objname, atrname));
    if (atr) {
        return atr->to_float();
    }
    return 0;
}

float ManagementClass::getPlantWeight() {
    return read_float32("scales", "gram1");
}

float ManagementClass::getMixWeight() {
    return read_float32("scales", "gram2");
}

float ManagementClass::getSewerWeight() {
    return read_float32("scales", "gram0");
}

uint16_t ManagementClass::getMoisture() {
    return read_uint16("soil0", "moisture");
}

uint32_t ManagementClass::getTimeOfDay() {
    uint32_t tow = 0;
    // [31:23] reserved
    // [22] PM
    // [21:20] Hours tens in BCD format
    // [19:16] Hours units in BCD format
    // [15] resrved
    // [14:12] Minutes tens in BCD format
    // [11:8] Minutes units in BCD format
    // [7] resered
    // [6:4] Seconds tens in BCD format
    // [3:0] Seconds units in BCD format
    uint32_t rtc_time = read_uint32("rtc", "Time");
    tow = ((rtc_time >> 20) & 0x3) * 10 * 3600; // seconds
    tow += ((rtc_time >> 16) & 0xF) * 3600;
    tow += ((rtc_time >> 12) & 0x7) * 10 * 60;
    tow += ((rtc_time >> 8) & 0xF) * 60;
    tow += ((rtc_time >> 4) & 0x7) * 10;
    tow += ((rtc_time >> 0) & 0xF);
    return tow;
}
