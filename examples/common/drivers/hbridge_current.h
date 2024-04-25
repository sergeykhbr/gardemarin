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

#include <gardemarin.h>
#include <prjtypes.h>
#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <SensorInterface.h>


class SensorCurrent {
 public:
    SensorCurrent(FwObject *parent, int idx, const char *adcport);

    // Common interface
    void Init();
    void PostInit();
    int32_t getRawValue();

 protected:
    class AdcValueAttribute : public FwAttribute {
     public:
        explicit AdcValueAttribute(SensorCurrent *parent, const char *name)
            : FwAttribute(name, "ADC I-sensor raw value"), parent_(parent) {
        }

        virtual void pre_read() override {
            u_.i32 = parent_->getRawValue();
        }
        SensorCurrent *parent_;
    };

    class AmpereAttribute : public FwAttribute {
     public:
        explicit AmpereAttribute(SensorCurrent *parent, const char *name)
            : FwAttribute(name, "I-sensor mA"), parent_(parent) {
        }

        virtual void pre_read() override {
            u_.f = static_cast<float>(parent_->getRawValue());
        }
        SensorCurrent *parent_;
    };

 protected:
    FwObject *parent_;
    int idx_;     // sensor index 0..1
    const char *adcport_;
    SensorInterface *isensor_;

    AdcValueAttribute value_;
    AmpereAttribute ampere_;
};
