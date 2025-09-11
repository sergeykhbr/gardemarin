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
  Reset and Clock Control registers 
 */

#ifdef __F103x

typedef struct RCC_registers_type
{
    volatile uint32_t CR;            // 0x00 RCC clock control register
    volatile uint32_t CFGR;          // 0x04 RCC clock configuration register
    volatile uint32_t CIR;           // 0x08 RCC clock interrupt register
    volatile uint32_t APB2RSTR;      // 0x0C RCC APB2 peripheral reset register
    volatile uint32_t APB1RSTR;      // 0x10 RCC APB1 peripheral reset register
    volatile uint32_t AHBENR;        // 0x14 RCC AHB peripheral clock register
    volatile uint32_t APB2ENR;       // 0x18 RCC APB2 peripheral clock enable register
    volatile uint32_t APB1ENR;       // 0x1C RCC APB1 peripheral clock enable register
    volatile uint32_t BDCR;          // 0x20 RCC Backup domain control register
    volatile uint32_t CSR;           // 0x24 RCC clock control & status register
} RCC_registers_type;

#else

typedef struct RCC_registers_type
{
    volatile uint32_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
    volatile uint32_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
    volatile uint32_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
    volatile uint32_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
    volatile uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
    volatile uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
    volatile uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
    uint32_t RESERVED0;     /*!< Reserved, 0x1C                                                                    */
    volatile uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
    volatile uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
    uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
    volatile uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
    volatile uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
    volatile uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
    uint32_t RESERVED2;     /*!< Reserved, 0x3C                                                                    */
    volatile uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
    volatile uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
    uint32_t RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
    volatile uint32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
    volatile uint32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
    volatile uint32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
    uint32_t RESERVED4;     /*!< Reserved, 0x5C                                                                    */
    volatile uint32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
    volatile uint32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
    uint32_t RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
    volatile uint32_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
    volatile uint32_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
    uint32_t RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
    volatile uint32_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
    volatile uint32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
} RCC_registers_type;

#endif