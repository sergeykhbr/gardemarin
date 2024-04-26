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

typedef struct tim_cr1_bits_type {
    volatile uint32_t CEN : 1;    // [0] rw Counter enable
    volatile uint32_t UDIS : 1;   // [1] rw Update disable
    volatile uint32_t URS : 1;    // [2] rw Update request source
    volatile uint32_t OPM : 1;    // [3] rw One-pulse mode
    volatile uint32_t DIR : 1;    // [4] rw 0=upcounter; 1=downcounter
    volatile uint32_t CMS : 2;    // [6:5] rw Center-aligned mode selection
    volatile uint32_t ARPE : 1;   // [7] rw Auto-reload preload enable: 1=TIMx_ARR is buffered
    volatile uint32_t CKD : 2;    // [9:8] rw Clock Division: 00=Tdts = Tck_int
    uint32_t rsrv31_10 : 22;      // [31:10]
} tim_cr1_bits_type;

typedef union tim_cr1_reg_type {
    tim_cr1_bits_type bits;
    volatile uint32_t val;
} tim_cr1_reg_type;

typedef struct TIM_registers_type {
    tim_cr1_reg_type CR1;         /*!< TIM control register 1,              Address offset: 0x00 */
    volatile uint16_t CR2;         /*!< TIM control register 2,              Address offset: 0x04 */
    uint16_t      RESERVED1;   /*!< Reserved, 0x06                                            */
    volatile uint16_t SMCR;        /*!< TIM slave mode control register,     Address offset: 0x08 */
    uint16_t      RESERVED2;   /*!< Reserved, 0x0A                                            */
    volatile uint16_t DIER;        /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
    uint16_t      RESERVED3;   /*!< Reserved, 0x0E                                            */
    volatile uint16_t SR;          /*!< TIM status register,                 Address offset: 0x10 */
    uint16_t      RESERVED4;   /*!< Reserved, 0x12                                            */
    volatile uint16_t EGR;         /*!< TIM event generation register,       Address offset: 0x14 */
    uint16_t      RESERVED5;   /*!< Reserved, 0x16                                            */
    volatile uint16_t CCMR1;       /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
    uint16_t      RESERVED6;   /*!< Reserved, 0x1A                                            */
    volatile uint16_t CCMR2;       /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
    uint16_t      RESERVED7;   /*!< Reserved, 0x1E                                            */
    volatile uint16_t CCER;        /*!< TIM capture/compare enable register, Address offset: 0x20 */
    uint16_t      RESERVED8;   /*!< Reserved, 0x22                                            */
    volatile uint32_t CNT;         /*!< TIM counter register,                Address offset: 0x24 */
    volatile uint16_t PSC;         /*!< TIM prescaler,                       Address offset: 0x28 */
    uint16_t      RESERVED9;   /*!< Reserved, 0x2A                                            */
    volatile uint32_t ARR;         /*!< TIM auto-reload register,            Address offset: 0x2C */
    volatile uint16_t RCR;         /*!< TIM repetition counter register,     Address offset: 0x30 */
    uint16_t      RESERVED10;  /*!< Reserved, 0x32                                            */
    volatile uint32_t CCR1;        /*!< TIM capture/compare register 1,      Address offset: 0x34 */
    volatile uint32_t CCR2;        /*!< TIM capture/compare register 2,      Address offset: 0x38 */
    volatile uint32_t CCR3;        /*!< TIM capture/compare register 3,      Address offset: 0x3C */
    volatile uint32_t CCR4;        /*!< TIM capture/compare register 4,      Address offset: 0x40 */
    volatile uint16_t BDTR;        /*!< TIM break and dead-time register,    Address offset: 0x44 */
    uint16_t      RESERVED11;  /*!< Reserved, 0x46                                            */
    volatile uint16_t DCR;         /*!< TIM DMA control register,            Address offset: 0x48 */
    uint16_t      RESERVED12;  /*!< Reserved, 0x4A                                            */
    volatile uint16_t DMAR;        /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
    uint16_t      RESERVED13;  /*!< Reserved, 0x4E                                            */
    volatile uint16_t OR;          /*!< TIM option register,                 Address offset: 0x50 */
    uint16_t      RESERVED14;  /*!< Reserved, 0x52                                            */
} TIM_registers_type;
