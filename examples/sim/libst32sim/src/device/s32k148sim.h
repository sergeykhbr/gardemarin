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

#include <s32k148api.h>
#include <devgen.h>
#include <list>
#include "nvicsim.h"

enum EIsrId {
    Nmi_Total = 15,
    Interrupt_Total = 256
};

class S32K148Sim : public DeviceGeneric {
 public:
    S32K148Sim(const char *name);

    void runFirmware(void *);
    void stopFirmware();
    void registerIsr(int idx, isr_type handler);
    void requestIrq(int idx);
    int setOption(int optname, void *optval);

 private:
    void handleInterrupts();

 private:
    std::list<DeviceGeneric *> devlist_;
    void *hFwThread_;
    void *eventIsrAsync_;
    bool enabled_;
    isr_type vector_[Interrupt_Total + Nmi_Total];
    NVICSim *nvic_;
    int nmi_request_;
};
