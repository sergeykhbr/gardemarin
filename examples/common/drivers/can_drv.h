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

#define CAN_RX_FRAMES_MAX 4

typedef union can_payload_type {
    uint32_t u32[2];
    uint8_t u8[8];
} can_payload_type;

typedef struct can_frame_type {
    uint32_t timestamp;
    uint32_t id;
    uint8_t dlc;
    uint8_t busid;
    can_payload_type data;
} can_frame_type;

typedef struct can_bus_type {
    gpio_pin_type gpio_cfg_rx;
    gpio_pin_type gpio_cfg_tx;
    CAN_registers_type *dev;
} can_bus_type;

typedef struct can_type {
    can_bus_type bus[GARDEMARIN_CAN_BUS_TOTAL];

    int rx_frame_cnt;
    can_frame_type rxframes[CAN_RX_FRAMES_MAX];
} can_type;

void can_init();
void can_bus_listener_start(can_type *p, int busid);
void can_bus_listener_stop(can_type *p, int busid);

#ifdef __cplusplus
}
#endif
