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

 #include "systicksim.h"
#include <sim_api.h>

SysTickSim::SysTickSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    CSR(static_cast<DeviceGeneric *>(this), "CSR", addr + 0x0),
    RVR(static_cast<DeviceGeneric *>(this), "RVR", addr + 0x4),
    CVR(static_cast<DeviceGeneric *>(this), "CVR", addr + 0x8),
    CALIB(static_cast<DeviceGeneric *>(this), "CALIB", addr + 0xc) {
    sysclk_hz_ = 144000000; // 1 clock 7ns
    time_ = 0;
}

void SysTickSim::update(double dt) {
    bool overflow = false;
    uint64_t cvr_prv;
    uint64_t cvr_cur;
    uint32_t rvr = RVR.get() + 1;
    if (!CSR.isEnabled()) {
        time_ = 0;
        return;
    }
    cvr_prv = static_cast<uint64_t>(time_ * sysclk_hz_);
    cvr_prv %= rvr;

    time_ += dt;
    cvr_cur = static_cast<uint64_t>(time_ * sysclk_hz_);
    cvr_cur %= rvr;
    CVR.set(rvr - static_cast<uint32_t>(cvr_cur));

    if (cvr_cur < cvr_prv && CSR.isIrqEnabled())  {
        sim_request_interrupt(-1);
    }
}

// small delta time to emulate CPU delay
uint32_t SysTickSim::CVR_TYPE::read_action(uint32_t prev) {
    prev -= 100;
    prev &= ((1 << 24) - 1);
    set(prev);
    return prev;
}
