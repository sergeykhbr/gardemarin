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
  GPIO registers 
 */
#ifdef __F103x

// No alternate function selector as in F4xx series.	
typedef struct GPIO_registers_type
{
    volatile uint32_t CR[2];    // 0x00, 0x04 Control Reg: Mode/Function
    volatile uint32_t IDR;      // 0x08 GPIO port input data register
    volatile uint32_t ODR;      // 0x0C GPIO port output data register
    volatile uint16_t BSRRL;    // 0x10 GPIO port bit set
    volatile uint16_t BSRRH;    // 0x12 GPIO port bit reset
    volatile uint32_t BRR;      // 0x14 GPIO port bit reset
    volatile uint32_t LCKR;     // 0x18 GPIO port configuration lock register
} GPIO_registers_type;

#else

typedef struct GPIO_registers_type
{
    volatile uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
    volatile uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
    volatile uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
    volatile uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    volatile uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
    volatile uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
    volatile uint16_t BSRRL;    /*!< GPIO port bit set/reset low register,  Address offset: 0x18      */
    volatile uint16_t BSRRH;    /*!< GPIO port bit set/reset high register, Address offset: 0x1A      */
    volatile uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    volatile uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_registers_type;

#endif
