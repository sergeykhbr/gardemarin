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

#include "rtcsim.h"
#include <iostream>
#include <simutils.h>
#include <sim_api.h>

RtcSim::RtcSim(const char *name, uint64_t addr, size_t sz, int irqidx) :
    DeviceGeneric(name, addr, sz),
    TR(static_cast<DeviceGeneric *>(this), "TR", addr + 0x00),
    DR(static_cast<DeviceGeneric *>(this), "DR", addr + 0x04),
    CR(static_cast<DeviceGeneric *>(this), "CR", addr + 0x08),
    ISR(static_cast<DeviceGeneric *>(this), "ISR", addr + 0x0C) {
    irqidx_ = irqidx;
    subsec_ = 0;
}

void RtcSim::update(double dt) {
    subsec_ += dt;
    if (subsec_ >= 1.0) {
        subsec_ -= 1.0;

    }
}

uint32_t RtcSim::getDate() {
    return DR.get();
}

uint32_t RtcSim::getTime() {
    return TR.get();
}


uint32_t RtcSim::ISR_TYPE::read_action(uint32_t prev) {
    uint32_t ret = prev;
    // [7] INIT rw
    // [6] INITF r
    if (regvalue_ & (1 << 7)) {
        ret |= (1 << 6);
    } else {
        ret &= ~(1 << 6);
    }
    return ret;
}

