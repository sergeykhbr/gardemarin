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

#include <prjtypes.h>
#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <BinInterface.h>
#include <gpio_drv.h>

class RelaisDriver : public FwObject,
                     public BinInterface {
 public:
    RelaisDriver(const char *name, int instidx);

    // FwObject interface:
    virtual void Init() override;

    // BinInterface:
    virtual void setBinEnabled() override;
    virtual void setBinDisabled() override;
    virtual uint8_t getBinState() override;

 protected:
    class BinAttribute : public FwAttribute {
     public:
        explicit BinAttribute(RelaisDriver *parent, const char *name) :
            FwAttribute(name,"0=ena, 1=dis"), parent_(parent) {
        }

        virtual void pre_read() override {
            u_.u8 = parent_->getBinState();
        }

        virtual void post_write() override {
            if (u_.i8) {
                parent_->setBinEnabled();
            } else {
                parent_->setBinDisabled();
            }
        }

        virtual float getMaxValue() override { return 1.0f; }
     private:
        RelaisDriver *parent_;
    };


 protected:
    int idx_;
    BinAttribute state_;
};
