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

const char *ManagementClass::STATES_NAMES[States_Total] = {
    "WaitInit",
    "CheckWateringInterval",
    "DrainBefore",
    "OxygenSaturation",
    "Watering",
    "DrainAfter",
    "AdjustLights",
    "Servicing"
};

static const int MIX_TANK_EMPTY_CNT_MAX = 2;

ManagementClass::ManagementClass(TaskHandle_t taskHandle)
    : FwObject("man"),
    estate_(WaitInit),
    taskHandle_(taskHandle) {
    btnClick_ = false;
    epochCnt_ = 0;
    epochMarker_ = 0;
    stateSwitchedLast_ = 0;
    mix_gram_ = 0;
    shortWateringCnt_ = 0;
    for (int i = 0; i < 2*WEIGHT_PERIOD_LENGTH; i++) {
        lastGram_[i] = 0;
    }
    plastGram_ = &lastGram_[WEIGHT_PERIOD_LENGTH - 1];
}

void ManagementClass::Init() {
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

    updateMixWeight();

    if (btnClick || read_int8("usrset", "RequestToService")) {
        write_int8("usrset", "RequestToService", 0);
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
            enableDrainPump();
        }
        break;
    case CheckWateringInterval:
        if (isPeriodExpired(read_uint16("usrset", "WateringInterval"))) {
            if (read_int8("usrset", "WateringPerDrain") > 1) {
                if (shortWateringCnt_ == 0) {
                    switchToState(OxygenSaturation);
                    enableOxyPump();
                } else {
                    switchToState(Watering);
                    enableHighPressurePump();
                }
                // drain only after cycle is finished
            } else {
                switchToState(DrainBefore);
                enableDrainPump();
            }
        }
        break;
    case DrainBefore:
        // Drain speed ~22 gram/sec
        if (isDrainEnd()) {
            switchToState(OxygenSaturation);
            disableDrainPump();
            enableOxyPump();
        }
        break;
    case OxygenSaturation:
        if (isPeriodExpired(read_uint16("usrset", "OxygenSaturationInterval"))) {
            switchToState(Watering);
            write_uint32("usrset", "LastWatering", read_uint32("rtc", "Time"));
            disableOxyPump();
            enableHighPressurePump();
        }
        break;
    case Watering:
        // Watering rate ~14 gram/sec
        if (isWateringEnd()) {
            disableHighPressurePump();
            if (++shortWateringCnt_ >= read_int8("usrset", "WateringPerDrain")) {
                switchToState(DrainAfter);
                enableDrainPump();
                shortWateringCnt_ = 0;
            } else {
                // skip drain
                switchToState(AdjustLights);
            }
        }
        break;
    case DrainAfter:
        if (isDrainEnd()) {
            disableDrainPump();
            switchToState(AdjustLights);
        }
        break;
    case AdjustLights:
        setDayLights(getTimeOfDay());
        switchToState(CheckWateringInterval);
        break;

    case Servicing:
        // Do nothing
        write_int8("uled0", "state", static_cast<int8_t>(epochCnt_ & 0x1));
        break;
    default:
        estate_ = WaitInit;
    }

    mix_gram_ = getMixWeight();
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

void ManagementClass::updateMixWeight() {
    int deltaGram = static_cast<int>(getMixWeight() - mix_gram_);
    plastGram_--;
    if (plastGram_ < lastGram_) {
        plastGram_ = &lastGram_[WEIGHT_PERIOD_LENGTH - 1];
    }

    *plastGram_ = deltaGram;
    *(plastGram_ + WEIGHT_PERIOD_LENGTH) = deltaGram;

}

bool ManagementClass::isDrainEnd() {
    int deltaGram = 0;
    if (isPeriodExpired(240)) {
        // 240 sec * 22 = 5280 grams watchdog
        return true;
    }
    if (!isPeriodExpired(WEIGHT_PERIOD_LENGTH)) {
        // minimal drain timeout to make any decision
        return false;
    }
    if (mix_gram_ < 5000.0f) {
        // minimal full tank volume
        return false;
    }
    for (int i = 0; i < WEIGHT_PERIOD_LENGTH; i++) {
        deltaGram += plastGram_[i];
    }
    uart_printf("[%d] DeltaGram=%d %d\r\n", xTaskGetTickCount(), deltaGram, plastGram_[0]);
    // Drain speed ~22 gram/sec
    if (deltaGram < 5*WEIGHT_PERIOD_LENGTH) {
        return true;
    }
    return false;
}

bool ManagementClass::isWateringEnd() {
    int deltaGram = 0;
    if (isPeriodExpired(read_uint16("usrset", "WateringDuration"))) {
        // 240 sec * 14 = 3360 grams of water
        return true;
    }
    if (!isPeriodExpired(WEIGHT_PERIOD_LENGTH)) {
        // minimal drain timeout to make any decision
        return false;
    }
    if (mix_gram_ > 2000.0f) {
        // minimal safe tank volume
        return false;
    }
    for (int i = 0; i < WEIGHT_PERIOD_LENGTH; i++) {
        deltaGram += plastGram_[i];
    }
    // Watering rate ~14 gram/sec
    if (deltaGram > -5) {
        // no water in mix tank
        uart_printf("[%d] Mix tank is empty\r\n", xTaskGetTickCount());
        // to switch to DrainAfter state
        shortWateringCnt_ = read_int8("usrset", "WateringPerDrain");
        return true;
    }
    return false;
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
    uart_printf("[%d, %d, %s, %s]\r\n",
        xTaskGetTickCount(),
        epochMarker_ - stateSwitchedLast_,
        STATES_NAMES[estate_],
        STATES_NAMES[newstate]);

    stateSwitchedLast_ = epochMarker_;
    estate_ = newstate;
    write_int8("usrset", "State", static_cast<int8_t>(newstate));
}

void ManagementClass::switchToService() {
    normal_.estate = estate_;
    disableHighPressurePump();  // watering
    disableDrainPump();         // sewer
    disableOxyPump();           // oxygen
    write_int8("hbrg2", "dc1_duty", 0); // lights up/down
    write_int8("uled0", "state", 0);
    write_uint32("usrset", "LastServiceTime", read_uint32("rtc", "Time"));
    write_uint32("usrset", "LastServiceDate", read_uint32("rtc", "Date"));
    write_int8("usrset", "State", static_cast<int8_t>(Servicing));
    estate_ = Servicing;
    uart_printf("[%d] Switching to Service\r\n", xTaskGetTickCount());
}

void ManagementClass::switchToNormal() {
    estate_ = normal_.estate;
    write_int8("uled0", "state", 1);
    write_int8("usrset", "State", static_cast<int8_t>(normal_.estate));
    uart_printf("[%d] Switching to Normal\r\n", xTaskGetTickCount());
}

void ManagementClass::setDayLights(uint32_t tow) {
    uint32_t dayStart = read_uint32("usrset", "DayStart");
    uint32_t dayEnd = read_uint32("usrset", "DayEnd");
    if (tow < dayStart || tow > dayEnd) {
        write_int8("ledrbw", "duty0", 0);   // blue
        write_int8("ledrbw", "duty1", 0);   // ventilation
        write_int8("ledrbw", "duty2", 0);   // white
        write_int8("ledrbw", "duty3", 0);   // red/blue
        disableRelaisLight();
    } else if (tow >= dayStart && tow < (dayStart + 30*60)) {
        // sunrise 30 minutes
        float dt = static_cast<float>(tow - dayStart)/ 1800.0f;
        write_int8("ledrbw", "duty0", 
            static_cast<int8_t>(dt * read_int8("usrset", "DayDuty0")));   // blue
        //write_int8("ledrbw", "duty1",
        //    static_cast<int8_t>(dt * read_int8("usrset", "DayDuty1")));   // unused
        write_int8("ledrbw", "duty2",
            static_cast<int8_t>(dt * 90));   // white
        write_int8("ledrbw", "duty3",
            static_cast<int8_t>(dt * read_int8("usrset", "DayDuty3")));   // red/blue
    } else if (tow >= (dayEnd - 30*60) && tow < dayEnd) {
        // sunset 30 minutes
        float dt = 1.0f - static_cast<float>(tow - (dayEnd - 1800))/ 1800.0f;
        write_int8("ledrbw", "duty0", 
            static_cast<int8_t>(dt * read_int8("usrset", "DayDuty0")));   // blue
        //write_int8("ledrbw", "duty1",
        //    static_cast<int8_t>(dt * read_int8("usrset", "DayDuty1")));   // unused
        write_int8("ledrbw", "duty2",
            static_cast<int8_t>(dt * 90));   // white
        write_int8("ledrbw", "duty3",
            static_cast<int8_t>(dt * read_int8("usrset", "DayDuty3")));   // red/blue
        disableRelaisLight();
    } else {
        write_int8("ledrbw", "duty0", read_int8("usrset", "DayDuty0"));   // blue
        write_int8("ledrbw", "duty1", read_int8("usrset", "DayDuty1"));   // ventilation
        write_int8("ledrbw", "duty2", read_int8("usrset", "DayDuty2"));   // white
        write_int8("ledrbw", "duty3", read_int8("usrset", "DayDuty3"));   // red/blue
        enableRelaisLight();
    }
}

void ManagementClass::enableHighPressurePump() {
    write_int8("relais0", "State", 1);
}

void ManagementClass::disableHighPressurePump() {
    write_int8("relais0", "State", 0);
}

void ManagementClass::enableRelaisLight() {
    write_int8("relais1", "State", 1);
}

void ManagementClass::disableRelaisLight() {
    write_int8("relais1", "State", 0);
}


void ManagementClass::enableDrainPump() {
    write_int8("hbrg0", "dc0_duty", 100);
}

void ManagementClass::disableDrainPump() {
    write_int8("hbrg0", "dc0_duty", 0);
}

void ManagementClass::enableOxyPump() {
    write_int8("hbrg2", "dc0_duty", 100);
}

void ManagementClass::disableOxyPump() {
    write_int8("hbrg2", "dc0_duty", 0);
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

void ManagementClass::write_uint32(const char *objname,
                                   const char *atrname,
                                   uint32_t v) {
    char buf[5] = {static_cast<char>(v),
                   static_cast<char>(v >> 8),
                   static_cast<char>(v >> 16),
                   static_cast<char>(v >> 24)};
    write_obj_attribute(objname, atrname, buf, 4);
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

float ManagementClass::getMixWeight() {
    return read_float32("scales", "gram2");
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
