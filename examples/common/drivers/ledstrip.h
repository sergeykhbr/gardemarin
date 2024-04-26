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
#include <gpio_drv.h>

// Use one PWM generator TIM2_CH3 at high frequency (2..100 kHz) to form
// LED channel PWM with the frequency 200 Hz (default)
class LedStripDriver : public FwObject {
 public:
    LedStripDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

 protected:
    class DutyAttribute : public FwAttribute {
     public:
        DutyAttribute(const char *name, int idx) : FwAttribute(name), idx_(idx) {}

        virtual void pre_read() override;
        virtual void post_write() override;
     protected:
        int idx_;
    };

    class HzAttribute : public FwAttribute {
     public:
        HzAttribute(const char *name) : FwAttribute(name) {}

        virtual void post_write() override;
        virtual float getMinValue() override { return 10.0f; }
        virtual float getMaxValue() override { return 10000.0f; }
    };

 protected:
    HzAttribute tim_hz_;
    DutyAttribute duty0_;
    DutyAttribute duty1_;
    DutyAttribute duty2_;
    DutyAttribute duty3_;
};
