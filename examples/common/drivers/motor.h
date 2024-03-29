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

#define MOTOR_DRV_NAME "mtr"

// DRV8847 H-brdige motor driver IC
//   1 step motor or 2 DC motors
typedef struct h_bridge_gpio_config {
    gpio_pin_type mode;          // 4-pins or 2-pins control selector: 0=4-pins (default)
    gpio_pin_type out[4];        // port bits connected to H-Bridge IN[4]
    gpio_pin_type adc[2];        // ADC inputs, current measurments
} h_bridge_gpio_config;

typedef struct motor_driver_type {
    h_bridge_gpio_config gpio_cfg[GARDEMARIN_HBRDIGE_MOTOR_TOTAL];
} motor_driver_type;


void motor_driver_init();
void motor_dc_start(int idx);
void motor_dc_stop(int idx);

#ifdef __cplusplus
}
#endif
