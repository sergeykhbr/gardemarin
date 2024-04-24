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
#include <TimerInterface.h>
#include <gpio_drv.h>

class UserLedDriver : public FwObject,
                      public BinInterface,
                      public TimerListenerInterface {
 public:
    UserLedDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // BinInterface
    virtual void setBinEnabled() override;
    virtual void setBinDisabled() override;
    virtual uint8_t getBinState() override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() override;
    virtual void callbackTimer(uint64_t tickcnt) override;

 protected:
    FwAttribute state_;
    FwAttribute hz_;
    int cnt_;
};

