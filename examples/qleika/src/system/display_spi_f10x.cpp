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

#include "display_spi_f10x.h"
#include "../gpio_cfg.h"

void DisplaySPI_F10x::init_GPIO() {
    AFIO_registers_type *afio = (AFIO_registers_type *)AFIO_BASE;
    // Display SPI1
    //     [PB5] SPI1 MOSI            (alternate)
    //     [PB4] CMD_DATA (SPI1 MISO) (Output)
    //     [PB3] SPI1_SCK             (alternate)
    //     [PA15] RST (SPI_NSS)       (Output)
    DISPLAY_RES = CFG_PIN_DISPLAY_RES;
    DISPLAY_SCK = CFG_PIN_DISPLAY_SCK;
    DISPLAY_SDA = CFG_PIN_DISPLAY_SDA;
    DISPLAY_DC = CFG_PIN_DISPLAY_DC;

    gpio_pin_as_output(&DISPLAY_RES,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&DISPLAY_RES);   // reset: active LOW

    gpio_pin_as_alternate(&DISPLAY_SCK, 6);

    gpio_pin_as_output(&DISPLAY_DC,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&DISPLAY_DC);

    gpio_pin_as_alternate(&DISPLAY_SDA, 6);

    // [PA7]SPI1_REMAP=0; [PB5] SPI1_REMAP=1
    uint32_t t1 = read32(&afio->MAPR);
    t1 |= (1 << 0);            // [0] SPI1rEMAP
    write32(&afio->MAPR, t1);
}

void DisplaySPI_F10x::init_SPI() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    // SPI clock on APB2 = 72 MHz
    uint32_t t1 = read32(&RCC->APB2ENR);
    t1 |= (1 << 12);            // APB2[12] SPI1EN
    write32(&RCC->APB2ENR, t1);

    // SPI master, transmit-only
    uint16_t t2;
    t2 = (0 << 7)    // [7] TXEIE: Tx buffer empty IRQ
       | (0 << 6)    // [6] RXNEIE: Rx buffer not empty IRQ
       | (0 << 5)    // [5] ERRIE: Error IRQ
       | (0 << 4)    // [4] FRF: Frame format: 0=SPI motorola; 1=SPI TI
       | (0 << 2)    // [2] SSOE: SS output enable: 0=disabled (can work multimaster mode)
       | (0 << 1)    // [1] TXDMAEN: Tx buffer DMA ena
       | (0 << 0);   // [0] RXDMAEN: Rx buffer DMA ena
    write16(&spi_bar()->CR2, t2);

    t2 = (0 << 15)    // [15] BIDIMODE: 0=2-line unidirectional, 1=1-linne bidirectional
       | (1 << 14)    // [14] BIDIOE: output enable in bidir mode
       | (0 << 13)    // [13] CRCEN: hardware CRC calc ena
       | (0 << 12)    // [12] CRCNEXT: CRC transfer next
       | (0 << 11)    // [11] DFF: Data frame format: 0=8bits, 1=16bits
       | (0 << 10)    // [10] RXONLY: Receive only
       | (1 << 9)     // [9] SSM: Software Slave Management. When 1(enabled) the NSS pin input is replaced with the value from SSI bit
       | (1 << 8)     // [8] SSI: Internal Slave select
       | (0 << 7)     // [7] LSBFIRST: 0=MSB tranmit first, 1=LSB
       | (0 << 6)     // [6] SPE: SPI enable
       | (5 << 3)     // [5:3] BR[2:0]: Baud rate control: 0-Fpclk/2, 1=Fpclk/4, 2=Fpclk/8, ...,4=Fpclk/32, ..,  7=Fpclk/256
       | (1 << 2)     // [2] MSTR: Master selection
       | (1 << 1)     // [1] CPOL: Clock polarity: 0=CK to 0 when idle, 1=when idle
       | (1 << 0);    // [0] CPHA: Clock phase: 0=the first clock transition is the first data capture edge
    write16(&spi_bar()->CR1, t2);

    t2 |= (1 << 6);     // [6] SPE: SPI enable
    write16(&spi_bar()->CR1, t2);

    // prio: 0 highest; 7 is lowest
    //nvic_irq_enable(51, 6); // 51 SPI3
}
