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

#include <s32k148api.h>
#include "device/s32k148sim.h"

extern "C" {

static S32K148Sim *s32k = 0;

void s32k148_init() {
    s32k = new S32K148Sim("s32k148");
}

void s32k148_destroy() {
    s32k->stopFirmware();
    delete s32k;
}

void s32k148_register_isr(int idx, isr_type handler) {
    s32k->registerIsr(idx, handler);
}

void s32k148_run_firmware(void *entry) {
    s32k->runFirmware(entry);
}

void s32k148_request_interrupt(int idx) {
    s32k->requestIrq(idx);
}

/** Set simulation model option */
int s32k148_set_opt(int optname, void *optval)
{
    return s32k->setOption(optname, optval);
}

void write8(const volatile uint8_t *adr, uint8_t val) {
    MemoryOperationType memop;
    memop.type = Memop_Write;
    memop.size = 1;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    memcpy(memop.payload.u8, &val, sizeof(val));
    s32k->operation(&memop);
}

void write16(const volatile uint16_t *adr, uint16_t val) {
    MemoryOperationType memop;
    memop.type = Memop_Write;
    memop.size = 2;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    memcpy(memop.payload.u8, &val, sizeof(val));
    s32k->operation(&memop);
}

void write32(const volatile uint32_t *adr, uint32_t val) {
    MemoryOperationType memop;
    memop.type = Memop_Write;
    memop.size = 4;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    memcpy(memop.payload.u8, &val, sizeof(val));
    s32k->operation(&memop);
}

void write64(const volatile uint64_t *adr, uint64_t val) {
    MemoryOperationType memop;
    memop.type = Memop_Write;
    memop.size = 8;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    memcpy(memop.payload.u8, &val, sizeof(val));
    s32k->operation(&memop);
}

uint8_t read8(const volatile uint8_t *adr) {
    MemoryOperationType memop;
    memop.type = Memop_Read;
    memop.size = 1;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    s32k->operation(&memop);
    return memop.payload.u8[0];

}

uint16_t read16(const volatile uint16_t *adr) {
    MemoryOperationType memop;
    memop.type = Memop_Read;
    memop.size = 2;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    s32k->operation(&memop);
    return memop.payload.u16[0];

}

uint32_t read32(const volatile uint32_t *adr) {
    MemoryOperationType memop;
    memop.type = Memop_Read;
    memop.size = 4;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    s32k->operation(&memop);
    return memop.payload.u32[0];
}

uint64_t read64(const volatile uint64_t *adr) {
    MemoryOperationType memop;
    memop.type = Memop_Read;
    memop.size = 8;
    memop.addr = reinterpret_cast<uint64_t>(adr) & 0xFFFFFFFFull;
    s32k->operation(&memop);
    return memop.payload.u64;
}

}  // extern "C"
