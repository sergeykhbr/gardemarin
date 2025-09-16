/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

typedef struct I2C_registers_type
{
    volatile uint32_t CR1;        // 0x00 control register 1
    volatile uint32_t CR2;        // 0x04 control register 2
    volatile uint32_t OAR1;       // 0x08 Own address register 1
    volatile uint32_t OAR2;       // 0x0C Own address register 2
    volatile uint32_t DR;         // 0x10 Data register
    volatile uint32_t SR1;        // 0x14 Status register 1
    volatile uint32_t SR2;        // 0x18 Status register 2
    volatile uint32_t CCR;        // 0x1C Clock control register
    volatile uint32_t TRISE;      // 0x20 Maximum rise time in Fast/Slow mode
} I2C_registers_type;

