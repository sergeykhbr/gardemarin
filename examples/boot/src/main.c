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
#include <stm32f4xx_map.h>
#include <uart.h>

extern int system_clock_hz();

int global_cnt = 100000;  // check global var initialization from flash
int systick_cnt = 0;

void SysTick_Handler() {
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;

    read32(&systick->CSR);     // Clear [16] COUNTFLAG
    if (++systick_cnt >= 100) {
        GPIO_registers_type *PE = (GPIO_registers_type *)GPIOE_BASE;
        systick_cnt = 0;
        global_cnt++;

        // Switch User LED
        if (global_cnt & 0x1) {
            // [15:0] BSy: set bit
            write16(&PE->BSRRL, (1 << 2));
        } else {
            // [31:16] BRy: reset bit
            write16(&PE->BSRRH, (1 << 2));
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

int main(int argcnt, char *args[]) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    FLASH_registers_type *FLASH = (FLASH_registers_type *)FLASH_R_BASE;

    int cnt_z = 0;
    uint32_t t1;

    EnableIrqGlobal();
    init_systick();

    while(1) {
        if (cnt_z != global_cnt) {
            uart_printf("Hello World %d!\r\n", global_cnt);
            cnt_z = global_cnt;

            t1 = read32(&RCC->CR);
            uart_printf("RCC_CR %08x\r\n", t1);

            t1 = read32(&FLASH->ACR);
            uart_printf("FLASH_ACR %08x\r\n", t1);

        }
    }
    return 0;
}
