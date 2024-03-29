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

#include <gardemarin.h>
#include <prjtypes.h>
#include <stm32f4xx_map.h>
#include <gpio_drv.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define LED_STRIP_DRV_NAME "ledstr"

typedef struct led_strip_type {
    gpio_pin_type gpio_cfg[GARDEMARIN_LED_STRIP_TOTAL];
    uint32_t enable;
} led_strip_type;

void led_strip_init();
void led_strip_on(int idx, int dimrate);
void led_strip_off(int idx);

#ifdef __cplusplus
}
#endif
