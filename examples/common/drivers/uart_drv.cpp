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
#include <vprintfmt.h>
#include "uart_drv.h"

//    PA10 USART1_RX = AF7
//    PA9  USART1_TX = AF7
static const gpio_pin_type rx_pin = {
    (GPIO_registers_type *)GPIOA_BASE, 10
};

static const gpio_pin_type tx_pin = {
    (GPIO_registers_type *)GPIOA_BASE, 9
};

static FwFifo *prxFifo_ = 0;
static FwFifo *ptxFifo_ = 0;
static void *iraw_ = 0;

extern "C" int uartdrv_putchar(int ch, void *putdat) {
    RawInterface *iraw = reinterpret_cast<RawInterface *>(putdat);
    char tbuf[4] = {static_cast<char>(ch), 0};
    if (iraw) {
        iraw->WriteData(tbuf, 1);
    }
    return 0;
}

extern "C" void uart_printf(const char *fmt, ...) {
    if (iraw_ == 0) {
        iraw_ = fw_get_object_interface("uart1", "RawInterface");
    }
    va_list ap;
    va_start(ap, fmt);
    vprintfmt_lib((f_putch)uartdrv_putchar, iraw_, fmt, ap);
    va_end(ap);
}


//
extern "C" void USART1_irq_handler() {
    USART_registers_type *dev = (USART_registers_type *)USART1_BASE;
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
        // SR[6] TC: {RC_W0} transmission complete
        // This bit is set by hardware if the transmission of a frame containing data is complete and if
        // SR[7] TXE: {RO} is set. An interrupt is generated if TCIE=1 in the USART_CR1 register. It is cleared by
        // a software sequence (a read from the USART_SR register followed by a write to the
        // USART_DR register). 
        if (ptxFifo_ && !fw_fifo_is_empty(ptxFifo_)) {
            fw_fifo_get(ptxFifo_, &tbyte);
            write16(&dev->DR, static_cast<uint16_t>(tbyte));
        } else {
            // clear TC (bit 6) and others RC_W0
            write16(&dev->SR, 0);
        }
    }

    nvic_irq_clear(37);
}


UartDriver::UartDriver(const char *name)
    : FwObject(name),
    listener_(0) {
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
       | (1 << 6)   // transmission complete
       | (1 << 5)
       | (1 << 3)
       | (1 << 2);
    write16(&UART1->CR1, t1);
    write16(&UART1->CR2, 0);
    write16(&UART1->CR3, 0);

    fw_fifo_init(&rxfifo_, 256);
    prxFifo_ = &rxfifo_;

    fw_fifo_init(&txfifo_, 256);
    ptxFifo_ = &txfifo_;
}

void UartDriver::Init() {
    RegisterInterface(static_cast<TimerListenerInterface *>(this));
    RegisterInterface(static_cast<RawInterface *>(this));

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(37, 3);
}


void UartDriver::callbackTimer(uint64_t tickcnt) {
    FwList *p = listener_;
    RawListenerInterface *iface;
    char rxbyte;

#ifdef _WIN32
    if (tickcnt == 4000) {
        // [4,2] rgbw:red read
        const char *xxx = ">!00000104,1,02";  // read value
        for (int i = 0; i < strlen(xxx); i++) {
            fw_fifo_put(&rxfifo_, xxx[i]);
        }
    } else if (tickcnt == 5000) {
        // [4,1] rgbw:red enable=0x56 non-zero value
        const char *xxx = ">!00000104,2,8156";
        for (int i = 0; i < strlen(xxx); i++) {
            fw_fifo_put(&rxfifo_, xxx[i]);
        }
    } else if (tickcnt == 5001) {
        // [10,3] hbrg0:dc0_duty enable=0x64(100) any non-zero value
        const char *xxx = ">!0000010B,2,8364";
        for (int i = 0; i < strlen(xxx); i++) {
            fw_fifo_put(&rxfifo_, xxx[i]);
        }
    } else if (tickcnt == 5002) {
        // [10,6] hbrg0:dc1_duty enable=0x64(100) any non-zero value
        const char *xxx = ">!0000010B,2,8664";
        for (int i = 0; i < strlen(xxx); i++) {
            fw_fifo_put(&rxfifo_, xxx[i]);
        }
    } else if (tickcnt == 5003) {
        // [8,0] uled0:state 2 blinking mode
        const char *xxx = ">!00000108,2,8002";
        for (int i = 0; i < strlen(xxx); i++) {
            fw_fifo_put(&rxfifo_, xxx[i]);
        }
    } else if (tickcnt == 5004) {
        // [2,0] relais0:state 1 enable
        const char *xxx = ">!00000102,2,8001";
        for (int i = 0; i < strlen(xxx); i++) {
            fw_fifo_put(&rxfifo_, xxx[i]);
        }
    }
#endif

    rxcnt_ = 0;
    while (!fw_fifo_is_empty(&rxfifo_)) {
        fw_fifo_get(&rxfifo_, &rxbyte);
        if (rxcnt_ < sizeof(rxbuf_)) {
            rxbuf_[rxcnt_++] = rxbyte;
        } else {
            break;
        }
    }

    p = listener_;
    while (p) {
        iface = reinterpret_cast<RawListenerInterface *>(fwlist_get_payload(p));
        iface->RawCallback(rxbuf_, rxcnt_);
        p = p->next;
    }
}

void UartDriver::WriteData(const char *buf, int sz) {
    USART_registers_type *dev = (USART_registers_type *)USART1_BASE;
    int was_empty = fw_fifo_is_empty(&txfifo_);
    for (int i = 0; i < sz; i++) {
        while (fw_fifo_is_full(&txfifo_)) {}
        fw_fifo_put(&txfifo_, buf[i]);
    }

    // [7] TXE, transmit data register empty
    if (was_empty && (read16(&dev->SR) & (1 << 7))) {
        char tbyte;
        fw_fifo_get(&txfifo_, &tbyte);
        write16(&dev->DR, static_cast<uint8_t>(tbyte));
    }  
}

void UartDriver::RegisterRawListener(RawListenerInterface *iface) {
    FwList *item = reinterpret_cast<FwList *>(fw_malloc(sizeof(FwList)));
    fwlist_set_payload(item, iface);
    fwlist_add(&listener_, item);
}

