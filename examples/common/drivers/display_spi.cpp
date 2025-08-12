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

#include <prjtypes.h>
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include <new>
#include <uart.h>
#include <spi.h>
#include "display_spi.h"

#define USE_TIM5



// PD[7] = DISPLAY_RES AF0 output
// PC[12] = SPI3_MOSI  AF6 -> AF0 output
// PC[11] = SPI3_MISO  AF0      AF0 output: command/data control
// PC[10] = SPI3_SCK   AF6 -> AF0 output
static const gpio_pin_type DISPLAY_RES = {(GPIO_registers_type *)GPIOD_BASE, 7};  // 88 PD5_SPI_CS3 (error in name PD7_..)
static const gpio_pin_type DISPLAY_SCK = {(GPIO_registers_type *)GPIOC_BASE, 10}; // 78 SPI3_SCK
static const gpio_pin_type DISPLAY_SDA = {(GPIO_registers_type *)GPIOC_BASE, 12}; // 80 SPI3_MOSI
static const gpio_pin_type DISPLAY_DC = {(GPIO_registers_type *)GPIOC_BASE, 11};  // 79 SPI3_MISO. Data/command

IrqHandlerInterface *drv_display_spi_tx_ = 0;


DisplaySPI::DisplaySPI(const char *name) : FwObject(name) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;
    SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;

    estate_ = Idle;
    bitCnt_ = 0;
    drv_display_spi_tx_ = static_cast<IrqHandlerInterface *>(this);


    // Setup SPI interface. Use bitband instead of SPI3 controller because 
    // non-standard 25-bits transactions:
    //   PD[7]  = DISPLAY_RES/PD7_SPI_CS3   AF6 -> AF0 output
    //   PC[12] = DISPLAY_SDA/SPI3_MOSI     AF6 -> AF0 output
    //   PC[11] = DISPLAY_DC/SPI3_MISO      AF6 -> AF0 output: command/data control (disable SPI Rx if SPI controller is used)
    //   PC[10] = DISPLAY_SCK/SPI3_SCK      AF6 -> AF0 output
    gpio_pin_as_output(&DISPLAY_RES,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&DISPLAY_RES);   // reset: active LOW

    // SCK AF6 = SPI3
    gpio_pin_as_alternate(&DISPLAY_SCK, 6);

    // Data/command:
    gpio_pin_as_output(&DISPLAY_DC,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&DISPLAY_DC);

    // SDA AF6 = SPI3
    gpio_pin_as_alternate(&DISPLAY_SDA, 6);

    // adc clock on APB1 = 144/4 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 15);            // APB1[15] SPI3EN
    t1 |= (1 << 3);             // APB1[3] TIM5EN
    write32(&RCC->APB1ENR, t1);

    // SPI3 master, transmit-only
    uint16_t t2;
    t2 = (0 << 15)    // [15] BIDIMODE: 0=2-line unidirectional, 1=1-linne bidirectional
       | (1 << 14)    // [14] BIDIOE: output enable in bidir mode
       | (0 << 13)    // [13] CRCEN: hardware CRC calc ena
       | (0 << 12)    // [12] CRCNEXT: CRC transfer next
       | (0 << 11)    // [11] DFF: Data frame format: 0=8bits, 1=16bits
       | (0 << 10)    // [10] RXONLY: Receive only
       | (1 << 9)     // [9] SSM: Software Slave Management. When 1(enabled) the NSS pin input is replaced with the value from SSI bit
       | (1 << 8)     // [8] SSI: Internal Slave select
       | (0 << 7)     // [7] LSBFIRST: 0=MSB tranmit first, 1=LSB
       | (1 << 6)     // [6] SPE: SPI enable
       | (4 << 3)     // [5:3] BR[2:0]: Baud rate control: 0-Fpclk/2, 1=Fpclk/4, ...,4=Fpclk/32, ..,  7=Fpclk/256
       | (1 << 2)     // [2] MSTR: Master selection
       | (0 << 1)     // [1] CPOL: Clock polarity: 0=CK to 0 when idle
       | (0 << 0);     // [0] CPHA: Clock phase: 0=the first clock transition is the first data capture edge
    write16(&SPI3->CR1, t2);

    t2 = (1 << 7)    // [7] TXEIE: Tx buffer empty IRQ
       | (0 << 6)    // [6] RXNEIE: Rx buffer not empty IRQ
       | (0 << 5)    // [5] ERRIE: Error IRQ
       | (0 << 4)    // [4] FRF: Frame format: 0=SPI motorola; 1=SPI TI
       | (0 << 2)    // [2] SSOE: SS output enable: 0=disabled (can work multimaster mode)
       | (0 << 1)    // [1] TXDMAEN: Tx buffer DMA ena
       | (0 << 0);   // [0] RXDMAEN: Rx buffer DMA ena
    write16(&SPI3->CR2, t2);

    // TIM5 (unused for now)
    write32(&TIM5->CR1.val, 0);         // stop counter
    // time scale 100 nsec
    write16(&TIM5->PSC, system_clock_hz() / 2 / 10000000 - 1);             // prescaler = 1: CK_CNT = (F_ck_psc/(PSC+1))
    write16(&TIM5->DIER, 1);            // [0] UIE - update interrupt enabled

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(50, 5);
}

void DisplaySPI::Init() {
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));
}

void DisplaySPI::handleInterrupt(int *argv) {
}
