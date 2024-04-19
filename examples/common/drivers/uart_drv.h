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
#include <fwfifo.h>
#include <FwAttribute.h>
#include <TimerInterface.h>
#include <RawInterface.h>

class UartDriver : public FwObject,
                   public TimerListenerInterface,
                   public RawInterface {
 public:
    explicit UartDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() override { return 1; }
    virtual void callbackTimer(uint64_t tickcnt) override;

    // RawInterface
    virtual void WriteData(const char *buf, int sz) override;
    virtual void RegisterRawListener(RawListenerInterface *iface) override;

 protected:
    FwList *listener_;

    FwFifo rxfifo_;
    char rxbuf_[32];
    int rxcnt_;
};
