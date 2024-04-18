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
    const char *attr_gram_name;    
    const char *attr_offset_name;    
    const char *attr_alpha_name;
};

static const LoadCellCfgType CELL_CONFIG[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    {"scale0", {(GPIO_registers_type *)GPIOD_BASE, 2}, "value0", "gram0", "offset0", "alpha0"},
    {"scale1", {(GPIO_registers_type *)GPIOD_BASE, 3}, "value1", "gram1", "offset1", "alpha1"},
    {"scale2", {(GPIO_registers_type *)GPIOD_BASE, 4}, "value2", "gram2", "offset2", "alpha2"},
    {"scale3", {(GPIO_registers_type *)GPIOD_BASE, 7}, "value3", "gram3", "offset3", "alpha3"}
};

// PC[12] = SPI3_MOSI  AF6 -> AF0 output (unused by HX711)
// PC[11] = SPI3_MISO  AF6 -> AF0 input
// PC[10] = SPI3_SCK   AF6 -> AF0 output
static const gpio_pin_type SPI3_MOSI = {(GPIO_registers_type *)GPIOC_BASE, 12};
static const gpio_pin_type SPI3_MISO = {(GPIO_registers_type *)GPIOC_BASE, 11};
static const gpio_pin_type SPI3_SCK = {(GPIO_registers_type *)GPIOC_BASE, 10};

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
    parent_(parent),
    portname_(CELL_CONFIG[idx].portname),
    value_(CELL_CONFIG[idx].attr_value_name),
    gram_(CELL_CONFIG[idx].attr_gram_name),
    offset_(CELL_CONFIG[idx].attr_offset_name),
    alpha_(CELL_CONFIG[idx].attr_alpha_name),
    idx_(idx) {

    gpio_pin_as_output(&CELL_CONFIG[idx].cs_gpio_cfg,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CELL_CONFIG[idx].cs_gpio_cfg);

    value_.make_uint32(0);
    gram_.make_float(0);
    offset_.make_uint32(0);
    alpha_.make_float(1.0);
}

void LoadSensorPort::Init() {
    parent_->RegisterPortInterface(portname_, static_cast<SensorInterface *>(this));

    parent_->RegisterAttribute(&value_);
    parent_->RegisterAttribute(&gram_);
    parent_->RegisterAttribute(&offset_);
    parent_->RegisterAttribute(&alpha_);
}

void LoadSensorPort::setSensorValue(uint32_t val) {
    value_.make_uint32(val);
    float phys = static_cast<float>(
        val - offset_.to_uint32()) * alpha_.to_float();
    gram_.make_float(phys);
}
