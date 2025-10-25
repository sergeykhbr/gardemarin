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

#define VEML7700_ADDR_WR  (0x20)
#define VEML7700_ADDR_RD  (0x21)

#define VEML7700_CONFIG_REG    0x00
#define VEML7700_ALS_DATA_REG  0x04
#define VEML7700_DATA_REG      0x05
#define VEML7700_DEVICE_ID_REG 0x07

static uint16_t lux_raw_ = 0;

void update_lux() {
    if (is_i2c_busy()) {
        return;
    }

    i2c_read_reg_burst(VEML7700_ADDR_WR,
                       VEML7700_DATA_REG,
                       (uint8_t *)&lux_raw_,
                       sizeof(lux_raw_));
}

int get_lux() {
    int ret = (int)lux_raw_ * 672;
    ret /= 10000;  // 0.0336 lx/bit resolution when GAIN=x2, for GAIN=x1 0.0672 lx/bit
    return ret;
}

// configure sensor (default settings):
void veml7700_configure() {
    I2C_registers_type *I2C = (I2C_registers_type *)I2C2_BASE;

    nvic_irq_disable(33); // 33 I2C2 event

    i2c_start_sequence();
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

    write32(&I2C->DR, VEML7700_CONFIG_REG);  // 0x00:Config; 0x07:DevceID; 0x04:ALS_DATA
    // TxE=1, shift register empty, data register empty, cleared by writing DR
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }

    // Write with auto-increment is supported:
    write32(&I2C->DR, 0x00);  // 0x0000 LSB
    while ((read32(&I2C->SR1) & (1 << 7)) == 0) {
        if (is_i2c_error()) {
            return;
        }
    }

    write32(&I2C->DR, 0x00);  // 0x0000 LSB
    i2c_stop_sequence();

    // [1] BUSY
    while (read32(&I2C->SR2) & (1 << 1)) {
        if (is_i2c_error()) {
            return;
        }
    }

    nvic_irq_clear(33);
}


