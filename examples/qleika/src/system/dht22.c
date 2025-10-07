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

#include "dht22.h"
#include "../gpio_cfg.h"

static char timeout_ = 0;
static unsigned long long raw_bits_ = 0;
static int raw_temperature_ = 0;
static int raw_moisture_ = 0;
static int errcode_ = 0;

static void startCounter(uint32_t usec) {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM2_BASE;
    tim_cr1_reg_type cr1;

    timeout_ = 0;
    write32(&TIM->ARR, usec);
    write32(&TIM->CNT, usec);

    cr1.val = 0;
    cr1.bits.CEN = 1;
    cr1.bits.OPM = 1;   // one pulse mode
    cr1.bits.DIR = 1;   // downcount
    write32(&TIM->CR1.val, cr1.val);
}

static void stopCounter() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM2_BASE;
    write32(&TIM->CR1.val, 0);
    nvic_irq_clear(28);        // TIM2
    timeout_ = 0;
}

void dht_tim_handler() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM2_BASE;
    uint16_t sr;

    sr = read16(&TIM->SR);

    write32(&TIM->CR1.val, 0); // stop timer
    write16(&TIM->SR, 0);      // clear all pending interrupts
    nvic_irq_clear(28);        // TIM2
    timeout_ = 1;
}

void dht_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM = (TIM_registers_type *) TIM2_BASE;
    // adc clock on APB1 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 0);             // APB1[0] TIM2EN
    write32(&RCC->APB1ENR, t1);

    write32(&TIM->CR1.val, 0);         // stop counter
    write16(&TIM->PSC, 71);            // to form 1 MHz count
    write16(&TIM->DIER, 1);            // [0] UIE - update interrupt enabled

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(28, 3);

    dht_reset();
}

void dht_reset() {
    raw_temperature_ = 0;
    raw_moisture_ = 0;
    gpio_pin_as_output(&CFG_PIN_DHT22_DATA,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_VERY_FAST,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CFG_PIN_DHT22_DATA);
}

void set_error(int errcode) {
    errcode_ = errcode;

    gpio_pin_as_output(&CFG_PIN_DHT22_DATA,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_VERY_FAST,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CFG_PIN_DHT22_DATA);
 
}

int dht_is_error() {
    return errcode_;
}

void dht_update() {
    errcode_ = 0;
    // Tbe: Host the start signal down: 0.8 - 20 ms. Typical 1 ms
    gpio_pin_as_output(&CFG_PIN_DHT22_DATA,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_FAST,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&CFG_PIN_DHT22_DATA);
    startCounter(1000);   // 800...20000
    while (timeout_ == 0) {}

    // Tgo: Bus master has released time
    gpio_pin_set(&CFG_PIN_DHT22_DATA);
    startCounter(30);    // 20...200 usec
    while (timeout_ == 0) {}

    gpio_pin_as_input(&CFG_PIN_DHT22_DATA,
                      GPIO_FAST,
                      GPIO_PULL_UP);
    startCounter(200);    // wait upto 200 usec to response low
    while (gpio_pin_get(&CFG_PIN_DHT22_DATA) != 0) {
        if (timeout_) {
            set_error(1);
            return;    // error
        }
    }
    stopCounter();

    // Trel: Response to low time
    startCounter(200);    // wiat 75..85 usec to response high
    while (gpio_pin_get(&CFG_PIN_DHT22_DATA) == 0) {
        if (timeout_) {
            set_error(2);
            return;    // error
        }
    }
    stopCounter();

    // Treh: In Response to high time
    startCounter(85);    // 75..85 usec to response high
    while (gpio_pin_get(&CFG_PIN_DHT22_DATA) != 0) {
        if (timeout_) {
            set_error(3);
            return;    // error
        }
    }
    stopCounter();

    // Bit 39..0:
    for (int i = 0; i < 40; i++) {
        startCounter(55);    // Tlow 45..55 usec low time of bits always the same
        while (gpio_pin_get(&CFG_PIN_DHT22_DATA) == 0) {
            if (timeout_) {
                set_error(10 + i);
                return;    // error
            }
        }
        stopCounter();

        startCounter(30);    // Signal"0" high time: 22..30
        while (gpio_pin_get(&CFG_PIN_DHT22_DATA) != 0) {
            if (timeout_) {
                break;
            }
        }
        stopCounter();

        if (gpio_pin_get(&CFG_PIN_DHT22_DATA) == 0) {
            raw_bits_ <<= 1;
        } else {
            raw_bits_ <<= 1;
            raw_bits_ |= 1;

            startCounter(75 - 30);    // Signal"1" high time: 68..75
            while (gpio_pin_get(&CFG_PIN_DHT22_DATA) != 0) {
                if (timeout_) {
                    set_error(20 + i);
                    return;    // error
                }
            }
            stopCounter();
        }
    }
    raw_temperature_ = (int)((raw_bits_ >> 8) & 0xFFFF);
    raw_moisture_ = (int)((raw_bits_ >> 24) & 0xFFFF);
    gpio_pin_as_output(&CFG_PIN_DHT22_DATA,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_VERY_FAST,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CFG_PIN_DHT22_DATA);
}


int dht_get_temperature() {
    return raw_temperature_;
}

int dht_get_moisture() {
    return raw_moisture_;
}


