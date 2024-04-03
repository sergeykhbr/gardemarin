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
#include <memanager.h>
#include <uart.h>
#include "user_btn.h"

//    PC13 - User btn (internal pull-up). 0=btn is pressed
void user_btn_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SYSCFG_registers_type *SYSCFG = (SYSCFG_registers_type *)SYSCFG_BASE;
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOC_BASE;
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    int bitidx = 13;
    uint32_t t1;

    // enable clocks for bullet proof
    t1 = read32(&RCC->AHB1ENR);
    t1 |= (1 << 2);           // PC
    write32(&RCC->AHB1ENR, t1);

    t1 = read32(&RCC->AHB1ENR);
    t1 |= 1 << 14;            // APB2[14] SYSCFGEN includes EXTI control registers
    write32(&RCC->APB2ENR, t1);


    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&P->MODER);
    t1 &= ~(0x3 << 2*bitidx);
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
    t1 |= (0x1 << 2*bitidx);
    write32(&P->PUPDR, t1);

    // connect EXTI13 request to port PC
    t1 = read32(&SYSCFG->EXTICR[3]);
    t1 &= ~0xF0;
    t1 |= 0x20;    // [7:4] EXIT13[3:0] 0000=PA; 0001=PB; 0010=PC;..
    write32(&SYSCFG->EXTICR[3], t1);


    write32(&EXTI->IMR, 1 << 13);     // Interrupt mask: 0 line is masked; 1=unmasked
    write32(&EXTI->RTSR, 1 << 13);    // Rising trigger selection
    write32(&EXTI->FTSR, 1 << 13);    // Falling trigger selection

    write32(&EXTI->PR, 1 << 13);   // Pending register, cleared by programming it to 1

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(40, 3);
}

// EXTI15_10
void Btn_IRQHandler() {
    GPIO_registers_type *PC = (GPIO_registers_type *)GPIOC_BASE;
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    user_btn_type *p = (user_btn_type *)fw_get_ram_data(USER_BTN_DRV_NAME);
    uint32_t state;

    write32(&EXTI->PR, 1 << 13);   // Pending register, cleared by programming it to 1
    nvic_irq_clear(40);

    if (p == 0) {
        // Something is totally wrong
        return;
    }

    state = (read32(&PC->IDR) >> 13) & 1;

    // inversed, if LOW btn is pressed
    if (state == 0) {
        if ((p->tm_count - p->tm_pressed) >= 2) {   // 1=500ms
            p->event |= BTN_EVENT_PRESSED;
        }
        p->tm_pressed = p->tm_count;
    } else {
        p->event |= BTN_EVENT_RELEASED;
    }
}
