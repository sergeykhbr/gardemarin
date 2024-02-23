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

/** Ssytem configuration controller */
typedef struct SYSCFG_registers_type
{
  volatile uint32_t MEMRMP;       /*!< SYSCFG memory remap register,                      Address offset: 0x00      */
  volatile uint32_t PMC;          /*!< SYSCFG peripheral mode configuration register,     Address offset: 0x04      */
  volatile uint32_t EXTICR[4];    /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
  uint32_t  RESERVED[2];          /*!< Reserved, 0x18-0x1C                                                          */ 
  volatile uint32_t CMPCR;        /*!< SYSCFG Compensation cell control register,         Address offset: 0x20      */
} SYSCFG_registers_type;
