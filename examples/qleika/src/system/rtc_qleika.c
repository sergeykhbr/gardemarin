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
#include "bkp.h"
#include "rtc_qleika.h"

static void rtc_delay_ms(volatile uint32_t ms) {
    volatile uint32_t n;
    while (ms--) {
        for (n = 0; n < 8000; ++n) {
            __asm__("nop");
        }
    }
}

static int rtc_wait_lse_ready(uint32_t timeout_ms) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    // [1] LSERDY
    while ((read32(&RCC->BDCR) & 0x2) == 0) {
        if (!timeout_ms--) {
            return 0;
        }
        rtc_delay_ms(1);
    }
    return 1;
}

// previous write operation finished
static void rtc_wait_rtoff() {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    uint32_t timeout_ms = 100;
    // [5] RTOFF: 0=Last write operation is ongoing; 1=last write operation terminated
    while ((read32(&RTC->CRL) & (1 << 5)) == 0) {
        if (!timeout_ms--) {
            return;
        }
        rtc_delay_ms(1);
    }
}

static void rtc_wait_sync() {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    // [3] RSF Register sinchronized flag
    // This bit is set by hardware at each time the RTC_CNT and RTC_DIV registers are updated
    // and cleared by software. Before any read operation after an APB1 reset or an APB1 clock
    // stop, this bit must be cleared by software, and the user application must wait until it is set to
    // be sure that the RTC_CNT, RTC_ALR or RTC_PRL registers are synchronized
    uint32_t t1 = read32(&RTC->CRL);
    t1 &= ~(1 <<  3);
    write32(&RTC->CRL, t1);
    while ((read32(&RTC->CRL) & (1 << 3)) == 0) {}
}

static void rtc_enter_config() {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    rtc_wait_rtoff();

    uint32_t t1 = read32(&RTC->CRL);
    // [4] CNF: 1=enter configuration mode
    t1 |= (1 << 4);
    write32(&RTC->CRL, t1);
}

static void rtc_exit_config() {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;

    uint32_t t1 = read32(&RTC->CRL);
    // [4] CNF: 1=enter configuration mode
    t1 &= ~(1 << 4);
    write32(&RTC->CRL, t1);
}

void rtc_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    PWR_registers_type *PWR = (PWR_registers_type *)PWR_BASE;
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    // APB1 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    // Warning: BKP should be init first
    t1 |= (1 << 27)              // APB1[27] BKPEN
        | (1 << 28);             // APB1[28] PWREN
    write32(&RCC->APB1ENR, t1);

    t1 = read32(&PWR->CR);
    t1 |= (1 << 8);             // CR[8]: DBP Disable backup domain write protection. 1=Acces to RTC and Backup enabled
    write32(&PWR->CR, t1);
    rtc_delay_ms(1);  // small delay

    if (bkp_is_rtc_initilized()) {
        rtc_wait_sync();
        return;
    }
    // unlock write protection on all RTC registers except ISR[13:8], TAFCR, BKPxR
    t1 = read32(&RCC->BDCR);
    t1 |= 1;    // [0] LSEON
    write32(&RCC->BDCR, t1);
    if (rtc_wait_lse_ready(500)) {
        // LSE not working
        return;
    }

    // [9:8] RTCSEL: 00=No clock; 01=LSE; 10=LSI; 11==HSE
    t1 = read32(&RCC->BDCR);
    t1 &= ~(0x3 << 8);
    t1 |= (0x1 << 8);
    write32(&RCC->BDCR, t1);


    // [15] RTCEN: 1=RTC clock enabled
    t1 = read32(&RCC->BDCR);
    t1 |= (1 << 15);
    write32(&RCC->BDCR, t1);

    rtc_wait_sync();

    rtc_enter_config();

    // LSE=32768Hz
    uint32_t prescaler = 32767; // 1Hz tick
    write16(&RTC->PRLH, (uint16_t)(prescaler >> 16));
    write16(&RTC->PRLL, (uint16_t)(prescaler));

    // reset counter to some value
    write16(&RTC->CNTH, (uint16_t)(0x33));
    write16(&RTC->CNTL, (uint16_t)(0x22));

    rtc_exit_config();
    bkp_set_rtc_initialized();
}

void rtc_apply_correction(int correction) {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    uint32_t prescaler = (uint32_t)(32767 - correction); // 1Hz tick
    rtc_enter_config();
    write16(&RTC->PRLH, (uint16_t)(prescaler >> 16));
    write16(&RTC->PRLL, (uint16_t)(prescaler));
    rtc_exit_config();

    bkp_set_rtc_correction(correction);
}

uint32_t rtc_get_time() {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    uint32_t high1;
    uint32_t high2;
    uint32_t low;
    do {
        high1 = read16(&RTC->CNTH);
        low = read16(&RTC->CNTL);
        high2 = read16(&RTC->CNTH);
    } while (high1 != high2);
    return (high1 << 16) | low;
}

void rtc_set_time(uint32_t val) {
    RTC_registers_type *RTC = (RTC_registers_type *)RTC_BASE;
    rtc_enter_config();
    write16(&RTC->CNTH, (uint16_t)(val >> 16));
    write16(&RTC->CNTL, (uint16_t)(val));
    rtc_exit_config();
}
