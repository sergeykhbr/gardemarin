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
#pragma once

#include <prjtypes.h>
#include <mcu.h>
#include <gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpio_pin_type {
    GPIO_registers_type *port;
    int pinidx;
} gpio_pin_type;

#define GPIO_NO_OPEN_DRAIN  0
#define GPIO_OPEN_DRAIN     1

#define GPIO_SLOW           0
#define GPIO_MEDIUM         1
#define GPIO_FAST           2
#define GPIO_VERY_FAST      3

#define GPIO_NO_PUSH_PULL   0
#define GPIO_PULL_UP        1
#define GPIO_PULL_DOWN      2

void gpio_pin_as_input(const gpio_pin_type *p,
                       uint32_t speed,
                       uint32_t pushpull);

void gpio_pin_as_output(const gpio_pin_type *p,
                        uint32_t odrain,
                        uint32_t speed,
                        uint32_t pushpull);

void gpio_pin_as_alternate(const gpio_pin_type *p,
                           uint32_t ADx);

void gpio_pin_as_analog(const gpio_pin_type *p);

void gpio_pin_set(const gpio_pin_type *p);
void gpio_pin_clear(const gpio_pin_type *p);
uint32_t gpio_pin_get(const gpio_pin_type *p);

#ifdef __cplusplus
}
#endif
