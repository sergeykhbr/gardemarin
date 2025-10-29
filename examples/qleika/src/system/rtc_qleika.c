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

#include <mcu.h>
#include "rtc_qleika.h"

void rtc_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    PWR_registers_type *PWR = (PWR_registers_type *)PWR_BASE;
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    // APB1 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    uint32_t ttime = 0x00215530;
    uint32_t tdate = 0x00251030;

    t1 |= (1 << 28);             // APB1[28] PWREN
    write32(&RCC->APB1ENR, t1);

    t1 = read32(&PWR->CR);
    t1 |= (1 << 8);             // CR[8]: DBP Disable backup domain write protection. 1=Acces to RTC and Backup enabled
    write32(&PWR->CR, t1);

    // [4] INITS: 1=Calendar was initialized
    t1 = read32(&RTC->ISR);
    if ((t1 & (1 << 4))) {
        // RTC: was initialized before reset
        ttime = read32(&RTC->TR);
        tdate = read32(&RTC->DR);
    }
    // unlock write protection on all RTC registers except ISR[13:8], TAFCR, BKPxR
    write32(&RCC->BDCR, 1 << 16);   // [16] BDRST. Backup domain software reset only after PWR_CR=1

    write8(&RTC->WPR, 0xCA);
    write8(&RTC->WPR, 0x53);


    int wdog = 0;
    int rtc_available = 1;
    // [16] BDRST: rw
    // [15] RTCEN: rw
    // [9:8] RTCSEL rw 00=no clock; 01=LSE; 10=LSI; 11=HSE
    // [2] LSEBYP rw
    // [1] LSERDY r
    // [0] LSEON rw
    t1 = (1 << 15)                  // [15] RTC ON
       | (0x1 << 8)                 // [9:8] 01=LSE
       | (1 << 0);                  // [0] LSE ON
    write32(&RCC->BDCR, t1);

    // Wait [1] LSERDY flag
    while ((read32(&RCC->BDCR) & (1 << 1)) == 0) {
        if (++wdog > system_clock_hz() / 10) {
            rtc_available = 0;
            break;
        }
    }

    // [4] INITS: 1=Calendar was initialized
    t1 = read32(&RTC->ISR);
    if (rtc_available && (t1 & (1 << 4)) == 0) {
        t1 |= (1 << 7);             // [7] INIT
        write32(&RTC->ISR, t1);
        // Wait [6] INITF Calendar registers update is allowed
        wdog = 0;
        while ((read32(&RTC->ISR) & (1 << 6)) == 0) {
            if (++wdog > system_clock_hz() / 10) {
                break;
            }
        }

        // RTC_CR (default is 0):
        // [23] COE Calibration output: 0=disabled
        // [22:21] OSEL Output selection: 00=disabled
        // [20] POL Output polarity
        // [19] COSEL Calibration output selection: 0=512 Hz; 1= 1 Hz
        // [18] BKP Backup: control by user to memorize something
        // [17] SUB1H Subtract 1 hour: 0=no effect
        // [16] ADD1H Add 1 hour: 0=no effect
        // [15] TSIE Timestamp interrupt: 0=disabled
        // [14] WUTIE Wake-up timer interrupt: 0=disabled 
        // [13] ALRBIE Alarm B interrupt: 0=disabled
        // [12] ALRAIE Alarm A interrupt: 0=disabled
        // [11] TSE Time stamp: 0=disabled
        // [10] WUTE Wake-up timer: 0=disabled
        // [9] ALRBE Alarm B: 0=disabled
        // [8] ALRAE Alarm A: 0=disabled
        // [7] DCE Coarse digital calibration: 0=disabled
        // [6] FMT Hour formar 0=24 hour/day; 1=AM/PM
        // [5] BYPSHAD Bypass the shadow registers: 0=TR,DR,SSR are taken from shadow registers
        // [4] REFCKON Reference clock detection: 0=disabled
        // [3] TSEDGE Timestamp event active edge: 0=rising
        // [2:0] WUCLKSEL Wake-up clock selection: 000=RTC/16 clock

        // Set default Date and Time:
        write32(&RTC->TR, ttime);
        write32(&RTC->DR, tdate);
        
        t1 &= ~(1 << 7);            // [7] INIT
        write32(&RTC->ISR, t1);
    }
}

uint32_t rtc_get_time() {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    return read32(&RTC->TR);
}

void rtc_set_time(uint32_t val) {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    int wdog = 0;

    write32(&RTC->ISR, (1 << 7)); // [7] INIT
    // Wait [6] INITF Calendar registers update is allowed
    while ((read32(&RTC->ISR) & (1 << 6)) == 0
        && ++wdog < (system_clock_hz() / 1000)) {}

    write32(&RTC->TR, val);

    write32(&RTC->ISR, 0);          // [7] INIT
}
