/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "spisim.h"
#include <sim_api.h>

SpiSim::SpiSim(const char *name, uint64_t addr, size_t sz, int irqidx) :
    DeviceGeneric(name, addr, sz),
    irqidx_(irqidx),
    CR1(static_cast<DeviceGeneric *>(this), "CR1", addr + 0x0),
    CR2(static_cast<DeviceGeneric *>(this), "CR2", addr + 0x4),
    SR(static_cast<DeviceGeneric *>(this), "SR", addr + 0x08),
    DR(static_cast<DeviceGeneric *>(this), "DR", addr + 0x0c) {
}

void SpiSim::update(double dt) {
    if (!CR1.isEnabled()) {
        return;
    }

    /*if (CR2.isUpdateIrqEnabled()) {
        sim_request_interrupt(irqidx_);  // do not simualting precise timing for now
    }*/
}

