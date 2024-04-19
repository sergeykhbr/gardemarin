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

 protected:
    class BinAttribute : public FwAttribute {
     public:
        explicit BinAttribute(const char *name, BinInterface *ibin) :
            FwAttribute(name,"0=ena, 1=dis"), ibin_(ibin) {
            make_int8(0);
        }

        virtual void post_write() override {
            if (to_int8()) {
                ibin_->setBinEnabled();
            } else {
                ibin_->setBinDisabled();
            }
        }
     private:
        BinInterface *ibin_;
    };


 protected:
    int idx_;
    BinAttribute state_;
};
