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

#include <prjtypes.h>
#include <stdio.h>
#include <mcu.h>
#include <uart.h>
#include <fwapi.h>
#include <vprintfmt.h>
#include <gpio_drv.h>
#include "gpio_cfg.h"
#include <RawInterface.h>

extern int system_clock_hz();

int global_cnt = 100000;  // check global var initialization from flash
int systick_cnt = 0;


extern "C" void uart_early_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    USART_registers_type *UART1  = (USART_registers_type *)USART1_BASE;
    uint32_t t1;

    t1 = read32(&RCC->APB2ENR);
    t1 |= (1 << 0)              // APB2[0] AFIO Alternate function
       | (1 << 14);             // APB2[14] USART1
    write32(&RCC->APB2ENR, t1);

    //    PA10 USART1_RX = AF7
    //    PA9  USART1_TX = AF7
    gpio_pin_as_alternate(&CFG_PIN_UART1_RX, 7);
    gpio_pin_as_alternate(&CFG_PIN_UART1_TX, 7);

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
       | (0 << 6)   // transmission complete
       | (0 << 5)
       | (1 << 3)
       | (1 << 2);
    write16(&UART1->CR1, t1);
    write16(&UART1->CR2, 0);
    write16(&UART1->CR3, 0);
}

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


extern "C" void SysTick_Handler() {
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;

    read32(&systick->CSR);     // Clear [16] COUNTFLAG
    if (++systick_cnt >= 100) {
        systick_cnt = 0;
        global_cnt++;

        // Switch User LED
        if (global_cnt & 0x1) {
            gpio_pin_set(&CFG_PIN_LED1);
        } else {
            gpio_pin_clear(&CFG_PIN_LED1);
        }
    }
}


void init_systick() {
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;
    uint32_t t1;

    t1 = system_clock_hz() / 100;     // 100 Hz, SysTick is 24-bits width
    write32(&systick->RVR, t1);       // reload value
    write32(&systick->CVR, t1);        // current value

    t1 = (1 << 2)      // CLKSOURCE: SysTick uses CPU clock (default 1)
       | (1 << 1)       // TICKINT: enable interrupt
       | 1;            // ENABLE:
    write32(&systick->CSR, t1);
}


extern "C" int fwmain(int argcnt, char *args[]) {
    gpio_pin_as_output(&CFG_PIN_LED1, 0, 0, 0);

    fw_init();

    int cnt_z = 0;
    int cycle_cnt= 0;
    uint32_t t1;
    int led = 0;

    EnableIrqGlobal();
    init_systick();

    while(1) {
    }
    return 0;
}
