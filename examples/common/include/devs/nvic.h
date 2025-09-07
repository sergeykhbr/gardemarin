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
#include <scs.h>

typedef struct NVIC_registers_type {
    volatile uint32_t ISER[16];  // 0xE000E100 [RW] Interrupt Set-Enable Registers
    uint8_t rsrv1[0x40];
    volatile uint32_t ICER[16];  // 0xE000E180 [RW] Interrupt Clear-Enable Registers
    uint8_t rsrv2[0x40];
    volatile uint32_t ISPR[16];  // 0xE000E200 [RW] Interrupt Set-Pending Registers
    uint8_t rsrv3[0x40];
    volatile uint32_t ICPR[16];  // 0xE000E280 [RW] Interrupt Clear-Pending Registers
    uint8_t rsrv4[0x40];
    volatile uint32_t IABR[16];  // 0xE000E300 [RO] Interrupt Active Bit Registers
    uint8_t rsrv5[0xC0];
    volatile uint8_t IPR[97];    // 0xE000E400 [RW] Interrupt Priority Registers
} NVIC_registers_type;

// All interrupts configures as preemptive
// prio = 0 highest priority
// prio = 7 lowest
static inline void nvic_irq_enable(int idx, uint8_t prio) {
    NVIC_registers_type *NVIC = (NVIC_registers_type *)NVIC_BASE;
    if (idx == -1) {
        // Enable all interrupts
        write32(&NVIC->ISER[0], ~0ul);
        write32(&NVIC->ISER[1], ~0ul);
        write32(&NVIC->ISER[2], ~0ul);
        write32(&NVIC->ISER[3], ~0ul);
    } else {
        write32(&NVIC->ISER[idx >> 5], (1ul << (idx & 0x1F)));
        // FreeRTOS demo does not use lower 4 bits
        write8(&NVIC->IPR[idx], prio << 4);
    }
}

static inline void nvic_irq_disable(int idx) {
    NVIC_registers_type *NVIC = (NVIC_registers_type *)NVIC_BASE;
    if (idx == -1) {
        // Disable all interrupts
        write32(&NVIC->ICER[0], ~0ul);
        write32(&NVIC->ICER[1], ~0ul);
        write32(&NVIC->ICER[2], ~0ul);
        write32(&NVIC->ICER[3], ~0ul);
    } else {
        write32(&NVIC->ICER[idx >> 5], (1ul << (idx & 0x1F)));
    }
}

static inline void nvic_irq_clear(int idx) {
    NVIC_registers_type *NVIC = (NVIC_registers_type *)NVIC_BASE;
    if (idx == -1) {
        // Clear all pending interrupts
        write32(&NVIC->ICPR[0], ~0ul);
        write32(&NVIC->ICPR[1], ~0ul);
        write32(&NVIC->ICPR[2], ~0ul);
        write32(&NVIC->ICPR[3], ~0ul);
    } else {
        write32(&NVIC->ICPR[idx >> 5], (1ul << (idx & 0x1F)));
    }
}
