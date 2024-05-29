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
  Real Time Clock registers
 */
typedef struct RTC_registers_type {
    volatile uint32_t TR;      /*!< RTC time register,                                        Address offset: 0x00 */
    volatile uint32_t DR;      /*!< RTC date register,                                        Address offset: 0x04 */
    volatile uint32_t CR;      /*!< RTC control register,                                     Address offset: 0x08 */
    volatile uint32_t ISR;     /*!< RTC initialization and status register,                   Address offset: 0x0C */
    volatile uint32_t PRER;    /*!< RTC prescaler register,                                   Address offset: 0x10 */
    volatile uint32_t WUTR;    /*!< RTC wakeup timer register,                                Address offset: 0x14 */
    volatile uint32_t CALIBR;  /*!< RTC calibration register,                                 Address offset: 0x18 */
    volatile uint32_t ALRMAR;  /*!< RTC alarm A register,                                     Address offset: 0x1C */
    volatile uint32_t ALRMBR;  /*!< RTC alarm B register,                                     Address offset: 0x20 */
    volatile uint8_t WPR;     /*!< RTC write protection register,                            Address offset: 0x24 */
    uint8_t rsrv1[3];
    volatile uint32_t SSR;     /*!< RTC sub second register,                                  Address offset: 0x28 */
    volatile uint32_t SHIFTR;  /*!< RTC shift control register,                               Address offset: 0x2C */
    volatile uint32_t TSTR;    /*!< RTC time stamp time register,                             Address offset: 0x30 */
    volatile uint32_t TSDR;    /*!< RTC time stamp date register,                             Address offset: 0x34 */
    volatile uint32_t TSSSR;   /*!< RTC time-stamp sub second register,                       Address offset: 0x38 */
    volatile uint32_t CALR;    /*!< RTC calibration register,                                 Address offset: 0x3C */
    volatile uint32_t TAFCR;   /*!< RTC tamper and alternate function configuration register, Address offset: 0x40 */
    volatile uint32_t ALRMASSR;/*!< RTC alarm A sub second register,                          Address offset: 0x44 */
    volatile uint32_t ALRMBSSR;/*!< RTC alarm B sub second register,                          Address offset: 0x48 */
    uint32_t RESERVED7;    /*!< Reserved, 0x4C                                                                 */
    volatile uint32_t BKP0R;   /*!< RTC backup register 1,                                    Address offset: 0x50 */
    volatile uint32_t BKP1R;   /*!< RTC backup register 1,                                    Address offset: 0x54 */
    volatile uint32_t BKP2R;   /*!< RTC backup register 2,                                    Address offset: 0x58 */
    volatile uint32_t BKP3R;   /*!< RTC backup register 3,                                    Address offset: 0x5C */
    volatile uint32_t BKP4R;   /*!< RTC backup register 4,                                    Address offset: 0x60 */
    volatile uint32_t BKP5R;   /*!< RTC backup register 5,                                    Address offset: 0x64 */
    volatile uint32_t BKP6R;   /*!< RTC backup register 6,                                    Address offset: 0x68 */
    volatile uint32_t BKP7R;   /*!< RTC backup register 7,                                    Address offset: 0x6C */
    volatile uint32_t BKP8R;   /*!< RTC backup register 8,                                    Address offset: 0x70 */
    volatile uint32_t BKP9R;   /*!< RTC backup register 9,                                    Address offset: 0x74 */
    volatile uint32_t BKP10R;  /*!< RTC backup register 10,                                   Address offset: 0x78 */
    volatile uint32_t BKP11R;  /*!< RTC backup register 11,                                   Address offset: 0x7C */
    volatile uint32_t BKP12R;  /*!< RTC backup register 12,                                   Address offset: 0x80 */
    volatile uint32_t BKP13R;  /*!< RTC backup register 13,                                   Address offset: 0x84 */
    volatile uint32_t BKP14R;  /*!< RTC backup register 14,                                   Address offset: 0x88 */
    volatile uint32_t BKP15R;  /*!< RTC backup register 15,                                   Address offset: 0x8C */
    volatile uint32_t BKP16R;  /*!< RTC backup register 16,                                   Address offset: 0x90 */
    volatile uint32_t BKP17R;  /*!< RTC backup register 17,                                   Address offset: 0x94 */
    volatile uint32_t BKP18R;  /*!< RTC backup register 18,                                   Address offset: 0x98 */
    volatile uint32_t BKP19R;  /*!< RTC backup register 19,                                   Address offset: 0x9C */
} RTC_registers_type;
