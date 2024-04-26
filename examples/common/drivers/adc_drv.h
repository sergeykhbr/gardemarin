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
#include <TimerInterface.h>
#include <gpio_drv.h>

class AdcChannel : public FwAttribute,
                   public SensorInterface {
 public:
    AdcChannel(FwObject *parent, const char *name, int idx, const char *descr);

    // FwAttribute
    virtual void pre_read() override ;
    virtual float getMaxValue() override { return 3.3f; }
    virtual float getScaleRateValue() override { return 1000.0f; }


    // SensorInterface
    virtual int32_t getSensorValue() override;

    // Common methods
    void Init();

  protected:
    FwObject *parent_;
    int idx_;
};

class TemperatureAdcChannel : public AdcChannel {
 public:
    TemperatureAdcChannel(FwObject *parent, int idx)
        : AdcChannel(parent, "temperature", idx, "CPU temperature") {
    }

    virtual void pre_read() override {
        AdcChannel::pre_read();
        // "DS8626 Rev 9", page 138, table 69. Temperature sensor characterisitcs
        float V_SENSE = static_cast<float>(u_.i32) / 
            static_cast<float>(AdcChannel::getScaleRateValue());
        static const float V25 = 0.76f;            // Voltage at 25 C [V]
        static const float Avg_Slope = 0.0025f;    // 2.5 [mV/C]

        u_.i32 = static_cast<int>(
            getScaleRateValue() * (V_SENSE - V25) / Avg_Slope) + 250;
    }

    virtual float getMinValue() override { return -40.0f; }
    virtual float getMaxValue() override { return 110.0f; }
    virtual float getScaleRateValue() override { return 10.0f; }
    virtual const char *getUnits() override { return "C"; }

};

class AdcDriver : public FwObject {
 public:
    AdcDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

 protected:
    AdcChannel in0_;
    AdcChannel in3_;
    AdcChannel in4_;
    AdcChannel in5_;
    AdcChannel in6_;
    AdcChannel in8_;
    AdcChannel in9_;
    AdcChannel in10_;
    TemperatureAdcChannel temp_;   // temperature
    AdcChannel vint_;   // V_INT
    AdcChannel vbat_;   // V_BAT
};

