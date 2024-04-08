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

typedef struct USART_registers_type
{
    volatile uint16_t SR;         /*!< USART Status register,                   Address offset: 0x00 */
    uint16_t RESERVED0;  /*!< Reserved, 0x02                                                */
    volatile uint16_t DR;         /*!< USART Data register,                     Address offset: 0x04 */
    uint16_t RESERVED1;  /*!< Reserved, 0x06                                                */
    volatile uint16_t BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
    uint16_t RESERVED2;  /*!< Reserved, 0x0A                                                */
    volatile uint16_t CR1;        /*!< USART Control register 1,                Address offset: 0x0C */
    uint16_t RESERVED3;  /*!< Reserved, 0x0E                                                */
    volatile uint16_t CR2;        /*!< USART Control register 2,                Address offset: 0x10 */
    uint16_t RESERVED4;  /*!< Reserved, 0x12                                                */
    volatile uint16_t CR3;        /*!< USART Control register 3,                Address offset: 0x14 */
    uint16_t RESERVED5;  /*!< Reserved, 0x16                                                */
    volatile uint16_t GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
    uint16_t RESERVED6;  /*!< Reserved, 0x1A                                                */
} USART_registers_type;
