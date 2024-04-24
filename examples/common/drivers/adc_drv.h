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

class AdcChannel : public SensorInterface {
 public:
    AdcChannel(FwObject *parent, const char *name, int idx);

    // SensorInterface
    virtual void setSensorOffset(uint32_t offset) override {}
    virtual void setSensorAlpha(double alpha) override {}
    virtual uint32_t getSensorValue() override;
    virtual double getSensorPhysical() override { return 0;}

    // Common methods
    void Init();

  protected:
    FwObject *parent_;
    const char *name_;
    int idx_;
};

class AdcDriver : public FwObject {
friend class TemperatureAttribute;
friend class VoltageRefAttribute;
 public:
    AdcDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

 protected:
    class TemperatureAttribute : public FwAttribute {
     public:
        explicit TemperatureAttribute(AdcDriver *parent, const char *name)
            : FwAttribute(name, "CPU temperature"), parent_(parent) {
        }

        virtual void pre_read() override {
            u_.u32 = parent_->temp_.getSensorValue();
        }
        AdcDriver *parent_;
    };

    class VoltageRefAttribute : public FwAttribute {
     public:
        explicit VoltageRefAttribute(AdcDriver *parent, const char *name)
            : FwAttribute(name, "CPU Internal Voltage"), parent_(parent) {
        }

        virtual void pre_read() override {
            u_.u32 = parent_->vint_.getSensorValue();
        }
        AdcDriver *parent_;
    };

    class BatteryAttribute : public FwAttribute {
     public:
        explicit BatteryAttribute(AdcDriver *parent, const char *name)
            : FwAttribute(name, "Battery Voltage"), parent_(parent) {
        }

        virtual void pre_read() override {
            u_.u32 = parent_->vbat_.getSensorValue();
        }
        AdcDriver *parent_;
    };

 protected:
    AdcChannel in0_;
    AdcChannel in3_;
    AdcChannel in4_;
    AdcChannel in5_;
    AdcChannel in6_;
    AdcChannel in8_;
    AdcChannel in9_;
    AdcChannel in10_;
    AdcChannel temp_;   // temperature
    AdcChannel vint_;   // V_INT
    AdcChannel vbat_;   // V_BAT

    TemperatureAttribute temperature_;
    VoltageRefAttribute voltageRef_;
    BatteryAttribute voltageBattery_;
};

