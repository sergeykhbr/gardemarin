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
#pragma once

#include "CommonInterface.h"

class SensorInterface : public CommonInterface {
 public:
    SensorInterface() : CommonInterface("SensorInterface") {}

    // set value from external interface
    virtual void setSensorValue(uint32_t val) = 0;
    // linear polynom of 1-st order: physical = a0 + a1 * adc
    virtual void setSensorOffset(uint32_t offset) = 0;
    virtual void setSensorAlpha(double alpha) = 0;
    // get ADC and convertd values
    virtual uint32_t getSensorValue() = 0;
    virtual double getSensorPhysical() = 0;
};

