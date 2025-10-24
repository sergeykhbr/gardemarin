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

#include "i2c_veml7700.h"
#include "../gpio_cfg.h"

#define VEML7700_ADDR_WR  (0x20)
#define VEML7700_ADDR_RD  (0x21)

typedef enum estate_type {
    state_idle,
    state_error,
    state_EV5_transmitter,   // START bit was generated
    state_EV6_transmitter,   // ADDR sent interrupt
    state_EV8_1_transmitter, // TxE=1 empty interrupt (Shift reg and Data reg are empty)
    state_EV8_transmitter,   // TxE=1 (Data reg is empty)
    state_EV5_receiver,      // START bit was generated
    state_EV6_receiver,      // ADDR=1
    state_EV7_1_receiver,    // RxNE=1, write ACK=0; STOP=1
    state_EV7_receiver,      // RxNE=1
    state_watchdog,          // timeout
} estate_type;

static int irq_cnt_ = 0;
static int err_cnt_ = 0;
static uint32_t err_sr1_ = 0;
static estate_type estate_ = state_idle;
static estate_type err_state_ = state_idle;
static uint16_t lux_raw_ = 0;
static int watchdog_ = 0;

void stop_sequence();

static void set_error(estate_type curstate, uint32_t sr1) {
    estate_ = state_error;
    err_state_ = curstate;
    err_sr1_ = sr1;
    stop_sequence();
    nvic_irq_disable(33);
    err_cnt_++;
}

static void startWatchdog() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM3_BASE;
    tim_cr1_reg_type cr1;
    uint32_t usec = 100000;  // 10ms

    watchdog_ = 0;
    write32(&TIM->ARR, usec);
    write32(&TIM->CNT, usec);

    cr1.val = 0;
    cr1.bits.CEN = 1;
    cr1.bits.OPM = 1;   // one pulse mode
    cr1.bits.DIR = 1;   // downcount
    write32(&TIM->CR1.val, cr1.val);
}

static void stopWatchdog() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM3_BASE;
    write32(&TIM->CR1.val, 0);
    nvic_irq_clear(29);        // TIM3
    watchdog_ = 0;
}

void i2c_watchdog_handler() {
    TIM_registers_type *TIM = (TIM_registers_type *) TIM3_BASE;
    uint16_t sr;

    sr = read16(&TIM->SR);

    write32(&TIM->CR1.val, 0); // stop timer
    write16(&TIM->SR, 0);      // clear all pending interrupts
    nvic_irq_clear(29);        // TIM3
    watchdog_ = 1;
    estate_ = state_watchdog;
    set_error(estate_, 0);
}

void i2c_reset() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1;
    // [15] SWRST software reset: 1=under reset state
    write32(&I2C->CR1, (1 << 15));
    write32(&I2C->CR1, 0);

    nvic_irq_clear(33);

    // [12] LAST
    // [11] DMAEN
    // [10] ITBUFEN.
    // [9] ITEVEN.
    // [8] ITEREN.
    // [5:0] FREQ. Periphral clock frequency. Must be configured with the APB clock
    write32(&I2C->CR2, (1 << 10)   // to generate irq on Rx/Tx events
                     | (1 << 9)
                     | 36);   // APB1 = 36 MHz

    // [15] F/S: I2C master mode selection: 0 = Standard mode (Sm=100kHz); 1=Fast Mode (Fm=400kHz)
    // [14] DUTY: Duty cycle: 0 =1/2; 1=16/9
    // [11:0] CCR Clock control regsiter in Fm/Sm mode
    write32(&I2C->CCR, 180);   // Standard mode 100 kHz. 36MHz/100kHz, duty=1/2 => 360/2=180

    write32(&I2C->TRISE, 37);   // Max rise time

    t1 = read32(&I2C->CR1);
    t1 |= (1 << 0);            // [0] PE: enable periphery
    write32(&I2C->CR1, t1);

    estate_ = state_idle;
}

void i2c_init() {
    AFIO_registers_type *afio = (AFIO_registers_type *)AFIO_BASE;
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    TIM_registers_type *TIM = (TIM_registers_type *) TIM3_BASE;
    uint32_t t1;

    // VEML7700 I2C Light sensor
    //     [PB10] I2C2 SCL (min 10 kHz - max 400 kHz)  pull-up to 3.3V  (2.2-4.7 kOhm)
    //     [PB11] I2C2 SDA                             pull-up to 3.3V  (2.2-4.7 kOhm)
    //
    /*gpio_pin_as_output(&CFG_PIN_I2C_SCL, GPIO_NO_OPEN_DRAIN, GPIO_SLOW, GPIO_NO_PUSH_PULL);
    gpio_pin_as_output(&CFG_PIN_I2C_SDA, GPIO_NO_OPEN_DRAIN, GPIO_SLOW, GPIO_NO_PUSH_PULL);

    gpio_pin_set(&CFG_PIN_I2C_SCL);
    gpio_pin_clear(&CFG_PIN_I2C_SCL);
    gpio_pin_set(&CFG_PIN_I2C_SCL);
    gpio_pin_clear(&CFG_PIN_I2C_SCL);

    gpio_pin_set(&CFG_PIN_I2C_SDA);
    gpio_pin_clear(&CFG_PIN_I2C_SDA);
    gpio_pin_set(&CFG_PIN_I2C_SDA);
    gpio_pin_clear(&CFG_PIN_I2C_SDA);
    gpio_pin_set(&CFG_PIN_I2C_SDA);
    gpio_pin_clear(&CFG_PIN_I2C_SDA);*/
    gpio_pin_as_alternate_open_drain(&CFG_PIN_I2C_SCL);
    gpio_pin_as_alternate_open_drain(&CFG_PIN_I2C_SDA);


    // I2C2 clock on APB1 = 36 MHz
    t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 22);            // APB1[22] I2C2EN
    write32(&RCC->APB1ENR, t1);

    // TIM3 as watchdog
    t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 1);             // APB1[1] TIM3EN
    write32(&RCC->APB1ENR, t1);

    write32(&TIM->CR1.val, 0);         // stop counter
    write16(&TIM->PSC, 710);            // to form 0.1 MHz count
    write16(&TIM->DIER, 1);            // [0] UIE - update interrupt enabled

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(29, 3);

    i2c_reset();
}


void start_sequence() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    startWatchdog();   // 10 msec interval
    uint32_t t1 = read32(&I2C->CR1);
    t1 |= (1 << 8);                // [8] START:
    write32(&I2C->CR1, t1);
}

void stop_sequence() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    stopWatchdog();

    uint32_t t1 = read32(&I2C->CR1);
    t1 |= (1 << 9);                 // [9] STOP:
    write32(&I2C->CR1, t1);
}

void ack_enable() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1 = read32(&I2C->CR1);
    t1 |= (1 << 10);                // [10] ACK: ack enable
    write32(&I2C->CR1, t1);
}

void ack_disable() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1 = read32(&I2C->CR1);
    t1 &= ~(1 << 10);               // [10] ACK:
    t1 |= (1 << 9);                 // [9] STOP:
    write32(&I2C->CR1, t1);
}

void enable_rxtx_event() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1 = read32(&I2C->CR2);
    t1 |= (1 << 10);                // [10] ITBUFEN
    write32(&I2C->CR2, t1);
}

void disable_rxtx_event() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1 = read32(&I2C->CR2);
    t1 &= ~(1 << 10);               // [10] ITBUFEN
    write32(&I2C->CR2, t1);
}

void update_lux() {
    if (estate_ != state_idle) {
        return;
    }
    estate_= state_EV5_transmitter;
    enable_rxtx_event();
    nvic_irq_enable(33, 6); // 33 I2C2 event
    start_sequence();
}

int is_i2c_busy() {
    return estate_ != state_idle && estate_ != state_error ? 1: 0;
}

int is_i2c_error() {
    return estate_ == state_error ? 1: 0;
}

int get_i2c_err_state() {
    return err_state_;
}

int get_lux() {
    int ret = (int)lux_raw_ * 672;
    ret /= 10000;  // 0.0336 lx/bit resolution when GAIN=x2, for GAIN=x1 0.0672 lx/bit
    return ret;
}

void i2c2_ev_handler() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t sr1;
    uint32_t t1;
    irq_cnt_++;

    sr1 = read32(&I2C->SR1);
    switch(estate_) {
    case state_EV5_transmitter:
        // SB=1, cleared by reading SR1 register followed by writting DR register with Address
        write32(&I2C->DR, VEML7700_ADDR_WR);
        // [0] SB
        if (sr1 & 1) {
            estate_ = state_EV6_transmitter;
        } else {
            set_error(estate_, sr1);
        }
        break;
    case state_EV6_transmitter:
        // ADDR=1, cleared by reading SR1 followed by reading SR2
        read32(&I2C->SR2);
        // [1] Address sent
        if (sr1 & (1 << 1)) {
            estate_ = state_EV8_1_transmitter;
        } else {
            set_error(estate_, sr1);
        }
        break;
    case state_EV8_1_transmitter:
        // TxE=1, shift register empty, data register empty, cleared by writing DR
        // ITBUFEN: buffer irq enable. 1:TxE=1 or RxE=1 generates Event interrupt)
        write32(&I2C->DR, 0x05);  // 0x07:DevceID; 0x04:ALS_DATA
        // [7] TxE
        if (sr1 & (1 << 7)) {
            estate_ = state_EV8_transmitter;
        } else {
            set_error(estate_, sr1);
        }
        disable_rxtx_event();  // interrupt should be on BTF flag
        break;
    case state_EV8_transmitter:
        // TxE=1, shift register not empty, data reg empty, cleared by writing DR
        // BUT we transmit one byte only. Unclear how many times irq will be called
        // [7] TxE
        // [2] BTF byte transfer finished
        if (sr1 & (1 << 2)) {
            start_sequence();
            estate_ = state_EV5_receiver;
        } else {
            set_error(estate_, sr1);
        }
        break;
    case state_EV5_receiver:
        // SB=1, cleared by reading SR1 followed by writing DR with address
        // [0] SB
        if (sr1 & 1) {
            write32(&I2C->DR, VEML7700_ADDR_RD);
            estate_ = state_EV6_receiver;
        } else {
            err_sr1_ = sr1;
            start_sequence();
            //set_error(estate_, sr1);
        }
        break;
    case state_EV6_receiver:
        // ADDR=1, cleared by reading SR1 followed by reading SR2
        read32(&I2C->SR2);
        ack_enable();
        // [1] Address sent
        if (sr1 & (1 << 1)) {
            estate_ = state_EV7_1_receiver;
            enable_rxtx_event();
        } else {
            set_error(estate_, sr1);
        }
        break;
    case state_EV7_1_receiver:
        // RxNE=1 cleared by reading DR, program ACK=0 and STOP request
        lux_raw_ = (uint8_t)read32(&I2C->DR);
        ack_disable();  // ACK=0, STOP=1

        // [6] RxNE
        if (sr1 & (1 << 6)) {
            estate_ = state_EV7_receiver;
        } else {
            set_error(estate_, sr1);
        }
        break;
    case state_EV7_receiver:
        // RxNE=1 cleared by reading DR register
        lux_raw_ |= (read32(&I2C->DR) & 0xFF) << 8;
        estate_ = state_idle;
        break;
    case state_idle:
        // shouldn't be here
        set_error(estate_, sr1);
        break;
    case state_watchdog:
        set_error(estate_, sr1);
        break;
    case state_error:
        break;
    default:;
    }

    nvic_irq_clear(33);
}



// configure sensor (default settings):
void veml7700_configure() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    nvic_irq_disable(33); // 33 I2C2 event

    start_sequence();
    while ((read32(&I2C->SR1) & 1) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }
    // SB=1, cleared by reading SR1 register followed by writting DR register with Address
    write32(&I2C->DR, VEML7700_ADDR_WR);

    // ADDR=1, cleared by reading SR1 followed by reading SR2
    while ((read32(&I2C->SR1) & (1 << 1)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }
    read32(&I2C->SR2);

    write32(&I2C->DR, 0x00);  // 0x00:Config; 0x07:DevceID; 0x04:ALS_DATA
    // TxE=1, shift register empty, data register empty, cleared by writing DR
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }

    write32(&I2C->DR, 0x00);  // 0x0000 LSB
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }

    write32(&I2C->DR, 0x00);  // 0x0000 LSB
    stop_sequence();

    // [1] BUSY
    while (read32(&I2C->SR2) & (1 << 1)) {
        if (is_i2c_error()) {
            return;
        }
    }

    nvic_irq_clear(33);
}

/*uint32_t veml32_lux() {
    uint16_t raw = veml7700_read(0x04);    // ALS_DATA
    float lux = raw * 0.0576f;
    return (uint32_t)(lux + 0.5f);
}*/

