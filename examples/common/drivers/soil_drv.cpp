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
#include <string.h>
#include <stdio.h>
#include <mcu.h>
#include <fwapi.h>
#include <uart.h>
#include <gpio_drv.h>
#include "soil_drv.h"

//    PD6 USART2_RX = AF7
//    PD5 USART2_TX = AF7
static const gpio_pin_type rx_pin = {
    (GPIO_registers_type *)GPIOD_BASE, 6
};

static const gpio_pin_type tx_pin = {
    (GPIO_registers_type *)GPIOD_BASE, 5
};

static FwFifo *prxFifo_ = 0;
static FwFifo *ptxFifo_ = 0;

//
extern "C" void USART2_irq_handler() {
    USART_registers_type *dev = (USART_registers_type *)USART2_BASE;
    uint16_t rbyte;
    char tbyte;
    while ((read16(&dev->SR) & (1 << 5)) != 0) {    // [5] RXNE: read data register not empty
        rbyte = read16(&dev->DR);
        if (prxFifo_) {
            fw_fifo_put(prxFifo_, static_cast<char>(rbyte));
        }
    }

    // ORE (overflow) bit is reset by a read to the USART_SR register followed by a USART_DR
    // register read operation.
    //read16(&dev->SR);

    if (read16(&dev->SR) & (1 << 7)) {          // SR[7] TXE transmit data register empty
        // SR[6] TC: transmission complete
        // This bit is set by hardware if the transmission of a frame containing data is complete and if
        // TXE is set. An interrupt is generated if TCIE=1 in the USART_CR1 register. It is cleared by
        // a software sequence (a read from the USART_SR register followed by a write to the
        // USART_DR register). 
        if (ptxFifo_ && !fw_fifo_is_empty(ptxFifo_)) {
            fw_fifo_get(ptxFifo_, &tbyte);
            write16(&dev->DR, static_cast<uint16_t>(tbyte));
        } else {
            write16(&dev->SR, 0);
        }
    }

    nvic_irq_clear(38);
}


SoilDriver::SoilDriver(const char *name)
    : FwObject(name),
    T_("T"),
    moisture_("moisture"),
    salnity_("salnity"),
    EC_("EC"),
    pH_("pH"),
    N_("N"),
    P_("P"),
    K_("K") {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    USART_registers_type *UART2  = (USART_registers_type *)USART2_BASE;
    uint32_t t1;

    T_.make_uint16(0);
    moisture_.make_uint16(0);
    salnity_.make_uint16(0);
    EC_.make_uint16(0);
    pH_.make_uint16(0);
    N_.make_uint16(0);
    P_.make_uint16(0);
    K_.make_uint16(0);

    t1 = read32(&RCC->APB1ENR);
    t1 |= 1 << 17;             // APB1[17] USART2
    write32(&RCC->APB1ENR, t1);

    //    PD6 USART2_RX = AF7
    //    PD5 USART2_TX = AF7
    gpio_pin_as_alternate(&rx_pin, 7);
    gpio_pin_as_alternate(&tx_pin, 7);

    // UART2 on APB1 = 36 MHz
    // 36000000/(16*9600)
    // APB1 = HCLK / 4
    uint32_t t2 = system_clock_hz() / 4 / 9600;
    write16(&UART2->BRR, (uint16_t)t2);

    // [15] OVER8: Oversampling: 0=16; 1=8
    // [13] UE: USART enable (0=disabled)
    // [12] M: word length: 0=8 data, 1=9 data
    // [11] WAKE
    // [10] PCE: Parity control en
    // [9] PS: Parity selection
    // [8] PEIE: PE irq ena
    // [7] TXEIE: TXE irq ena
    // [6] TCIE: Transmission complete irq en
    // [5] RXNEIE: RX not empty irq ena
    // [4] EDLEIE: IDLE irq ena
    // [3] TE: transmitter ena
    // [2] RE: receiver ena
    // [1] RWU: Receiver wake-up
    // [0] SBRK: send break
    t1 = (1 << 13)
       | (1 << 6)
       | (1 << 5)
       | (1 << 3)
       | (1 << 2);
    write16(&UART2->CR1, t1);
    write16(&UART2->CR2, 0);
    write16(&UART2->CR3, 0);

    fw_fifo_init(&rxfifo_, 2*sizeof(DataResponseType));
    prxFifo_ = &rxfifo_;

    fw_fifo_init(&txfifo_, 2*sizeof(queryData_));
    ptxFifo_ = &txfifo_;

}

void SoilDriver::Init() {
    RegisterAttribute(&T_);
    RegisterAttribute(&moisture_);
    RegisterAttribute(&salnity_);
    RegisterAttribute(&EC_);
    RegisterAttribute(&pH_);
    RegisterAttribute(&N_);
    RegisterAttribute(&P_);
    RegisterAttribute(&K_);
    RegisterInterface(static_cast<TimerListenerInterface *>(this));

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(38, 4);
}


void SoilDriver::callbackTimer(uint64_t tickcnt) {
    char *xbyte = reinterpret_cast<char *>(&response_);
    int rxcnt = 0;
    uint16_t v;

    while (!fw_fifo_is_empty(&rxfifo_)) {
        fw_fifo_get(&rxfifo_, xbyte++);
        if (++rxcnt >= sizeof(response_)) {
            xbyte = reinterpret_cast<char *>(&response_);
            rxcnt = 0;
            v = response_.temp[0];
            v |= (v << 8) | response_.temp[1];
            T_.make_uint16(v);

            v = response_.moisture[0];
            v |= (v << 8) | response_.moisture[1];
            moisture_.make_uint16(v);

            v = response_.salinty[0];
            v |= (v << 8) | response_.salinty[1];
            salnity_.make_uint16(v);

            v = response_.EC[0];
            v |= (v << 8) | response_.EC[1];
            EC_.make_uint16(v);

            v = response_.pH[0];
            v |= (v << 8) | response_.pH[1];
            pH_.make_uint16(v);

            v = response_.N[0];
            v |= (v << 8) | response_.N[1];
            N_.make_uint16(v);

            v = response_.P[0];
            v |= (v << 8) | response_.P[1];
            P_.make_uint16(v);

            v = response_.K[0];
            v |= (v << 8) | response_.K[1];
            K_.make_uint16(v);
        }
    }

    xbyte = reinterpret_cast<char *>(&queryData_);
    for (int i = 0; i < sizeof(QueryDataType); i++) {
        fw_fifo_put(&txfifo_, *xbyte);
        xbyte++;
    }

    USART2_irq_handler();
}

