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
#include "rtc_drv.h"
#include "dbc.h"

class KernelClassGeneric : public FwObject {
 public:
    explicit KernelClassGeneric(const char *name);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

 protected:
    void addToOutputList(FwAttribute *attr, uint32_t period);
    void removeFromOutputList(FwAttribute *attr);

 protected:
    class TargetConfigAttribute : public FwAttribute {
     public:
        TargetConfigAttribute(KernelClassGeneric *parent, const char *name)
            : FwAttribute(name, "PnP info"), parent_(parent) {
            make_string("");
        }

        virtual void pre_read() override;
     protected:
        void print_attribute(int idx, FwAttribute *attr);
     private:
        KernelClassGeneric *parent_;
    };

    /**
        Enable or disable FwAttribute periodic output
     */
    class OutputControlAttribute : public FwAttribute {
     public:
        OutputControlAttribute(KernelClassGeneric *parent, const char *name)
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
        KernelClassGeneric *parent_;
    };

 protected:
    /** @brief Kernel Version attribute */
    TargetConfigAttribute targetConfig_;
    FwAttribute version_;
    OutputControlAttribute output_;    // enable/disable specific attribute periodic output

    UartDriver uart1_;
    RtcDriver rtc_;
    DbcConverter dbc_;        // CAN database converter
};
