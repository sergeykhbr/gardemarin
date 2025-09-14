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
#include <spi_display.h>
#include "gpio_cfg.h"
#include "task.h"

extern int system_clock_hz();

int systick_cnt_ = 0;
int sub_sec_cnt_ = 0;
int time_sec_ = 0;
void *iraw_ = 0;

extern "C" int getTickTime() { return systick_cnt_; }


extern "C" void SysTick_Handler() {
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;

    read32(&systick->CSR);     // Clear [16] COUNTFLAG
    ++systick_cnt_;
    if (++sub_sec_cnt_ >= 100) {
        sub_sec_cnt_ = 0;
        time_sec_++;
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
    int sec_z = 0;
    task_data_type task_data;
    EnableIrqGlobal();
    init_systick();

    display_init();
    display_splash_screen();

    gpio_pin_as_output(&CFG_PIN_LED1, GPIO_NO_OPEN_DRAIN, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_as_input(&CFG_PIN_WATER_LEVEL_DATA, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_as_output(&CFG_PIN_RELAIS_PWR, GPIO_NO_OPEN_DRAIN, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_as_output(&CFG_PIN_RELAIS_PUMP, GPIO_NO_OPEN_DRAIN, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CFG_PIN_RELAIS_PWR);

    task_init(&task_data);

    while(1) {
        if (sec_z != time_sec_) {
             sec_z = time_sec_;

             task_update(&task_data, time_sec_);

            // Switch User LED
            if (time_sec_ & 0x8) {
                gpio_pin_set(&CFG_PIN_LED1);  // LED is OFF

                //gpio_pin_set(&CFG_PIN_RELAIS_PUMP);

            } else {
                //gpio_pin_clear(&CFG_PIN_LED1);  // LED is ON

                //gpio_pin_clear(&CFG_PIN_RELAIS_PUMP);
            }
        }
    }
    return 0;
}
