#pragma once

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void write32(const volatile uint32_t *adr, uint32_t val);
void write64(const volatile uint64_t *adr, uint64_t val);
uint8_t read8(const volatile uint8_t *adr);
uint16_t read16(const volatile uint16_t *adr);
uint32_t read32(const volatile uint32_t *adr);
uint64_t read64(const volatile uint64_t *adr);

static void EnableIrqGlobal() {}
static void DisableIrqGlobal() {}

#ifdef __cplusplus
}  // extern "C"
#endif
