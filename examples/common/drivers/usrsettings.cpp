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

#include <prjtypes.h>
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include "usrsettings.h"
#include <uart.h>

UserSettings::UserSettings(const char *name) : FwObject(name),
    lastServiceDate_("LastServiceDate", "rtc date"),
    lastServiceTime_("LastServiceTime", "rtc time"),
    requestToService_("RequestToService"),
    wateringPerDrain_("WateringPerDrain", "0..n counts"),
    wateringInterval_("WateringInterval", "[sec]"),
    wateringDuration_("WateringDuration", "[sec]"),
    lastWatering_("LastWatering", "rtc time"),
    oxygenSaturationInterval_("OxygenSaturationInterval", "[sec]"),
    dayStart_("DayStart", "[sec] time of day"),
    dayEnd_("DayEnd", "[sec] time of day"),
    dayDuty0_("DayDuty0", "pwm: 0..100 (blue)"),
    dayDuty1_("DayDuty1", "pwm: 0..100 (unused)"),
    dayDuty2_("DayDuty2", "pwm: 0..100 (white)"),
    dayDuty3_("DayDuty3", "pwm: 0..100 (red/blue)"),
    state_("State", "Management task state: "
        "0=WaitInit, "
        "1=CheckWateringInterval, "
        "2=DrainBefore, "
        "3=OxygenSaturation, "
        "4=Watering, "
        "5=DrainAfter, "
        "6=AdjustLights, "
        "7=Servicing") {

    lastServiceDate_.make_uint32(0);
    lastServiceTime_.make_uint32(0);
    requestToService_.make_int8(0);
    wateringPerDrain_.make_int8(10);
    wateringInterval_.make_uint16(270);
    wateringDuration_.make_uint16(30);
    lastWatering_.make_uint32(0);
    oxygenSaturationInterval_.make_uint16(30);
    dayStart_.make_uint32(6*3600);   // 6:00 AM
    dayEnd_.make_uint32(22*3600);   // 22:00
    dayDuty0_.make_int8(50);
    dayDuty1_.make_int8(0);
    dayDuty2_.make_int8(0);
    dayDuty3_.make_int8(0);
    state_.make_int8(0);
}

void UserSettings::Init() {
    RegisterAttribute(&lastServiceTime_);
    RegisterAttribute(&lastServiceDate_);
    RegisterAttribute(&requestToService_);
    RegisterAttribute(&wateringPerDrain_);
    RegisterAttribute(&wateringInterval_);
    RegisterAttribute(&wateringDuration_);
    RegisterAttribute(&lastWatering_);
    RegisterAttribute(&oxygenSaturationInterval_);
    RegisterAttribute(&dayStart_);
    RegisterAttribute(&dayEnd_);
    RegisterAttribute(&dayDuty0_);
    RegisterAttribute(&dayDuty1_);
    RegisterAttribute(&dayDuty2_);
    RegisterAttribute(&dayDuty3_);
    RegisterAttribute(&state_);
}

