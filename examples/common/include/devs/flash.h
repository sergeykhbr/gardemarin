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
  Flash registers
 */
typedef struct FLASH_registers_type
{
    volatile uint32_t ACR;      /*!< FLASH access control register, Address offset: 0x00 */
    volatile uint32_t KEYR;     /*!< FLASH key register,            Address offset: 0x04 */
    volatile uint32_t OPTKEYR;  /*!< FLASH option key register,     Address offset: 0x08 */
    volatile uint32_t SR;       /*!< FLASH status register,         Address offset: 0x0C */
    volatile uint32_t CR;       /*!< FLASH control register,        Address offset: 0x10 */
    volatile uint32_t OPTCR;    /*!< FLASH option control register, Address offset: 0x14 */
} FLASH_registers_type;
