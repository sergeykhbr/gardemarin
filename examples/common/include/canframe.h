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

#include <inttypes.h>

#define OBJ_MSG_ID(objidx) (0x100 | (objidx << 8))

/*
 * @brief CAN ID used in FW to form DBC output and CAN filters
 * @{
 */
/** @brief CAN ID for debug output */
static const uint32_t CAN_MSG_ID_DBG_OUTPT = 0x776;
/**
 * @brief Temporary CAN ID for the read messages. This value used to setup
 *        Rx CAN filter and properly handle Read/Write request
 */
static const uint32_t CAN_MSG_ID_READ_DATA = 0x777;
/**
 * @brief Temporary CAN ID for the write messages. This value used to setup
 *        Rx CAN filter and properly handle Read/Write request
 */
static const uint32_t CAN_MSG_ID_WRITE_DATA = 0x778;
/**
 * @}
 */


typedef union can_payload_type {
    uint32_t u32[2];
    uint8_t u8[8];
    char s8[8];
} can_payload_type;

typedef struct can_frame_type {
    uint32_t timestamp;
    uint32_t id;
    uint8_t dlc;
    uint8_t busid;
    can_payload_type data;
} can_frame_type;
