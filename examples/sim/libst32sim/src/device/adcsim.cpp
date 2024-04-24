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

 #include "adcsim.h"
#include <sim_api.h>

AdcSim::AdcSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    SR(static_cast<DeviceGeneric *>(this), "SR", addr + 0x0),
    CR1(static_cast<DeviceGeneric *>(this), "CR1", addr + 0x4),
    CR2(static_cast<DeviceGeneric *>(this), "CR2", addr + 0x8),
    DR(static_cast<DeviceGeneric *>(this), "DR", addr + 0x4c) {
    seqcnt_ = 0;
}

void AdcSim::update(double dt) {
    if (!CR2.isEnabled()) {
        return;
    }

    if (CR1.isIrqEnabled()) {
        DR.set(0x10 * seqcnt_);
        sim_request_interrupt(18);
        if (++seqcnt_ >= 11) {
            seqcnt_ = 0;
            SR.setEndOfSequence();
        }
    }
}

