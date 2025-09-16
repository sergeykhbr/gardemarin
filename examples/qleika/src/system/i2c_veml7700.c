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

#define VEML7700_ADDR  (0x10)   // 7-bit

void veml7700_init() {
    AFIO_registers_type *afio = (AFIO_registers_type *)AFIO_BASE;
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint32_t t1;
    uint16_t t2;

    // VEML7700 I2C Light sensor
    //     [PB10] I2C2 SCL (min 10 kHz - max 400 kHz)  pull-up to 3.3V  (2.2-4.7 kOhm)
    //     [PB11] I2C2 SDA                             pull-up to 3.3V  (2.2-4.7 kOhm)
    //
    gpio_pin_as_alternate(&CFG_PIN_LUMEN_SCL, 6);
    gpio_pin_as_alternate_open_drain(&CFG_PIN_LUMEN_SDA);


    // I2C2 clock on APB1 = 36 MHz
    t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 22);            // APB1[22] I2C2EN
    write32(&RCC->APB1ENR, t1);

    // [15] SWRST software reset: 1=under reset state
    write16(&I2C->CR1, (1 << 15));
    write16(&I2C->CR1, 0);

    // [5:0] FREQ. Periphral clock frequency. Must be configured with the APB clock
    write16(&I2C->CR2, 36);   // APB1 = 36 MHz

    // [15] F/S: I2C master mode selection: 0 = Standard mode (Sm=100kHz); 1=Fast Mode (Fm=400kHz)
    // [14] DUTY: Duty cycle: 0 =1/2; 1=16/9
    // [11:0] CCR Clock control regsiter in Fm/Sm mode
    write16(&I2C->CCR, 180);   // Standard mode 100 kHz. 36MHz/100kHz, duty=1/2 => 360/2=180

    write16(&I2C->TRISE, 37);   // Max rise time

    t2 = read16(&I2C->CR1);
    t2 |= (1 << 0);            // [0] PE: enable periphery
    write16(&I2C->CR1, t2);

    // prio: 0 highest; 7 is lowest
    //nvic_irq_enable(51, 6); // 51 SPI3

    // configure sensor (default settings):
    veml7700_write(0x0, 0x0000);
}

static void I2C_start(uint8_t addr, uint8_t dir) {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    t2 = read16(&I2C->CR1);
    t2 |= (1 << 8);            // [8] START:
    write16(&I2C->CR1, t2);
    // [0] SB: Start bit: 0=no start condition
    while ((read16(&I2C->SR1) & 0x1) == 0) {}

    write16(&I2C->DR, (addr << 1) | dir);
    // [1] ADDR: Address sent: 0=no end of address transmission
    while ((read16(&I2C->SR1) & 0x2) == 0) {}
    read16(&I2C->SR2);      // recommended sequence to clear bit [1] ADDR
}

static void I2C_write(uint8_t data) {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    // [7] TxE: Data register empty: 0=not empty; 1=empty
    while ((read16(&I2C->SR1) & (1 << 7)) == 0) {}
    write16(&I2C->DR, data);
    // [2] BTF: Byte transfer finished
    while ((read16(&I2C->SR1) & (1 << 2)) == 0) {}
}

static uint8_t I2C_read(uint8_t ack) {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    uint16_t t1 = read16(&I2C->CR1);
    if (ack) {
        t1 |= (1 << 10);            // [10] ACK: ack enable
    } else {
        t1 &= ~(1 << 10);           // [10] ACK: ack disable
    }
    write16(&I2C->CR1, t1);
    // [6] RxNE: Data register not empty: 0=empty; 1=not empty
    while ((read16(&I2C->SR1) & (1 << 6)) == 0) {}
    return (uint8_t)read(&I2C->DR);
}

static void I2C_stop() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;
    uint16_t t1 = read16(&I2C->CR1);
    t1 |= (1 << 9);                 // [9] STOP:
    write16(&I2C->CR1, t1);
}


// Write 16-bit value
void veml7700_write(uint8_t reg, uint16_t value) {
    I2C_Start(VEML7700_ADDR, 0);   // write mode
    I2C_Write(reg);
    I2C_Write(value & 0xFF);       // LSB
    I2C_Write(value >> 8);         // MSB
    I2C_Stop();
}

// Read 16-bit value
uint16_t veml7700_read(uint8_t reg) {
    uint16_t val;
    I2C_Start(VEML7700_ADDR, 0);   // write
    I2C_Write(reg);
    I2C_Start(VEML7700_ADDR, 1);   // repeated start, read
    uint8_t lsb = I2C_Read(1);
    uint8_t msb = I2C_Read(0);
    I2C_Stop();
    val = (msb << 8) | lsb;
    return val;
}

uint32_t veml32_lux() {
    uint16_t raw = veml7700_read(0x04);    // ALS_DATA
    float lux = raw * 0.0576f;
    return (uint32_t)(lux + 0.5f);
}

