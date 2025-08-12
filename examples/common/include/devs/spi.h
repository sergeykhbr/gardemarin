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

typedef struct SPI_registers_type
{
    volatile uint16_t CR1;        // 0x00 SPI control register 1 (not used in I2S mode)
    volatile uint16_t RESERVED0;  // 0x02 Reserved
    volatile uint16_t CR2;        // 0x04 SPI control register 2
    volatile uint16_t RESERVED1;  // 0x06 Reserved
    volatile uint16_t SR;         // 0x08 SPI status register
    volatile uint16_t RESERVED2;  // 0x0A Reserved
    volatile uint16_t DR;         // 0x0C SPI data register
    volatile uint16_t RESERVED3;  // 0x0E Reserved
    volatile uint16_t CRCPR;      // 0x10 SPI CRC polynomial register (not used in I2S mode)
    volatile uint16_t RESERVED4;  // 0x12 Reserved
    volatile uint16_t RXCRCR;     // 0x14 SPI RX CRC register (not used in I2S mode)
    volatile uint16_t RESERVED5;  // 0x16 Reserved
    volatile uint16_t TXCRCR;     // 0x18 SPI TX CRC register (not used in I2S mode)
    volatile uint16_t RESERVED6;  // 0x1A Reserved
    volatile uint16_t I2SCFGR;    // 0x1C SPI_I2S configuration register
    volatile uint16_t RESERVED7;  // 0x1E Reserved
    volatile uint16_t I2SPR;      // 0x20 SPI_I2S prescaler register
    volatile uint16_t RESERVED8;  // 0x22 Reserved
} SPI_registers_type;

