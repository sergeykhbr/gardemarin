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

void pwm_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM = (TIM_registers_type *)TIM1_BASE;
    uint32_t hz = 200;
    uint32_t period = 1000000/hz;

    // TIM1 on APB2 = 72 MHz
    uint32_t t1 = read32(&RCC->APB2ENR);
    t1 |= (1 << 11);             // APB2[11] TIM1EN
    write32(&RCC->APB2ENR, t1);

    // TIM1_CH1N -> PB[13]: TIM1_REMAP[1:0]=00 (no remap)
    gpio_pin_as_alternate(&CFG_PIN_MOSFET_1);

    // OCxM = 011 to toggle OCx output pin when CNT matches CCRx
    // OCxPE = 0 to disable preload register
    // CCxP = 0 to select active high polarity
    // CCxE = 1 to enable output

    // [6:4] OC1M output compare 1 mode: 011=Toggle when CNT=CCR1; 110=PWM mode 1; 111=PWM mode 2
    // [1:0] CC1S: 00 = CC1 configured as output
    write32(&TIM->CCMR1, (0x3 << 4));
    // [3] CC1NP: 0 = active high; 1=active low
    // [2] CC1NE: 1: pin 
    write32(&TIM->CCER, (1 << 3) | (1 << 2));

    write32(&TIM->ARR, period - 1);
    write32(&TIM->CCR1, period/2);

    write32(&TIM->CR1.val, 0);         // stop counter
    write16(&TIM->PSC, 71);            // to form 1 MHz count
    write16(&TIM->DIER, 0);            // irq disabled

}

void pwm_enable() {
    TIM_registers_type *TIM = (TIM_registers_type *)TIM1_BASE;
    tim_cr1_reg_type cr1;
    write32(&TIM->CNT, 0);

    cr1.val = 0;
    cr1.bits.CEN = 1;
    write32(&TIM->CR1.val, cr1.val);
}

void pwm_disable() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM1_BASE;
    write32(&TIM->CR1.val, 0);
}

