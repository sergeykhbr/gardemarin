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
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include <uart.h>
#include <gpio_drv.h>
#include "uart_drv.h"

//    PA10 USART1_RX = AF7
//    PA9  USART1_TX = AF7
static const gpio_pin_type rx_pin = {
    (GPIO_registers_type *)GPIOA_BASE, 10
};

static const gpio_pin_type tx_pin = {
    (GPIO_registers_type *)GPIOA_BASE, 9
};

//
extern "C" void USART1_irq_handler() {
    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("uart1", "IrqHandlerInterface"));
    if (iface) {
        int argv = 0;
        iface->handleInterrupt(&argv);
    }

    nvic_irq_clear(37);
}


UartDriver::UartDriver(const char *name)
    : FwObject(name),
    listener_(0),
    rxfifo_(sizeof(rxbuf_)) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    USART_registers_type *UART1  = (USART_registers_type *)USART1_BASE;
    uint32_t t1;

    t1 = read32(&RCC->APB2ENR);
    t1 |= 1 << 4;             // APB2[4] USART1
    write32(&RCC->APB2ENR, t1);

    //    PA10 USART1_RX = AF7
    //    PA9  USART1_TX = AF7
    gpio_pin_as_alternate(&rx_pin, 7);
    gpio_pin_as_alternate(&tx_pin, 7);

    // UART1 on APB2 = 72 MHz
    // 72000000/(16*115200) = 39.0625
    // APB2 = HCLK / 2
    uint32_t t2 = system_clock_hz() / 2 / 115200;
    write16(&UART1->BRR, (uint16_t)t2);

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
       | (1 << 5)
       | (1 << 3)
       | (1 << 2);
    write16(&UART1->CR1, t1);
    write16(&UART1->CR2, 0);
    write16(&UART1->CR3, 0);
}

void UartDriver::Init() {
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));
    RegisterInterface(static_cast<RawInterface *>(this));

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(37, 3);
}

void UartDriver::handleInterrupt(int *argv) {
    USART_registers_type *dev = (USART_registers_type *)USART1_BASE;
    char rbyte;
    while ((read16(&dev->SR) & (1 << 5)) != 0) {    // [5] RXNE: read data register not empty
        rbyte = static_cast<char>(read16(&dev->DR));
        rxfifo_.put(&rbyte);
    }
}

void UartDriver::callbackTimer(uint64_t tickcnt) {
    FwList *p = listener_;
    RawListenerInterface *iface;

    if (rxfifo_.isEmpty()) {
        return;
    }

    rxcnt_ = 0;
    while (!rxfifo_.isEmpty()) {
        rxfifo_.get(&rxbuf_[rxcnt_++]);
    }

    while (p) {
        iface = reinterpret_cast<RawListenerInterface *>(fwlist_get_payload(p));
        iface->RawCallback(rxbuf_, rxcnt_);
        p = p->next;
    }

    // Debug ouput of input stream:
    if (rxcnt_ >= sizeof(rxbuf_)) {
        rxcnt_ = sizeof(rxbuf_) - 1;
    }
    rxbuf_[rxcnt_] = '\0';
    uart_printf(">>%s\r\n", rxbuf_);
}

void UartDriver::WriteData(const char *buf, int sz) {
    USART_registers_type *dev = (USART_registers_type *)USART1_BASE;
    for (int i = 0; i < sz; i++) {
        while ((read16(&dev->SR) & (1 << 7)) == 0) {}  // [7] TXE, transmit data register empty
        write16(&dev->DR, (uint8_t)buf[i]);
    }
}

void UartDriver::RegisterRawListener(RawListenerInterface *iface) {
    FwList *item = reinterpret_cast<FwList *>(fw_malloc(sizeof(FwList)));
    fwlist_set_payload(item, iface);
    fwlist_add(&listener_, item);
}

