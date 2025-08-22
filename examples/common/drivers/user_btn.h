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
#include <TimerInterface.h>
#include <KeyInterface.h>

#define BTN_EVENT_PRESSED  0x01
#define BTN_EVENT_RELEASED 0x02

#define BTN_POS_PRESSED true
#define BTN_POS_RELEASED false

class UserButtonDriver : public FwObject,
                         public IrqHandlerInterface,
                         public TimerListenerInterface,
                         public KeyInterface {
 public:
    explicit UserButtonDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // IrqHandlerInterface
    virtual void handleInterrupt(int *argv) override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() override { return 1; }
    virtual void callbackTimer(uint64_t tickcnt) override;

    // KeyInterface
    virtual void registerKeyListener(KeyListenerInterface *iface) override;

 protected:
    uint32_t event_;
    uint64_t ms_cnt_;
    bool keyPosition_;
    bool keyPositionPrev_;
    uint64_t keyPositionChangedMs_;
    FwList *listener_;
};
