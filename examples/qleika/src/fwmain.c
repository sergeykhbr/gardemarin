/*
 *  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

typedef struct gpio_pin_type {
    GPIO_registers_type *port;
    int pinidx;
} gpio_pin_type;

extern int system_clock_hz();

int global_cnt = 100000;  // check global var initialization from flash
int systick_cnt = 0;

//    PA10 USART1_RX  (USART1_REMAP=0); PB7 (USART1_REMAP=1);
//    PA9 USART1_TX  (USART1_REMAP=0); PB6 (USART1_REMAP=1);
static const gpio_pin_type CFG_PIN_UART1_RX = {
    (GPIO_registers_type *)GPIOA_BASE, 10
};

static const gpio_pin_type CFG_PIN_UART1_TX = {
    (GPIO_registers_type *)GPIOA_BASE, 9
};

// PC13 output
static const gpio_pin_type CFG_PIN_LED1 = {
    (GPIO_registers_type *)GPIOC_BASE, 13
};

void gpio_pin_as_output(const gpio_pin_type *p,
                        uint32_t odrain,
                        uint32_t speed,
                        uint32_t pushpull) {
    uint32_t t1;

    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    // [3:2] = 00 General purpose output push-pull
    // [1:0] = 01 Output mode, max speed 10 MHz
    t1 |= (0x1 << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);
}


void gpio_pin_as_alternate(const gpio_pin_type *p,
                           uint32_t ADx) {

    uint32_t t1;
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    // [3:2] = 10 Alternate push-pull
    // [1:0] = 00  (alternate could be enabled only when = 00)
    t1 |= (0x8 << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);
}

void gpio_pin_set(const gpio_pin_type *p) {
    write16(&p->port->BSRRL, (1 << p->pinidx));
}

void gpio_pin_clear(const gpio_pin_type *p) {
    //write16(&p->port->BSRRH, (1 << p->pinidx));
    write32(&p->port->BRR, (1 << p->pinidx));
}


void uart_early_init() {
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


void SysTick_Handler() {
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

int fwmain(int argcnt, char *args[]) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;

    gpio_pin_as_output(&CFG_PIN_LED1, 0, 0, 0);

    int cnt_z = 0;
    int cycle_cnt= 0;
    uint32_t t1;
    int led = 0;

    EnableIrqGlobal();
    init_systick();

    while(1) {
        if (cnt_z != global_cnt) {
            //uart_printk("Hello World %d!\r\n", global_cnt);
            cnt_z = global_cnt;

            t1 = read32(&RCC->CR);
            //uart_printk("RCC_CR %08x\r\n", t1);

            //t1 = read32(&FLASH->ACR);
            //uart_printk("FLASH_ACR %08x\r\n", t1);
        }
        /*if  (++cycle_cnt > 2000000) {
            cycle_cnt = 0;
            if (led) {
                gpio_pin_set(&CFG_PIN_LED1);
                led = 0;
            } else {
                gpio_pin_clear(&CFG_PIN_LED1);
                led = 1;
            }
        }*/
    }
    return 0;
}
