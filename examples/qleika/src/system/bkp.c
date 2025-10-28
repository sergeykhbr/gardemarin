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

#include <mcu.h>
#include "bkp.h"

void bkp_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    PWR_registers_type *PWR = (PWR_registers_type *)PWR_BASE;
    // adc clock on APB1 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 27)              // APB1[27] BKPEN
        | (1 << 28);             // APB1[28] PWREN
    write32(&RCC->APB1ENR, t1);

    t1 = read32(&PWR->CR);
    t1 |= (1 << 8);             // CR[8]: DBP Disable backup domain write protection. 1=Acces to RTC and Backup enabled
    write32(&PWR->CR, t1);
}

// 0x00 BKP_DR0 reserved / does not exists
// 0x04 BKP_DR1
// ..
// 0x28 BKP_DR10
int bkp_get_watering_mode() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    return BKP[3] & 0x7;    // BKP_DR3[2:0] watering mode
}

void bkp_set_watering_mode(int val) {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    BKP[3] = val & 0x7;
}

