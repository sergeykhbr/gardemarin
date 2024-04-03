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
#include <can.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_DRV_NAME "can"

typedef struct can_controller_type {
    gpio_pin_type gpio_cfg_rx;
    gpio_pin_type gpio_cfg_tx;
    CAN_registers_type *dev;
} can_controller_type;

typedef struct can_type {
    can_controller_type ctrl[GARDEMARIN_CAN_TOTAL];
} can_type;

void can_init();

#ifdef __cplusplus
}
#endif
