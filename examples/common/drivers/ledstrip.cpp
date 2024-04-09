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

// LED[0] PB[10] TIM2_CH3
// LED[1] PE[0]
// LED[2] PE[1]
// LED[3] PE[15]
extern "C" void led_strip_init() {
    led_strip_type *p = (led_strip_type *)fw_get_ram_data(LED_STRIP_DRV_NAME);
    if (p == 0) {
         return;
    }

    p->gpio_cfg[0].port = (GPIO_registers_type *)GPIOB_BASE;
    p->gpio_cfg[0].pinidx = 10;

    p->gpio_cfg[1].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[1].pinidx = 0;

    p->gpio_cfg[2].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[2].pinidx = 1;

    p->gpio_cfg[3].port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg[3].pinidx = 15;

    for (int i = 0; i < GARDEMARIN_LED_STRIP_TOTAL; i++) {
        gpio_pin_as_output(&p->gpio_cfg[i],
                           GPIO_NO_OPEN_DRAIN,
                           GPIO_SLOW,
                           GPIO_NO_PUSH_PULL);

        led_strip_off(i);
    }
}

extern "C" void led_strip_on(int idx, int dimrate) {
    led_strip_type *p = (led_strip_type *)fw_get_ram_data(LED_STRIP_DRV_NAME);
    if (p == 0) {
         return;
    }
    if (idx < 0 || idx >= GARDEMARIN_LED_STRIP_TOTAL) {
        for (int i = 0; i < GARDEMARIN_LED_STRIP_TOTAL; i++) {
            gpio_pin_set(&p->gpio_cfg[i]);
        }
    } else {
        gpio_pin_set(&p->gpio_cfg[idx]);
    }
}

extern "C" void led_strip_off(int idx) {
    led_strip_type *p = (led_strip_type *)fw_get_ram_data(LED_STRIP_DRV_NAME);
    if (p == 0) {
         return;
    }
    if (idx < 0 || idx >= GARDEMARIN_LED_STRIP_TOTAL) {
        for (int i = 0; i < GARDEMARIN_LED_STRIP_TOTAL; i++) {
            gpio_pin_clear(&p->gpio_cfg[i]);
        }
    } else {
        gpio_pin_clear(&p->gpio_cfg[idx]);
    }
}
