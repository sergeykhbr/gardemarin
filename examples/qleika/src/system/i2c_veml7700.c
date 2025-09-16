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
    state_set_i2c_addr_wr, // I2C write command
    state_set_reg_addr,    // set VELM7700 register address
    state_data_sb1,        // Check that address was sent and start new sequence
    state_set_i2c_addr_rd, // I2C read command
    state_read_lsb,
    state_read_msb,
    state_stop
} estate_type;

int irq_cnt_ = 0;
int err_cnt_ = 0;
uint32_t tcr1_ = ~0ul;
estate_type estate_ = state_idle;
estate_type err_state_ = state_idle;
uint16_t lux_raw_ = 0;

static void set_error(estate_type curstate) {
    err_state_ = curstate;
    err_cnt_++;
}

void start_sequence() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1 = read32(&I2C->CR1);
    t1 |= (1 << 8);            // [8] START:
    write32(&I2C->CR1, t1);
}

uint16_t update_lux_raw() {
    if (estate_ != state_idle) {
        return lux_raw_;
    }
    estate_= state_set_i2c_addr_wr;
    start_sequence();
    return lux_raw_;
}

void i2c2_ev_handler() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t sr1;
    uint32_t t1;
    irq_cnt_++;

    switch(estate_) {
    case state_set_i2c_addr_wr:
        sr1 = read32(&I2C->SR1);
        write32(&I2C->DR, VEML7700_ADDR_WR);
        // [0]
        if ((sr1 & 1) == 0) {
            set_error(estate_);
        }
        estate_ = state_set_reg_addr;
        break;
    case state_set_reg_addr:
        sr1 = read32(&I2C->SR1);
        read32(&I2C->SR2);    // recommended sequence to clear bit [1] ADDR
        write32(&I2C->DR, 0x04);  // ALS_DATA
        // [1] Address sent
        if ((sr1 & (1 << 1)) == 0) {
            set_error(estate_);
        }
        estate_ = state_data_sb1;
        break;
    case state_data_sb1:
        sr1 = read32(&I2C->SR1);
        start_sequence();
        if ((sr1 & (1 << 2)) == 0) {
            set_error(estate_);
        }
        estate_ = state_set_i2c_addr_rd;
         // [2] BTF: Byte transfer finished
        break;
    case state_set_i2c_addr_rd:
        sr1 = read32(&I2C->SR1);
        write32(&I2C->DR, VEML7700_ADDR_RD);
        // [0]
        if ((sr1 & 1) == 0) {
            set_error(estate_);
        }
        estate_ = state_read_lsb;
        break;
    case state_read_lsb:
        sr1 = read32(&I2C->SR1);
        read32(&I2C->SR2);    // recommended sequence to clear bit [1] ADDR
        t1 = read32(&I2C->CR1);
        t1 |= (1 << 10);            // [10] ACK: ack enable
        write32(&I2C->CR1, t1);

        //write32(&I2C->DR, 0);
        // [1] Address sent
        if ((sr1 & (1 << 1)) == 0) {
            set_error(estate_);
        }
        estate_ = state_read_msb;
        break;
    case state_read_msb:
        sr1 = read32(&I2C->SR1);
        lux_raw_ = (uint8_t)read32(&I2C->DR);
        t1 = read32(&I2C->CR1);
        t1 &= ~(1 << 10);            // [10] ACK: ack disable
        write32(&I2C->CR1, t1);
        //read32(&I2C->SR1);
        //write32(&I2C->DR, 0);

        // [6] RxNE: Data register not empty: 0=empty; 1=not empty
        if ((sr1 & (1 << 6)) == 0) {
            set_error(estate_);
        }
        estate_ = state_stop;
        break;
    case state_stop:
        sr1 = read32(&I2C->SR1);
        lux_raw_ |= (read32(&I2C->DR) & 0xFF) << 8;
        // [6] RxNE: Data register not empty: 0=empty; 1=not empty
        if ((sr1 & (1 << 6)) == 0) {
            set_error(estate_);
        }
        estate_ = state_idle;

        t1 = read32(&I2C->CR1);
        t1 |= (1 << 9);                 // [9] STOP:
        write32(&I2C->CR1, t1);
        break;
    case state_idle:
        tcr1_ = read32(&I2C->CR1);
        read32(&I2C->CR2);
    default:;
    }

    nvic_irq_clear(33);
}

void veml7700_init() {
    AFIO_registers_type *afio = (AFIO_registers_type *)AFIO_BASE;
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1;

    // VEML7700 I2C Light sensor
    //     [PB10] I2C2 SCL (min 10 kHz - max 400 kHz)  pull-up to 3.3V  (2.2-4.7 kOhm)
    //     [PB11] I2C2 SDA                             pull-up to 3.3V  (2.2-4.7 kOhm)
    //
    gpio_pin_as_alternate_open_drain(&CFG_PIN_LUMEN_SCL);
    gpio_pin_as_alternate_open_drain(&CFG_PIN_LUMEN_SDA);


    // I2C2 clock on APB1 = 36 MHz
    t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 22);            // APB1[22] I2C2EN
    write32(&RCC->APB1ENR, t1);

    // [15] SWRST software reset: 1=under reset state
    write32(&I2C->CR1, (1 << 15));
    write32(&I2C->CR1, 0);

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

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(33, 6); // 33 I2C2 event
}

static void I2C_start(uint8_t addr) {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    uint32_t t1 = read32(&I2C->CR1);
    t1 |= (1 << 8);            // [8] START:
    write32(&I2C->CR1, t1);
    // [0] SB: Start bit: 0=no start condition
    while ((read32(&I2C->SR1) & 0x1) == 0) {}

    write32(&I2C->DR, addr);
    // [1] ADDR: Address sent: 0=no end of address transmission
    while ((read32(&I2C->SR1) & 0x2) == 0) {}
    read32(&I2C->SR2);      // recommended sequence to clear bit [1] ADDR
}

static void I2C_write(uint8_t data) {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    // [7] TxE: Data register empty: 0=not empty; 1=empty
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {}
    write32(&I2C->DR, data);
    // [2] BTF: Byte transfer finished
    while ((read32(&I2C->SR1) & (1 << 2)) == 0) {}
}

static uint8_t I2C_read(uint8_t ack) {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    uint32_t t1 = read32(&I2C->CR1);
    if (ack) {
        t1 |= (1 << 10);            // [10] ACK: ack enable
    } else {
        t1 &= ~(1 << 10);           // [10] ACK: ack disable
    }
    write32(&I2C->CR1, t1);
    // [6] RxNE: Data register not empty: 0=empty; 1=not empty
    while ((read32(&I2C->SR1) & (1 << 6)) == 0) {}
    return (uint8_t)read32(&I2C->DR);
}

static void I2C_stop() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1 = read32(&I2C->CR1);
    t1 |= (1 << 9);                 // [9] STOP:
    write32(&I2C->CR1, t1);
}

// Write 16-bit value
void veml7700_write(uint8_t reg, uint16_t value) {
    I2C_start(VEML7700_ADDR_WR);   // write mode
    I2C_write(reg);
    I2C_write(value & 0xFF);       // LSB
    I2C_write(value >> 8);         // MSB
    I2C_stop();
}

// Read 16-bit value
uint16_t veml7700_read(uint8_t reg) {
    uint16_t val;
    I2C_start(VEML7700_ADDR_WR);   // write
    I2C_write(reg);
    I2C_start(VEML7700_ADDR_RD);   // repeated start, read
    uint8_t lsb = I2C_read(1);
    uint8_t msb = I2C_read(0);
    I2C_stop();
    val = (msb << 8) | lsb;
    return val;
}

// configure sensor (default settings):
void veml7700_configure() {
    veml7700_write(0x0, 0x0000);
}

uint32_t veml32_lux() {
    uint16_t raw = veml7700_read(0x04);    // ALS_DATA
    float lux = raw * 0.0576f;
    return (uint32_t)(lux + 0.5f);
}

