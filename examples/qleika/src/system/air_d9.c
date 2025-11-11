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

#include "../gpio_cfg.h"
#include "air_d9.h"

static uint8_t rxbuf_[64] = {0};
static int cnt_ = 0;
static uint8_t *pwr_ = 0;
static uint8_t *prd_ = 0;
static uint16_t checksum_ = 0;
static int delta_baud_ = 0;
static int no_prm_cnt_ = 0;
static int baudrate_confirmed_ = 0;

static int is_msg_valid(uint32_t checksum) {
    if (prd_[4] != prd_[10]
       || prd_[5] != prd_[11]
       || prd_[6] != prd_[12]
       || prd_[7] != prd_[13]
       || prd_[8] != prd_[14]
       || prd_[9] != prd_[15]) {
        return 0;
    }
    return 1;
}

static void set_baudrate(int v) {
    USART_registers_type *UART  = (USART_registers_type *)USART1_BASE;
    uint32_t t2 = system_clock_hz() / (uint32_t)v;
    write16(&UART->BRR, (uint16_t)t2);

}

void air_d9_irq_handler() {
    USART_registers_type *UART  = (USART_registers_type *)USART1_BASE;
    uint16_t t1 = read16(&UART->SR);
    uint8_t s = (uint8_t)read16(&UART->DR);

    if (cnt_ || s == 0x42) {  // [0]=0x42; [1]=0x4d
        if (cnt_ == 1 && s != 0x4d) {
            cnt_ = 0;
            checksum_ = s;
        } else {
            pwr_[cnt_++] = s;
            checksum_ += s;
        }
    } else {
        // reset auto tuning
        if (++no_prm_cnt_ > 64) {
            if (delta_baud_ > 0) {
                delta_baud_ = -delta_baud_;
            } else {
                delta_baud_ = 0;
            }
            set_baudrate(9600 + delta_baud_);
            no_prm_cnt_ = 0;
        }
    }
    if (cnt_ >= 32) {
        uint8_t *ptmp = pwr_;
        pwr_ = prd_;
        prd_ = ptmp;
        if (is_msg_valid(checksum_) == 0) {
            if (baudrate_confirmed_) {
                baudrate_confirmed_--;
            }
            if (baudrate_confirmed_ == 0) {
                if (delta_baud_ < 4800) {
                    if (delta_baud_ > 0) {
                        delta_baud_ = -delta_baud_;
                    } else {
                        delta_baud_ = -delta_baud_ + 100;
                    }
                } else {
                    delta_baud_ = 0;
                }
                set_baudrate(9600 + delta_baud_);
            }
        } else {
            if (baudrate_confirmed_ < 3) {
                baudrate_confirmed_++;
            }
        }
        cnt_ = 0;
        checksum_ = 0;
    }

    write16(&UART->SR, 0);
    nvic_irq_clear(37);
}

void air_d9_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    USART_registers_type *UART1  = (USART_registers_type *)USART1_BASE;
    uint32_t t1;

    pwr_ = rxbuf_;
    prd_ = pwr_ + 32;

    t1 = read32(&RCC->APB2ENR);
    t1 |= 1 << 14;               // APB2[14] USART1EN
    write32(&RCC->APB2ENR, t1);

    //    PA10 USART1_RX = AF7
    //    PA9  USART1_TX = AF7
    gpio_pin_as_alternate(&CFG_PIN_AIR_UART1_RX, 7);
    gpio_pin_as_alternate(&CFG_PIN_AIR_UART1_TX, 7);

    // UART1 on APB2 = 72 MHz
    // APB2 = HCLK
    uint32_t t2 = system_clock_hz() / 9600;
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
       | (0 << 6)   // transmission complete
       | (1 << 5)   // rx not empty
       | (1 << 3)
       | (1 << 2);
    write16(&UART1->CR1, t1);
    write16(&UART1->CR2, 0);
    write16(&UART1->CR3, 0);

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(37, 6);
}


uint32_t air_d9_get_pm1p0() {
    uint32_t ret = prd_[6];
    ret = (ret << 8) | prd_[5];  // [6] - always zero
    return ret;
}

uint32_t air_d9_get_pm2p5() {
    uint32_t ret = prd_[8];
    ret = (ret << 8) | prd_[7];
    return ret;
}

uint32_t air_d9_get_pm10() {
    uint32_t ret = prd_[10];
    ret = (ret << 8) | prd_[9];
    return ret;
}
