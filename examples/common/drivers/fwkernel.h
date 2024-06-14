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

#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <uart_drv.h>
#include "relais.h"
#include "ledstrip.h"
#include "can_drv.h"
#include "loadsensor.h"
#include "user_led.h"
#include "user_btn.h"
#include "adc_drv.h"
#include "hbridge.h"
#include "ds18b20_drv.h"
#include "soil_drv.h"
#include "rtc_drv.h"
#include "usrsettings.h"
#include "dbc.h"

class KernelClass : public FwObject {
 public:
    explicit KernelClass(const char *name);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

 private:
    void addToOutputList(FwAttribute *attr, uint32_t period);
    void removeFromOutputList(FwAttribute *attr);

 private:
    class TargetConfigAttribute : public FwAttribute {
     public:
        TargetConfigAttribute(KernelClass *parent, const char *name)
            : FwAttribute(name, "PnP info"), parent_(parent) {
            make_string("");
        }

        virtual void pre_read() override;
     protected:
        void print_attribute(int idx, FwAttribute *attr);
     private:
        KernelClass *parent_;
    };

    /**
        Enable or disable FwAttribute periodic output
     */
    class OutputControlAttribute : public FwAttribute {
     public:
        OutputControlAttribute(KernelClass *parent, const char *name)
            : FwAttribute(name, "Ena/dis periodic value output"), parent_(parent) {
        }

        virtual void post_write() override;

        union ValueType {
            struct bits_type {
                uint32_t objid : 8;
                uint32_t attrid : 7;
                uint32_t ena_dis : 1;   // 1=enable output; 0=disable output
                uint32_t period : 16;
            } b;
            uint32_t u32;
        };

     private:
        KernelClass *parent_;
    };

 private:
    /** @brief Kernel Version attribute */
    TargetConfigAttribute targetConfig_;
    FwAttribute version_;
    OutputControlAttribute output_;    // enable/disable specific attribute periodic output

    UartDriver uart1_;
    RelaisDriver relais0_;
    RelaisDriver relais1_;
    LedStripDriver ledrbw_;
    CanDriver can1_;
    CanDriver can2_;
    LoadSensorDriver scales_;
    UserLedDriver uled0_;
    UserButtonDriver ubtn0_;
    AdcDriver adc1_;
    HBridgeDriver hbrg0_;
    HBridgeDriver hbrg1_;
    HBridgeDriver hbrg2_;
    HBridgeDriver hbrg3_;
    Ds18b20Driver temp0_;
    SoilDriver soil0_;
    RtcDriver rtc_;
    UserSettings settings_;
    DbcConverter dbc_;        // CAN database converter
};
