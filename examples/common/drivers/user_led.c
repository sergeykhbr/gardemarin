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
#include "user_led.h"

//    PE2 - User LED. 0=LED is on (inversed)
void user_led_init() {
    user_led_type *p = (user_led_type *)fw_get_ram_data(USER_LED_DRV_NAME);
    if (p == 0) {
         return;
    }
    p->gpio_cfg.port = (GPIO_registers_type *)GPIOE_BASE;
    p->gpio_cfg.pinidx = 2;

    gpio_pin_as_output(&p->gpio_cfg,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);

    user_led_set_state(1);
}

void user_led_set_state(int on) {
    user_led_type *p = (user_led_type *)fw_get_ram_data(USER_LED_DRV_NAME);
    if (p == 0) {
         return;
    }
    // inversed
    if (on) {
        gpio_pin_clear(&p->gpio_cfg);
    } else {
        gpio_pin_set(&p->gpio_cfg);
    }
}
