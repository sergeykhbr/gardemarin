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

/**
  Alternate function GPIO registers 
 */
#ifdef __F103x

typedef struct AFIO_registers_type
{
    volatile uint32_t EVCR;         // 0x00
    volatile uint32_t MAPR;         // 0x04
    volatile uint32_t EXTICR[4];    // 0x08, 0x0C, 0x10, 0x14
    volatile uint32_t MAPR2;        // 0x18
} AFIO_registers_type;

#else

#endif
