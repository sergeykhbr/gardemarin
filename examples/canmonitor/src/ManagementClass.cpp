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
#include <vprintfmt.h>


ManagementClass::ManagementClass(TaskHandle_t taskHandle)
    : FwObject("man"),
    taskHandle_(taskHandle) {
    disp0_ = 0;
    btnClick_ = false;
    updateCnt_ = 0;
    errCntBus0_ = 0;
    errCntBus1_ = 0;
    estate_ = State_SplashScreen;
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

    disp0_ = reinterpret_cast<DisplayInterface *>(
        fw_get_object_interface("disp0", "DisplayInterface"));
}


void ManagementClass::update() {
    uint32_t t1;
    int btnClick = btnClick_;
    btnClick_ = 0;

    //uint32_t can2_rxcnt = read_uint32("can2", "rxcnt");
    switch (estate_) {
    case State_SplashScreen:
        if (++updateCnt_ >= 5) {
            estate_ = State_CanListener;
            if (disp0_) {
                disp0_->clearScreen();
                disp0_->outputText24Line("PGM:     -         ", 0, 0, 0xFFFF, 0x61d0);
                disp0_->outputText24Line("CAN0 Rx: -", 1, 0, 0xBFE6, 0x0000);
                disp0_->outputText24Line(" ErrCnt: -", 2, 0, 0xFAAA, 0x0000);
                //disp0_->outputText24Line("   Mode: -", 3, 0, 0x13F6, 0x0000);
                disp0_->outputText24Line("CAN1 Rx: -", 5, 0, 0xBFE6, 0x0000);
                disp0_->outputText24Line(" ErrCnt: -", 6, 0, 0xFAAA, 0x0000);
                //disp0_->outputText24Line("   Mode: -", 7, 0, 0x13F6, 0x0000);
            }
        }
        break;
    case State_CanListener:
        if (disp0_) {
            char tstr[20];
            snprintf_lib(tstr, static_cast<int>(sizeof(tstr)), "%d  ",
                        read_int8("can1", "pgm"));
            disp0_->outputText24Line(tstr, 0, 9, 0xFFFF, 0x61d0);

            t1 = read_uint32("can1", "rxcnt");
            snprintf_lib(tstr, static_cast<int>(sizeof(tstr)), "%d", t1);
            disp0_->outputText24Line(tstr, 1, 9, 0xffff, 0x0000);

            t1 = read_uint32("can1", "errcnt");
            snprintf_lib(tstr, static_cast<int>(sizeof(tstr)), "%d          ", t1);
            tstr[12] = 0;
            if (t1 != errCntBus0_) {
                disp0_->outputText24Line(tstr, 2, 9, 0xEF3B, 0x9186);
            } else {
                disp0_->outputText24Line(tstr, 2, 9, 0xFAAA, 0x0000);
            }
            errCntBus0_ = t1;

            t1 = read_uint32("can1", "mode");
            if (t1 == 0) {
                disp0_->outputText24Line("Bus OFF           ", 3, 1, 0xF7E7, 0x0000);
            } else if (t1 == 2) {
                disp0_->outputText24Line("Bus Listener      ", 3, 1, 0x47EB, 0x0000);
            } else if (t1 == 3) {
                t1 = read_uint32("inj0", "cnt");
                snprintf_lib(tstr, static_cast<int>(sizeof(tstr)), "%d         ", t1);
                disp0_->outputText24Line("Inject: ", 3, 1, 0xFBBB, 0x0000);
                disp0_->outputText24Line(tstr, 3, 9, 0xFBBB, 0xAC00);

                // Show Last injected error code
                t1 = read_uint32("can1", "lasterr");
                // ESR[4:6] = LEC LAst error code
                switch ((t1 >> 4) & 0x7) {
                case 0:
                    disp0_->outputText24Line("No Error         ", 4, 1, 0x13F6, 0x0000);
                    break;
                case 1:
                    disp0_->outputText24Line("Stuff bit err    ", 4, 1, 0x13F6, 0x0000);
                    break;
                case 2:
                    disp0_->outputText24Line("Form err         ", 4, 1, 0x13F6, 0x0000);
                    break;
                case 3:
                    disp0_->outputText24Line("ACK err          ", 4, 1, 0x13F6, 0x0000);
                    break;
                case 4:
                    disp0_->outputText24Line("Recessive bit err", 4, 1, 0x13F6, 0x0000);
                    break;
                case 5:
                    disp0_->outputText24Line("Dominant bit err ", 4, 1, 0x13F6, 0x0000);
                    break;
                case 6:
                    disp0_->outputText24Line("CRC err          ", 4, 1, 0x13F6, 0x0000);
                    break;
                default:
                    disp0_->outputText24Line("Err set by SW    ", 4, 1, 0x13F6, 0x0000);
                }
            } else {
                disp0_->outputText24Line("No Data           ", 3, 1, 0x6B6D, 0x0000);
            }


            t1 = read_uint32("can2", "rxcnt");
            snprintf_lib(tstr, static_cast<int>(sizeof(tstr)), "%d", t1);
            disp0_->outputText24Line(tstr, 5, 9, 0xffff, 0x0000);

            t1 = read_uint32("can2", "errcnt");
            snprintf_lib(tstr, static_cast<int>(sizeof(tstr)), "%d          ", t1);
            tstr[12] = 0;
            if (t1 != errCntBus1_) {
                disp0_->outputText24Line(tstr, 6, 9, 0xEF3B, 0x9186);
            } else {
                disp0_->outputText24Line(tstr, 6, 9, 0xFAAA, 0x0000);
            }
            errCntBus1_ = t1;

            t1 = read_uint32("can2", "mode");
            if (t1 == 0) {
                disp0_->outputText24Line("Bus OFF           ", 7, 1, 0xF7E7, 0x0000);
            } else if (t1 == 2) {
                disp0_->outputText24Line("Bus Listener      ", 7, 1, 0x47EB, 0x0000);
            } else {
                disp0_->outputText24Line("No Data           ", 8, 1, 0x6B6D, 0x0000);
            }
        }
        break;
    default:;
    }

}

void ManagementClass::keyPressed() {
    xTaskNotify(taskHandle_,
                0,
                eNoAction);
    btnClick_ = true;

    uint32_t canmod = read_uint32("can1", "mode");
    if (canmod == 2) {
        write_uint32("can1", "mode", 3);
        write_uint32("inj0", "inject", 1);
    } else if (canmod == 3) {
        write_uint32("can1", "mode", 2);
        write_uint32("inj0", "inject", 0);
    }

}

void ManagementClass::keyReleased() {
}

void ManagementClass::waitKeyPressed() {
    uint32_t notifiedValue = 0;
    xTaskNotifyStateClear(taskHandle_);
    xTaskNotifyWait(0x00,   // don't clear any notification bits on entry
                    0xffffffffUL,  // Reset the notification value to 0 on exit
                    &notifiedValue,
                    portMAX_DELAY); // Block indefinetly
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
