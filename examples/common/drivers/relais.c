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
#include "relais.h"

//    PD13 - relais 0
//    PD14 - relais 1
void relais_init() {
    relais_type *p = (relais_type *)fw_get_ram_data(RELAIS_DRV_NAME);
    if (p == 0) {
         return;
    }
    p->gpio_cfg[0].port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[0].pinidx = 13;

    p->gpio_cfg[1].port = (GPIO_registers_type *)GPIOD_BASE;
    p->gpio_cfg[1].pinidx = 14;


    for (int i = 0; i < GARDEMARIN_RELAIS_TOTAL; i++) {
        gpio_pin_as_output(&p->gpio_cfg[i],
                           GPIO_NO_OPEN_DRAIN,
                           GPIO_SLOW,
                           GPIO_NO_PUSH_PULL);
        relais_off(i);
    }
}

void relais_on(int idx) {
    relais_type *p = (relais_type *)fw_get_ram_data(RELAIS_DRV_NAME);
    if (p == 0) {
         return;
    }
    gpio_pin_set(&p->gpio_cfg[idx]);
}

void relais_off(int idx) {
    relais_type *p = (relais_type *)fw_get_ram_data(RELAIS_DRV_NAME);
    if (p == 0) {
         return;
    }
    gpio_pin_clear(&p->gpio_cfg[idx]);
}
