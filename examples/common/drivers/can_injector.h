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
#include <IrqInterface.h>

class CanInjectorDriver : public FwObject,
                          public IrqHandlerInterface {
 public:
    explicit CanInjectorDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // IrqHandlerInterface
    virtual void handleInterrupt(int *argv) override;

    // Common methods:
    virtual void setInjection();
    virtual void releaseInjection();

 protected:
    class InjectError : public FwAttribute {
     public:
        InjectError(CanInjectorDriver *parent, const char *name)
            : FwAttribute(name) , parent_(parent) {}

        virtual void post_write() override;
     protected:
        CanInjectorDriver *parent_;
    };
 protected:
    InjectError inject_;
    bool state_;
};
