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
#include <gpio_drv.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define USER_LED_DRV_NAME "uled"

typedef struct user_led_type {
    gpio_pin_type gpio_cfg;
    uint32_t state;
} user_led_type;

void user_led_init();
void user_led_set_state(int on);

#ifdef __cplusplus
}
#endif
