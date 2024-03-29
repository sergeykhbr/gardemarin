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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct load_sensor_type {
    char hx711_not_found;  // [3:0] mask
    char ready;            // [3:0] mask
    uint32_t value[4];
} load_sensor_type;

void load_sensor_init(load_sensor_type *data);
void load_sensor_read(load_sensor_type *data);
void load_sensor_sleep(load_sensor_type *data);

#ifdef __cplusplus
}
#endif
