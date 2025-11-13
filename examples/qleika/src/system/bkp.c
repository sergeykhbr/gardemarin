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
    uint32_t t = BKP[3] & ~0x7;
    t |= val & 0x7;
    BKP[3] = t;
}

// 0 = PWM off
// 25 maximum duty cycle
uint32_t bkp_get_pwm_duty() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    uint32_t ret = (BKP[3] >> 4) & 0x1F;    // BKP_DR3[8:4] light PWM
    if (ret > LED_DUTY_CYCLE_MAX) {
        ret = LED_DUTY_CYCLE_MAX;
    }
    return ret;
}

void bkp_set_pwm_duty(uint32_t val) {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    uint32_t t = BKP[3] & ~0x1F0;
    if (val > LED_DUTY_CYCLE_MAX) {
        val = LED_DUTY_CYCLE_MAX;
    }
    t |= val << 4;
    BKP[3] = t;
}

int bkp_is_rtc_initilized() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    return BKP[4] == 0xcafe ? 1: 0;
}

void bkp_set_rtc_initialized() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    BKP[4] = 0xcafe;
}

int bkp_get_rtc_correction() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    int t1 = BKP[5];
    if (t1 & 0x8000) {
        t1 |= 0xFFFF0000;
    }
    return t1;
}

void bkp_set_rtc_correction(int rtc_corr) {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    BKP[5] = rtc_corr;
}

int bkp_get_temperature_correction() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    int t1 = BKP[6];
    if (t1 & 0x8000) {
        t1 |= 0xFFFF0000;
    }
    return t1;
}

void bkp_set_temperature_correction(int t_corr) {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    BKP[6] = t_corr;
}

int bkp_get_pressure_correction() {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    int t1 = BKP[7];
    if (t1 & 0x8000) {
        t1 |= 0xFFFF0000;
    }
    return t1;
}

void bkp_set_pressure_correction(int t_corr) {
    uint32_t *BKP = (uint32_t *)BKP_BASE;
    BKP[7] = t_corr;
}
