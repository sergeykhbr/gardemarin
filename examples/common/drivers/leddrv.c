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

#include <prjtypes.h>
#include <stdio.h>
#include <stm32f4xx_map.h>
#include <uart.h>
#include "leddrv.h"

// LED[0] PB[10] TIM2_CH3
// LED[1] PE[0]
// LED[2] PE[1]
// LED[3] PE[15]
void led_init(led_channel_type *data) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOB_BASE;
    int bitidx = 10;
    uint32_t t1;

    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&P->MODER);
    t1 &= ~(0x3 << 2*bitidx);
    t1 |= (0x1 << 2*bitidx);
    write32(&P->MODER, t1);

    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    t1 = read32(&P->OTYPER);
    t1 &= ~(1 << bitidx);
    write32(&P->OTYPER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&P->OSPEEDR);
    t1 &= ~(0x3 << 2*bitidx);
    write32(&P->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&P->PUPDR);
    t1 &= ~(0x3 << 2*bitidx);
    write32(&P->PUPDR, t1);

    led_off(data);
}

void led_on(led_channel_type *data) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOB_BASE;
    int bitidx = 10;
    uint32_t t1 = read32(&P->ODR);
    t1 |= (1 << bitidx);
    write32(&P->ODR, t1);
}

void led_off(led_channel_type *data) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOB_BASE;
    int bitidx = 10;
    uint32_t t1 = read32(&P->ODR);
    t1 &= ~(1 << bitidx);
    write32(&P->ODR, t1);
}
