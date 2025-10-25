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

#include "i2c_common.h"
#include "i2c_veml7700.h"
#include "../gpio_cfg.h"

#define BMP280_ADDR_WR  (0xEC)
#define BMP280_ADDR_RD  (0xED)

#define BMP280_id         0xD0  // expected 0x58
#define BMP280_reset      0xE0  // write only
// status[3]: measuring[0] '1' Conversion is running
// status[0]: im_update[0] '1' NVM data are being copied to image registers
#define BMP280_status     0xF3  //
// ctrl_meas[7:5]:  osrs_t[2:0] Controls oversampling of temperature data
// ctrl_meas[4:2]:  osrs_p[2:0] Controls oversampling of pressure data
// ctrl_meas[1:0]:  mode[1:0] Controls the power mode of the device
#define BMP280_ctrl_meas  0xF4  //
// config[7:5]: t_sb[2:0] Controls inactive duration Tstandby in normal mode. See 3.6.3
// config[4:2]: filter[2:0] IIR filter time constant
// config[0]:   spi3w_en[0] Enable 3-wire SPI interface when set to '1'
#define BMP280_config     0xF5  //
#define BMP280_press_msb  0xF7  // press_msb[7:0]: [19:12] of raw pressure.
#define BMP280_press_lsb  0xF8  // press_lsb[7:0]: [11:4] of raw pressure.
#define BMP280_press_xlsb 0xF9  // (bit 7,6,5,4) press_xlsb[3:0]: [3:0] of raw pressure. Content depends on T resolution (see table 5)
#define BMP280_temp_msb   0xFA  // temp_msb[7:0]: [19:12] of raw temperature.
#define BMP280_temp_lsb   0xFB  // temp_lsb[7:0]: [11:4] of raw temperature.
#define BMP280_temp_xlsb  0xFC  // (bit 7,6,5,4) temp_xlsb[3:0]: [3:0] of raw temperature. Content depends on Pa resolution (see table 4)

#pragma pack(1)
typedef struct bmp_calib_type {
    uint16_t T1;       // 0x88/0x89
    int16_t T2;        // 0x8a/0x8b
    int16_t T3;        // 0x8c/0x8d
    uint16_t P1;       // 0x8e/0x8f
    int16_t P2;        // 0x90/0x91
    int16_t P3;        // 0x92/0x93
    int16_t P4;        // 0x94/0x95
    int16_t P5;        // 0x96/0x97
    int16_t P6;        // 0x98/0x99
    int16_t P7;        // 0x9a/0x9b
    int16_t P8;        // 0x9c/0x9d
    int16_t P9;        // 0x9e/0x9f
    uint16_t reserved; // 0xa0/0xa1
} bmp_calib_type;
#pragma pack()

static bmp_calib_type dig_;
static uint8_t id_;
static uint8_t pa_raw_[3];

void update_pressure() {
    if (is_i2c_busy()) {
        return;
    }

    i2c_read_reg_burst(BMP280_ADDR_WR,
                       BMP280_press_msb,
                       pa_raw_,
                       sizeof(pa_raw_));
}

int get_pressure() {
    int ret = id_;
    return ret;
}

// configure sensor (default settings):
void bmp280_configure() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    nvic_irq_disable(33); // 33 I2C2 event

    i2c_start_sequence();
    while ((read32(&I2C->SR1) & 1) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }
    // SB=1, cleared by reading SR1 register followed by writting DR register with Address
    write32(&I2C->DR, BMP280_ADDR_WR);

    // ADDR=1, cleared by reading SR1 followed by reading SR2
    while ((read32(&I2C->SR1) & (1 << 1)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }
    read32(&I2C->SR2);

    write32(&I2C->DR, BMP280_ctrl_meas);
    // TxE=1, shift register empty, data register empty, cleared by writing DR
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }

    // Write with auto-increment not supported:
    // ctrl_meas[7:5]:  osrs_t[2:0] Controls oversampling of temperature data
    // ctrl_meas[4:2]:  osrs_p[2:0] Controls oversampling of pressure data
    // ctrl_meas[1:0]:  mode[1:0] Controls the power mode of the device (11 = normal mode)
    // Ultra High resolution: osrs_t=2; osrs_p=16
    // oversampling:
    //     000 output set to 0x80000
    //     001 x1
    //     010 x2
    //     011 x4
    //     100 x8
    //     101, 110, 111 x16
    write32(&I2C->DR, (2 << 5) | (0x6 << 2) | 0x3);  // x2, x16, normal
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }


    // config[7:5]: t_sb[2:0] Controls inactive duration Tstandby in normal mode. See 3.6.3
    // config[4:2]: filter[2:0] IIR filter time constant
    // config[0]:   spi3w_en[0] Enable 3-wire SPI interface when set to '1'
    write32(&I2C->DR, BMP280_config);
    // TxE=1, shift register empty, data register empty, cleared by writing DR
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }

    // config[7:5]: t_sb[2:0] =100b= 500 ms Tstandby
    // config[4:2]: = 000b Filter off
    write32(&I2C->DR, (0x4 << 5) | (0 << 2));
    i2c_stop_sequence();

    // [1] BUSY
    while (read32(&I2C->SR2) & (1 << 1)) {
        if (is_i2c_error()) {
            return;
        }
    }

    nvic_irq_clear(33);

    i2c_read_reg_burst(BMP280_ADDR_WR,
                       BMP280_id,
                       &id_,
                       1);
    while (is_i2c_busy()) {}
}


