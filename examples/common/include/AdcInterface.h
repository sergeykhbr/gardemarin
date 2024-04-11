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

class AdcInterface : public CommonInterface {
 public:
    AdcInterface() : CommonInterface("AdcInterface") {}

    // Non-modified  ADC value
    virtual uint32_t getAdcValue() = 0;

    // Normalized to transformed value representing physical process (A, mg or other)
    virtual double getAdcNormalized() = 0;

    // linear polynom of 1-st order: physical = a0 + a1 * adc
    virtual void setAdcLinearRate0(double a0) = 0;
    virtual void setAdcLinearRate1(double a1) = 0;
};

