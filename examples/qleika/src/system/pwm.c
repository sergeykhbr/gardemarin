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
#include "pwm.h"
#include "bkp.h"
#include "../gpio_cfg.h"

uint32_t pwm_get_period() {
    uint32_t hz = 5000;
    return 1000000/hz;
}

uint32_t pwm_get_duty_cycle() {
    return bkp_get_pwm_duty() * pwm_get_period() / 100;
}

void pwm_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM = (TIM_registers_type *)TIM1_BASE;

    // TIM1 on APB2 = 72 MHz
    uint32_t t1 = read32(&RCC->APB2ENR);
    t1 |= (1 << 11);             // APB2[11] TIM1EN
    write32(&RCC->APB2ENR, t1);

    // TIM1_CH1N -> PB[13]: TIM1_REMAP[1:0]=00 (no remap)
    gpio_pin_as_alternate(&CFG_PIN_MOSFET_1, 6);

    // Output compare mode:
    // OCxM = 011 to toggle OCx output pin when CNT matches CCRx
    // OCxPE = 0 to disable preload register
    // CCxP = 0 to select active high polarity
    // CCxE = 1 to enable output

    // [7] OC1CE Output compare 1 clear enable
    // [6:4] OC1M output compare 1 mode: 011=Toggle when CNT=CCR1; 110=PWM mode 1; 111=PWM mode 2
    // [3] OC1PE Output compare 1 preload enable
    // [2] OC1FE: Output compare 1 fast enable
    // [1:0] CC1S: 00 = CC1 configured as output
    write16(&TIM->CCMR1, (0x6 << 4) | (1 << 3));
    // [3] CC1NP: 0 = active high; 1=active low
    // [2] CC1NE: 1: pin 
    write16(&TIM->CCER, (3 << 1) | (2 << 0));

    // 0    < cnt < ccr1 => HIGH
    // ccr1 < cnt < arr  => LOW
    write32(&TIM->ARR, pwm_get_period());
    write32(&TIM->CCR1, pwm_get_duty_cycle());

    t1 = read16(&TIM->BDTR);
    t1 |= (1 << 15);  // [15] MOE: Main output enable
    write16(&TIM->BDTR, (uint16_t)t1);

    write32(&TIM->CR1.val, 0);         // stop counter
    write16(&TIM->PSC, 71);            // to form 1 MHz count
    write16(&TIM->DIER, 0);            // irq disabled

}

void pwm_enable() {
    TIM_registers_type *TIM = (TIM_registers_type *)TIM1_BASE;
    tim_cr1_reg_type cr1;

    if (pwm_get_duty_cycle() == 0) {
        return;
    }

    write32(&TIM->CNT, 0);

    cr1.val = 0;
    cr1.bits.ARPE = 1;   // auto-reload preload enable
    cr1.bits.CEN = 1;
    write32(&TIM->CR1.val, cr1.val);
}

void pwm_disable() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM1_BASE;
    write32(&TIM->CR1.val, 0);
}

void pwm_set_duty_cyle(uint32_t duty) {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM1_BASE;
    bkp_set_pwm_duty(duty);
    pwm_disable();

    if (duty) {
        write32(&TIM->CCR1, duty);
        pwm_enable();
    }
}
