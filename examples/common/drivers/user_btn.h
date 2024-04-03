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

#define USER_BTN_DRV_NAME "ubtn"

#define BTN_EVENT_PRESSED  0x01
#define BTN_EVENT_RELEASED 0x02

typedef struct user_btn_type {
    uint32_t event;
    uint32_t tm_count;     // copied from task 500 ms (for now)
    uint32_t tm_pressed;
} user_btn_type;

void user_btn_init();

#ifdef __cplusplus
}
#endif
