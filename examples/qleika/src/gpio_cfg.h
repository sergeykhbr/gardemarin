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

#pragma once

#include <gpio_drv.h>

// Air sensor D9:
//     PIN1 Src Voltage for FAN 5V    => PA0 output ena/dis +5V
//     PIN2 Src Voltage for FAN 5V    => PA0 output ena/dis +5V
//     PIN3 GND
//     PIN4 GND
//     PIN5 NC
//     PIN6 NC
//     PIN7 RXD PM2.5 UART INPUT (0~3.3V) (response time 0-10 seconds, typical 5 s)
//     PIN8 PWM OUTPUT (0~3.3V)
//     [PA3 Serial2 RX] PIN9 TXD PM2.5 UART OUTPUT (0~3.3V)
//     PIN10 NC
//
//    PA9 USART1_TX  (USART1_REMAP=0); PB6 (USART1_REMAP=1);
//    PA10 USART1_RX  (USART1_REMAP=0); PB7 (USART1_REMAP=1);
static const gpio_pin_type CFG_PIN_AIR_D9_PWR5V = {
    (GPIO_registers_type *)GPIOA_BASE, 8
};
static const gpio_pin_type CFG_PIN_AIR_UART1_TX = {
    (GPIO_registers_type *)GPIOA_BASE, 9
};
static const gpio_pin_type CFG_PIN_AIR_UART1_RX = {
    (GPIO_registers_type *)GPIOA_BASE, 10
};
static const gpio_pin_type CFG_PIN_AIR_D9_PWM = {
    (GPIO_registers_type *)GPIOA_BASE, 11
};


// Pump Relais
//     [PA12] (output) (yellow)
static const gpio_pin_type CFG_PIN_RELAIS_PUMP = {
    (GPIO_registers_type *)GPIOA_BASE, 12
};

// [PB13_MOSFET_1] (output)
static const gpio_pin_type CFG_PIN_MOSFET_1 = {
    (GPIO_registers_type *)GPIOB_BASE, 13
};



// XKC Y25 T12V Liquide level sensor (5-12V)
//     [+5V] Brown VCC (5V)         (output to enable/disable 5V VCC sensor)
//     [PA8]  Yellow  OUT           (input)
//            Blue GND
//            Black MODE (positive/negative selection) -> floating/GND
//
static const gpio_pin_type CFG_PIN_WATER_LEVEL_DATA = {
    (GPIO_registers_type *)GPIOB_BASE, 15
};


// Display SPI1
//     [PB5] SPI1 MOSI            (alternate)
//     [PB4] CMD_DATA (SPI1 MISO) (Output)
//     [PB3] SPI1_SCK             (alternate)
//     [PA15] RST (SPI_NSS)       (Output)
static const gpio_pin_type CFG_PIN_DISPLAY_RES = {
    (GPIO_registers_type *)GPIOA_BASE, 4
};
static const gpio_pin_type CFG_PIN_DISPLAY_SCK = {
    (GPIO_registers_type *)GPIOA_BASE, 5
};
static const gpio_pin_type CFG_PIN_DISPLAY_DC = {
    (GPIO_registers_type *)GPIOA_BASE, 6
};
static const gpio_pin_type CFG_PIN_DISPLAY_SDA = {
    (GPIO_registers_type *)GPIOA_BASE, 7
};

// VEML7700 I2C Light sensor
//     [PB10] I2C2 SCL (min 10 kHz - max 400 kHz)  pull-up to 3.3V  (2.2-4.7 kOhm)
//     [PB11] I2C2 SDA                             pull-up to 3.3V  (2.2-4.7 kOhm)
//
static const gpio_pin_type CFG_PIN_I2C_SCL = {
    (GPIO_registers_type *)GPIOB_BASE, 10
};
static const gpio_pin_type CFG_PIN_I2C_SDA = {
    (GPIO_registers_type *)GPIOB_BASE, 11
};

// Blue LED
//     [PC13] output
//static const gpio_pin_type CFG_PIN_LED1 = {
//    (GPIO_registers_type *)GPIOC_BASE, 13
//};


// DHT22(AM2302 the same in a case) Temperature Humidity sensor (sampling period 2 sec)
//     1 VDD (3.3-6V, typical 5V)
//     [A1] 2 DATA
//     3 NC
//     4 GND
static const gpio_pin_type CFG_PIN_DHT22_DATA = {
    (GPIO_registers_type *)GPIOA_BASE, 1
};


