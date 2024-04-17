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

#include "uartsim.h"
#include <iostream>
#include <simutils.h>

UARTSim::UARTSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    SR(static_cast<DeviceGeneric *>(this), "SR", addr + 0x00),
    DR(static_cast<DeviceGeneric *>(this), "DR", addr + 0x04) {
}

uint32_t UARTSim::SR_TYPE::read_action(uint32_t prev) {
    uint32_t ret = 0;
    ret |= (1 << 7);    // [7] TXE transmit empty
    return ret;
}

uint32_t UARTSim::DR_TYPE::write_action(uint32_t nxt) {
    char tstr[2] = {static_cast<char>(nxt), 0};
    printf_info("%s", tstr);
    return nxt;
}
