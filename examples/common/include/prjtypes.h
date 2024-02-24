/*
 *  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*irq_handler_type)(void);

#ifdef WIN32
#define __attribute__(x)

#define RV_PRI64 "I64"

void write8(const volatile uint8_t *adr, uint8_t val);
void write16(const volatile uint16_t *adr, uint16_t val);
void write32(const volatile uint32_t *adr, uint32_t val);
void write64(const volatile uint64_t *adr, uint64_t val);
uint8_t read8(const volatile uint8_t *adr);
uint16_t read16(const volatile uint16_t *adr);
uint32_t read32(const volatile uint32_t *adr);
uint64_t read64(const volatile uint64_t *adr);

static void EnableIrqGlobal() {}
static void DisableIrqGlobal() {}
static inline void memory_barrier() {}

#else

#define RV_PRI64 "ll"

static inline void write8(const volatile uint8_t *adr, uint8_t val) {
    *((volatile uint8_t *)adr) = val;
}

static inline void write16(const volatile uint16_t *adr, uint16_t val) {
    *((volatile uint16_t *)adr) = val;
}

static inline void write32(const volatile uint32_t *adr, uint32_t val) {
    *((volatile uint32_t *)adr) = val;
}

static inline void write64(const volatile uint64_t *adr, uint64_t val) {
    *((volatile uint64_t *)adr) = val;
}

static inline uint8_t read8(const volatile uint8_t *adr) {
    return adr[0];
}

static inline uint16_t read16(const volatile uint16_t *adr) {
    return adr[0];
}

static inline uint32_t read32(const volatile uint32_t *adr) {
    return adr[0];
}

static inline uint64_t read64(const volatile uint64_t *adr) {
    return adr[0];
}

static inline void EnableIrqGlobal(void)
{
#if defined (__GNUC__) 
    __asm volatile ("cpsie i" : : : "memory");
#else
    __asm("cpsie i")
#endif
}

static inline void DisableIrqGlobal(void)
{
#if defined (__GNUC__)
    __asm volatile ("cpsid i" : : : "memory");
#else
    __asm("cpsid i")
#endif
}


static inline void memory_barrier() {
   __asm("DSB");
}

#endif              

#ifdef __cplusplus
}
#endif
