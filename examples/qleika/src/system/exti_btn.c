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

#include "exti_btn.h"
#include "../gpio_cfg.h"


static int btn_[3] = {0};


void exti0_btn_up_handler() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    btn_[0] = 1;
    write32(&EXTI->PR, 1 << CFG_PIN_BTN1_UP.pinidx);  // clear pending bit
    nvic_irq_clear(6);        // EXTI0
}

void exti1_btn_center_handler() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    btn_[1] = 1;
    write32(&EXTI->PR, 1 << CFG_PIN_BTN2_CENTER.pinidx);  // clear pending bit
    nvic_irq_clear(7);        // EXTI1
}

void exti13_btn_down_handler() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    btn_[2] = 1;
    write32(&EXTI->PR, 1 << CFG_PIN_BTN3_DOWN.pinidx);  // clear pending bit
    nvic_irq_clear(40);        // EXTI15_10
}

void exti_init() {
    AFIO_registers_type *afio = (AFIO_registers_type *)AFIO_BASE;
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    uint32_t t1;

    gpio_pin_as_input(&CFG_PIN_BTN1_UP, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_as_input(&CFG_PIN_BTN2_CENTER, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_as_input(&CFG_PIN_BTN3_DOWN, GPIO_SLOW, GPIO_NO_PUSH_PULL);


    t1 = read32(&afio->EXTICR[0]);
    t1 &= ~(0xFF << 0);
    t1 |= (0x11 << 0);   // [7:4]EXTI1 [3:0] 0001 EXTI0 <- PB0 (btn up); EXTI1 <- PB1
    write32(&afio->EXTICR[0], t1);

    t1 = read32(&afio->EXTICR[3]);
    t1 &= ~(0xF << 4);
    t1 |= (0x2 << 4);   // [7:4] 0010 EXTI13 <- PC13 (btn down)
    write32(&afio->EXTICR[0], t1);

    write32(&EXTI->EMR, 0);  // No event
    write32(&EXTI->RTSR, 0);  // Disable rising edge
    write32(&EXTI->FTSR, (1 << 0) | (1 << 1) | (1 << 13));  // Enable falling edge
    write32(&EXTI->IMR, (1 << 0) | (1 << 1) | (1 << 13));  // Unmask interrupts
    write32(&EXTI->PR, ~0ul);  // clear pending bit

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(6, 7);  // 6=EXTI0
    nvic_irq_enable(7, 7);   // 7=EXTI1
    nvic_irq_enable(40, 7);  // 40=EXTI[15:10]
}


int is_btn_up_pressed() {
    int ret = btn_[0];
    btn_[0] = 0;
    return ret;
}

int is_btn_center_pressed() {
    int ret = btn_[1];
    btn_[1] = 0;
    return ret;
}

int is_btn_down_pressed() {
    int ret = btn_[2];
    btn_[2] = 0;
    return ret;
}
