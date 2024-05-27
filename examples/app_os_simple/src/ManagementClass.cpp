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
            switchToState(CheckMoisture);
        }
        break;
    case CheckMoisture:
        if (isPeriodExpired(30 * 60)
            || (isPeriodExpired(10 * 60) && read_uint16("soil0", "moisture") < 750)) {
            switchToState(DrainBefore);
            write_int8("hbrg0", "dc0_duty", 100);
        }
        break;
    case DrainBefore:
        if (isPeriodExpired(10)) {
            switchToState(OxygenSaturation);
            write_int8("hbrg0", "dc0_duty", 0);
            write_int8("hbrg2", "dc0_duty", 100);
        }
        break;
    case OxygenSaturation:
        if (isPeriodExpired(30)) {
            switchToState(Watering);
            lastWatering_.make_uint32(epochCnt_);
            write_int8("hbrg2", "dc0_duty", 0);
            write_int8("relais0", "state", 1);
        }
        break;
    case Watering:
        if (isPeriodExpired(20)) {
            write_int8("relais0", "state", 0);
            write_int8("hbrg0", "dc0_duty", 100);
            switchToState(DrainAfter);
        }
        break;
    case DrainAfter:
        if (isPeriodExpired(10)) {
            write_int8("hbrg0", "dc0_duty", 0);
            switchToState(CheckMoisture);
        }
        break;

    case Servicing:
        // Do nothing
        write_int8("uled0", "state", static_cast<int8_t>(epochCnt_ & 0x1));
        break;
    default:
        estate_ = WaitInit;
    }
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
    write_int8("uled0", "state", 0);
    estate_ = Servicing;
    uart_printf("[%d] Switching to Service\r\n", xTaskGetTickCount());
}

void ManagementClass::switchToNormal() {
    estate_ = normal_.estate;
    write_int8("uled0", "state", 1);
    uart_printf("[%d] Switching to Normal\r\n", xTaskGetTickCount());
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
