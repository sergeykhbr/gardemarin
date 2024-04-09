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
#include <fwapi.h>
#include <uart.h>
#include "motor.h"

static void motor_configurate_gpio_bridge(h_bridge_gpio_config *brg) {
    uint32_t t1;
    gpio_pin_type *pin;

    // Set control pins as simple output (no pwm for now)
    for (int i = 0; i < 4; i++) {
        pin = &brg->out[i];

        gpio_pin_as_output(pin,
                           GPIO_NO_OPEN_DRAIN,
                           GPIO_SLOW,
                           GPIO_NO_PUSH_PULL);

        // Set output value to LOW
        gpio_pin_clear(pin);
    }

    // Set 4-pins mode = 0;
    gpio_pin_as_output(&brg->mode,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&brg->mode);

    // Set ADC inputs
    for (int i = 0; i < 2; i++) {
        pin = &brg->adc[i];

        // 00 input; 01 output; 10 alternate; 11 analog
        t1 = read32(&pin->port->MODER);
        t1 |= (0x3 << 2*pin->pinidx);
        write32(&pin->port->MODER, t1);
    }

}

extern "C" void motor_driver_init() {
    motor_driver_type *p = (motor_driver_type *)fw_get_ram_data(MOTOR_DRV_NAME);
    if (p == 0) {
         return;
    }
    // Bridge[0]
    // 40 PE9 TIM1_CH1
    p->gpio_cfg[0].out[0].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[0].out[0].pinidx = 9;
    // 42 PE11 TIM1_CH2
    p->gpio_cfg[0].out[1].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[0].out[1].pinidx = 11;
    // 44 PE13 TIM1_CH3
    p->gpio_cfg[0].out[2].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[0].out[2].pinidx = 13;
    // 45 PE14 TIM1_CH4
    p->gpio_cfg[0].out[3].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[0].out[3].pinidx = 14;
    // 55 PD8 DRV0_MODE
    p->gpio_cfg[0].mode.port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[0].mode.pinidx = 8;
    // 23 PA0 ADC123_IN0
    p->gpio_cfg[0].adc[0].port = (GPIO_registers_type *)GPIOA_BASE;
    p->gpio_cfg[0].adc[0].pinidx = 0;
    // 26 PA3 ADC123_IN3
    p->gpio_cfg[0].adc[1].port = (GPIO_registers_type *)GPIOA_BASE;
    p->gpio_cfg[0].adc[1].pinidx = 3;

    // Bridge[1]
    // 59 PD12 TIM4_CH1
    p->gpio_cfg[1].out[0].port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[1].out[0].pinidx = 12;
    // 93 PB7 TIM4_CH2
    p->gpio_cfg[1].out[1].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[1].out[1].pinidx = 7;
    // 95 PB8 TIM4_CH3
    p->gpio_cfg[1].out[2].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[1].out[2].pinidx = 8;
    // 96 PB9 TIM4_CH4
    p->gpio_cfg[1].out[3].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[1].out[3].pinidx = 9;
    // 56 PD9 DRV1_MODE
    p->gpio_cfg[1].mode.port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[1].mode.pinidx = 9;
    // 29 PA4 ADC12_IN4
    p->gpio_cfg[1].adc[0].port = (GPIO_registers_type *)GPIOA_BASE;
    p->gpio_cfg[1].adc[0].pinidx = 4;
    // 30 PA5 ADC12_IN5
    p->gpio_cfg[1].adc[1].port = (GPIO_registers_type *)GPIOA_BASE;
    p->gpio_cfg[1].adc[1].pinidx = 5;

    // Bridge[2]
    // 63 PC6 TIM8_CH1
    p->gpio_cfg[2].out[0].port = (GPIO_registers_type *)GPIOC_BASE;
    p->gpio_cfg[2].out[0].pinidx = 6;
    // 64 PC7 TIM8_CH2
    p->gpio_cfg[2].out[1].port = (GPIO_registers_type *)GPIOC_BASE;
    p->gpio_cfg[2].out[1].pinidx = 7;
    // 65 PC8 TIM8_CH3
    p->gpio_cfg[2].out[2].port = (GPIO_registers_type *)GPIOC_BASE;
    p->gpio_cfg[2].out[2].pinidx = 8;
    // 66 PC9 TIM8_CH4
    p->gpio_cfg[2].out[3].port = (GPIO_registers_type *)GPIOC_BASE;
    p->gpio_cfg[2].out[3].pinidx = 9;
    // 57 PD10 DRV2_MODE
    p->gpio_cfg[2].mode.port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[2].mode.pinidx = 10;
    // 31 PA6 ADC12_IN6
    p->gpio_cfg[2].adc[0].port = (GPIO_registers_type *)GPIOA_BASE;
    p->gpio_cfg[2].adc[0].pinidx = 6;
    // 35 PB0 ADC12_IN8
    p->gpio_cfg[2].adc[1].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[2].adc[1].pinidx = 0;

    // Bridge[3]
    // 4 PE5 TIM9_CH1
    p->gpio_cfg[3].out[0].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[3].out[0].pinidx = 5;
    // 5 PE6 TIM9_CH2
    p->gpio_cfg[3].out[1].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[3].out[1].pinidx = 6;
    // 53 PB14 TIM12_CH1
    p->gpio_cfg[3].out[2].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[3].out[2].pinidx = 14;
    // 54 PB15 TIM12_CH2
    p->gpio_cfg[3].out[3].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[3].out[3].pinidx = 15;
    // 58 PD11 DRV3_MODE
    p->gpio_cfg[3].mode.port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[3].mode.pinidx = 11;
    // 36 PB1 ADC12_IN9
    p->gpio_cfg[3].adc[0].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[3].adc[0].pinidx = 1;
    // 15 PC0 ADC123_IN10
    p->gpio_cfg[3].adc[1].port = (GPIO_registers_type *)GPIOC_BASE;
    p->gpio_cfg[3].adc[1].pinidx = 0;

    for (int i = 0; i < 4; i++) {
        motor_configurate_gpio_bridge(&p->gpio_cfg[i]);
    }
}

// DC motor
// mode = 0 (4-pins control)
//    in[1]  in[0]     out[1] out[0]
//     0       0         z     z 
//     0       1         0     1    forward rotation
//     1       0         1     0    backward rotation
//     1       1         break break 
extern "C" void motor_dc_start(int idx) {
    motor_driver_type *p = (motor_driver_type *)fw_get_ram_data(MOTOR_DRV_NAME);
    if (p == 0) {
         return;
    }
    h_bridge_gpio_config *brg = &p->gpio_cfg[idx >> 1];
    int chn = idx & 0x1;

    // Set channel OUT[1:0] to 01 value
    gpio_pin_set(&brg->out[2*chn]);
    gpio_pin_clear(&brg->out[2*chn + 1]);
}

extern "C" void motor_dc_stop(int idx) {
    motor_driver_type *p = (motor_driver_type *)fw_get_ram_data(MOTOR_DRV_NAME);
    if (p == 0) {
         return;
    }
    h_bridge_gpio_config *brg = &p->gpio_cfg[idx >> 1];
    int chn = idx & 0x1;

    gpio_pin_clear(&brg->out[2*chn]);
    gpio_pin_clear(&brg->out[2*chn + 1]);
}
