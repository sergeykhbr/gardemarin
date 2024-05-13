/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
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
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include <new>
#include <uart.h>
#include "loadsensor.h"

// Chip select pins per channel
struct LoadCellCfgType {
    const char *portname;        // all names should be placed into flash, do not use SRAM for that
    gpio_pin_type cs_gpio_cfg;
    const char *attr_value_name;
    const char *attr_alpha_name;
    const char *attr_zero_name;
    const char *attr_tara_name;    
    const char *attr_gram_name;    
};

static const LoadCellCfgType CELL_CONFIG[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    {"scale0", {(GPIO_registers_type *)GPIOD_BASE, 2}, "value0", "alpha0", "zero0", "tara0", "gram0"},
    {"scale1", {(GPIO_registers_type *)GPIOD_BASE, 3}, "value1", "alpha1", "zero1", "tara1", "gram1"},
    {"scale2", {(GPIO_registers_type *)GPIOD_BASE, 4}, "value2", "alpha2", "zero2", "tara2", "gram2"},
    {"scale3", {(GPIO_registers_type *)GPIOD_BASE, 7}, "value3", "alpha3", "zero3", "tara3", "gram3"}
};

// PC[12] = SPI3_MOSI  AF6 -> AF0 output (unused by HX711)
// PC[11] = SPI3_MISO  AF6 -> AF0 input
// PC[10] = SPI3_SCK   AF6 -> AF0 output
static const gpio_pin_type SPI3_MOSI = {(GPIO_registers_type *)GPIOC_BASE, 12};
static const gpio_pin_type SPI3_MISO = {(GPIO_registers_type *)GPIOC_BASE, 11};
static const gpio_pin_type SPI3_SCK = {(GPIO_registers_type *)GPIOC_BASE, 10};

// callibration data:
// 0 sewer    y = 892.97*x + 344332 (R^2 = 0.9998)
//          0 00056200
//      +124  00071420
//     ++375  000c2d40
//       2000 00208f00
//     ++2000 00275280
// 1 plant    y = 406.01*x + 4E+06
//          0 003c1480
//      +124  003cd440
//     ++375  003f2300
//      2000  00487980
//    ++2000  004b8900
// 3 mix       y = 404.58*x + 1E+06
//          0 0011b400 (3.3 kg)
//      +124  00127800
//     ++375  0014b400
//      2000  001e2200
//    ++2000  00210c00
static const float INIT_ALPHA[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    1.0f/892.97f,
    1.0f/406.01f,
    1.0f/404.58f,
    1.0f
};

static const float INIT_ZERO[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    395.0f,
    9697.81f,
    535.0f,
    0.0f
};

static const float INIT_TARA[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    0.0f,
    0.0f,
    0.0f,
    0.0f
};

LoadSensorDriver::LoadSensorDriver(const char *name) : FwObject(name) {
    for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
        chn_[i].port = new(fw_malloc(sizeof(LoadSensorPort)))
               LoadSensorPort(static_cast<FwObject *>(this), i);
    }
    selectChannel(-1);

    // Setup SPI interface. Use bitband instead of SPI3 controller because 
    // non-standard 25-bits transactions:
    //     PC[12] = SPI3_MOSI  AF6 -> AF0 output (unused by HX711)
    //     PC[11] = SPI3_MISO  AF6 -> AF0 input
    //     PC[10] = SPI3_SCK   AF6 -> AF0 output
    gpio_pin_as_output(&SPI3_SCK,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&SPI3_SCK);

    // MISO:
    gpio_pin_as_input(&SPI3_MISO,
                      GPIO_MEDIUM,
                      GPIO_NO_PUSH_PULL);

    // MOSI (unused by HX711)
    gpio_pin_as_output(&SPI3_MOSI,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&SPI3_MOSI);
}

void LoadSensorDriver::Init() {
    RegisterInterface(static_cast<RunInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));

    for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
        chn_[i].port->Init();
    }
}

void LoadSensorDriver::selectChannel(int chidx) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOD_BASE;
    uint32_t t1 = read32(&P->ODR);
    t1 |= 0x9C;  // all MISO to z-state
    if (chidx >= 0 && chidx < GARDEMARIN_LOAD_SENSORS_TOTAL) {
        t1 &= ~(1 << CELL_CONFIG[chidx].cs_gpio_cfg.pinidx);
    }
    write32(&P->ODR, t1);
}

void LoadSensorDriver::setSleep() {
    gpio_pin_set(&SPI3_SCK);
    system_delay_ns(60000);
}

void LoadSensorDriver::callbackTimer(uint64_t tickcnt) {
    uint32_t ready = 0;
    uint32_t value[GARDEMARIN_LOAD_SENSORS_TOTAL];
    for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
        selectChannel(i);

        value[i] = 0;
        if (gpio_pin_get(&SPI3_MISO) == 0) {
            ready |= 1 << i;
        }
    }

    // 27 clocks to read DOUT with gain 64
    for (int i = 0; i < 27; i++) {
        gpio_pin_set(&SPI3_SCK);
        system_delay_ns(1000);   // 0.2 us; typical 1 us; max 50 us

        gpio_pin_clear(&SPI3_SCK);
        system_delay_ns(1000);   // 0.2 us; typical 1 us

        for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
            selectChannel(i);
            value[i] <<= 1;
            value[i] |= gpio_pin_get(&SPI3_MISO);
            chn_[i].port->setSensorValue(value[i]);
        }
    }

    selectChannel(-1);    // deselect all
}


LoadSensorPort::LoadSensorPort(FwObject *parent, int idx) : 
    FwAttribute(CELL_CONFIG[idx].attr_value_name),
    parent_(parent),
    alpha_(CELL_CONFIG[idx].attr_alpha_name),
    zero_(CELL_CONFIG[idx].attr_zero_name),
    tara_(CELL_CONFIG[idx].attr_tara_name),
    gram_(CELL_CONFIG[idx].attr_gram_name),
    idx_(idx) {

    gpio_pin_as_output(&CELL_CONFIG[idx].cs_gpio_cfg,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CELL_CONFIG[idx].cs_gpio_cfg);

    make_int32(0);
    alpha_.make_float(INIT_ALPHA[idx]);
    zero_.make_float(INIT_ZERO[idx]);
    tara_.make_uint32(INIT_TARA[idx]);
    gram_.make_float(0);
}

void LoadSensorPort::Init() {
    parent_->RegisterPortInterface(CELL_CONFIG[idx_].attr_value_name,
                                    static_cast<SensorInterface *>(this));

    parent_->RegisterAttribute(static_cast<FwAttribute *>(this));
    parent_->RegisterAttribute(&gram_);
    parent_->RegisterAttribute(&alpha_);
    parent_->RegisterAttribute(&zero_);
    parent_->RegisterAttribute(&tara_);
}

void LoadSensorPort::setSensorValue(uint32_t val) {
    make_uint32(val);

    int32_t t1 = static_cast<int32_t>(val);
    if (t1 & 0x04000000) {
        t1 |= 0xf8000000;
    }
    float phys = static_cast<float>(t1) * alpha_.to_float();
    gram_.make_float(phys + zero_.to_float() + tara_.to_float());
}
