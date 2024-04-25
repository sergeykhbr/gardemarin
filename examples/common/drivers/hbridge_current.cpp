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
#include <fwapi.h>
#include "hbridge_current.h"

struct CurrentSensorNamesType {
    const char *portname;
    const char *value_name;
    const char *ampere_name;
};

static const CurrentSensorNamesType CURRENT_SENSOR_CFG[GARDEMARIN_DCMOTOR_PER_HBDRIGE] = {
    {"i0", "i0_value", "i0_ampere"},
    {"i1", "i1_value", "i1_ampere"}
};


SensorCurrent::SensorCurrent(FwObject *parent, int idx, const char *adcport) :
    parent_(parent),
    idx_(idx),
    adcport_(adcport),
    isensor_(0),
    value_(this, CURRENT_SENSOR_CFG[idx].value_name),
    ampere_(this, CURRENT_SENSOR_CFG[idx].ampere_name) {
}

void SensorCurrent::Init() {
    parent_->RegisterAttribute(&value_);
    parent_->RegisterAttribute(&ampere_);
}

void SensorCurrent::PostInit() {
    isensor_ =
        reinterpret_cast<SensorInterface *>(fw_get_object_port_interface(
                "adc1",
                adcport_,
                "SensorInterface"));
}

int32_t SensorCurrent::getRawValue() {
    uint32_t ret = 0;
    if (isensor_) {
        ret = isensor_->getSensorValue();
    }
    return ret;
}
