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
#include <BinInterface.h>
#include <SensorInterface.h>
#include <TimerInterface.h>
#include <gpio_drv.h>
#include "hbridge_current.h"
#include "hbridge_dcmotor.h"

class HBridgeDriver : public FwObject,
                      public BinInterface,
                      public TimerListenerInterface {
 public:
    HBridgeDriver(const char *name, int idx);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // BinInterface: control "DRV_MODE" pin output
    virtual void setBinEnabled() override;
    virtual void setBinDisabled() override;
    virtual uint8_t getBinState() override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() { return 10; }
    virtual void callbackTimer(uint64_t tickcnt) override;

    // Common methods:
    void startDcMotor(int idx, int8_t direction);
    void stopDcMotor(int idx);

 protected:
    class DriveModeAttribute : public FwAttribute {
     public:
        DriveModeAttribute(HBridgeDriver *parent, const char *name) :
            FwAttribute(name, "Drive mode pin: 0=4-pins control; 1=2-pins control"),
            parent_(parent) {
            kind_ = Attr_Int8;
        }

        virtual void pre_read() override;
        virtual void post_write() override;
     protected:
        HBridgeDriver *parent_;
    };

 protected:
    int idx_;
    DriveModeAttribute drvmode_;

    DcMotor dc0_;
    DcMotor dc1_;
    SensorCurrent i0_;
    SensorCurrent i1_;
};
