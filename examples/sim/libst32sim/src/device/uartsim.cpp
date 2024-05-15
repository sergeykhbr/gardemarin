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
#include <sim_api.h>

UARTSim::UARTSim(const char *name, uint64_t addr, size_t sz, int irqidx) :
    DeviceGeneric(name, addr, sz),
    SR(static_cast<DeviceGeneric *>(this), "SR", addr + 0x00),
    DR(static_cast<DeviceGeneric *>(this), "DR", addr + 0x04),
    CR1(static_cast<DeviceGeneric *>(this), "CR1", addr + 0x0C) {
    irqidx_ = irqidx;
    txempty_ = true;
}

void UARTSim::update(double dt) {
    bool wasempty = txempty_;
    txempty_ = true;
    if (CR1.isTxEmptyIrqEna()
        || (!wasempty && CR1.isTxCompleteIrqEna())) {
        sim_request_interrupt(irqidx_);
    }
}


uint32_t UARTSim::SR_TYPE::read_action(uint32_t prev) {
    uint32_t ret = 0;
    ret |= (1 << 7);    // [7] TXE transmit empty
    return ret;
}

uint32_t UARTSim::DR_TYPE::write_action(uint32_t nxt) {
    static_cast<UARTSim *>(parent_)->clearTxEmpty();
    char tstr[2] = {static_cast<char>(nxt), 0};
    if ((memAddr_ & ~0xFFF) == 0x40011000) { //USART1_BASE)
        printf_info("%s", tstr);
    }
    return nxt;
}
