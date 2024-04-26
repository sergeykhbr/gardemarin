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
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include "ledstrip.h"
#include <uart.h>

// LED[0] PB[10] TIM2_CH3
// LED[1] PE[0]
// LED[2] PE[1]
// LED[3] PE[15]
static const gpio_pin_type GPIO_CFG[GARDEMARIN_LED_STRIP_TOTAL] = {
    {(GPIO_registers_type *)GPIOB_BASE, 10},
    {(GPIO_registers_type *)GPIOE_BASE, 0},
    {(GPIO_registers_type *)GPIOE_BASE, 1},
    {(GPIO_registers_type *)GPIOE_BASE, 15}
};

// Access from interrupt
int8_t duty_[GARDEMARIN_LED_STRIP_TOTAL] = {0};
int8_t tim_cnt_ = 0;

extern "C" void TIM2_irq_handler() {
    TIM_registers_type *TIM2 = (TIM_registers_type *)TIM2_BASE;

    if (++tim_cnt_ >= 100) {
        tim_cnt_ = 0;
    }

    for (int i = 0; i < GARDEMARIN_LED_STRIP_TOTAL; i++) {
        if (duty_[i] > tim_cnt_) {
            gpio_pin_set(&GPIO_CFG[i]);
        } else {
            gpio_pin_clear(&GPIO_CFG[i]);
        }
    }

    write16(&TIM2->SR, 0);  // clear all pending bits
    nvic_irq_clear(28);
}

// TIM2 is used as PWM period generator
LedStripDriver::LedStripDriver(const char *name) : FwObject(name),
    tim_hz_("tim_hz"),
    duty0_("duty0", 0),
    duty1_("duty1", 1),
    duty2_("duty2", 2),
    duty3_("duty3", 3) {

    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM2 = (TIM_registers_type *)TIM2_BASE;
    tim_cr1_reg_type cr1;

    tim_cnt_ = 0;

    // adc clock on APB1 = 144/4 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 0);             // APB1[0] TIM2EN
    write32(&RCC->APB1ENR, t1);

    cr1.val = 0;
    write32(&TIM2->CR1.val, cr1.val);   // stop counter
    write16(&TIM2->PSC, 35);            // to form 1 MHz count
    tim_hz_.make_int32(200);
    write16(&TIM2->DIER, 1);            // [0] UIE - update interrupt enabled

    cr1.val = 0;
    cr1.bits.CEN = 1;
    write32(&TIM2->CR1.val, cr1.val);

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(28, 3);
}

void LedStripDriver::Init() {
    RegisterAttribute(&tim_hz_);
    RegisterAttribute(&duty0_);
    RegisterAttribute(&duty1_);
    RegisterAttribute(&duty2_);
    RegisterAttribute(&duty3_);
}

void LedStripDriver::DutyAttribute::pre_read() {
    u_.i8 = duty_[idx_];
}

void LedStripDriver::DutyAttribute::post_write() {
    if (u_.i8 > 100) {
        u_.i8 = 100;
    } else if (u_.i8 < 0) {
        u_.i8 = 0;
    }
    duty_[idx_] = u_.i8;
}

void LedStripDriver::HzAttribute::post_write() {
    TIM_registers_type *TIM2 = (TIM_registers_type *)TIM2_BASE;
    if (u_.i32 < 10) {
        u_.i32 = 10;
    } else if (u_.i32 > 10000) {
        u_.i32 = 10000;
    }
    write32(&TIM2->ARR, 1000000/(100 * u_.i32) - 1);
}
