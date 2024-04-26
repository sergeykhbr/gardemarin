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

SensorCurrent::SensorCurrent(FwObject *parent, const char *name, const char *adcport)
    : FwAttribute(name, "I-sensor mA"), 
    parent_(parent),
    adcport_(adcport),
    isensor_(0) {
    make_int32(0);
}

void SensorCurrent::Init() {
    parent_->RegisterAttribute(this);
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

void SensorCurrent::pre_read() {
    // TODO: ADC value to mA
    make_int32(getRawValue());
}
