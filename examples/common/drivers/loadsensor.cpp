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

#define USE_TIM5

// Chip select pins per channel
struct LoadCellCfgType {
    const char *portname;        // all names should be placed into flash, do not use SRAM for that
    gpio_pin_type cs_gpio_cfg;
    const char *attr_value_name;
    const char *attr_alpha_name;
    const char *attr_zero_name;
    const char *attr_tara_name;    
    const char *attr_gram_name;
    const char *attr_gramflt_name;
};

static const LoadCellCfgType CELL_CONFIG[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    {"scale0", {(GPIO_registers_type *)GPIOD_BASE, 2}, "value0", "alpha0", "zero0", "tara0", "gram0", "gram0flt"},
    {"scale1", {(GPIO_registers_type *)GPIOD_BASE, 3}, "value1", "alpha1", "zero1", "tara1", "gram1", "gram1flt"},
    {"scale2", {(GPIO_registers_type *)GPIOD_BASE, 4}, "value2", "alpha2", "zero2", "tara2", "gram2", "gram2flt"},
    {"scale3", {(GPIO_registers_type *)GPIOD_BASE, 7}, "value3", "alpha3", "zero3", "tara3", "gram3", "gram3flt"}
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
// 2 mix       y = 404.58*x + 1E+06
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
    -402.6f,
    -4989.0f,
    113.0f,
    0.0f
};

static const float INIT_TARA[GARDEMARIN_LOAD_SENSORS_TOTAL] = {
    0.0f,
    0.0f,
    422.0f,
    0.0f
};

static IrqHandlerInterface *drivers_ = 0;

static void startCounter(TIM_registers_type *TIM, int usec) {
    tim_cr1_reg_type cr1;

    write32(&TIM->ARR, static_cast<uint32_t>(usec));
    write32(&TIM->CNT, static_cast<uint32_t>(usec));

    cr1.val = 0;
    cr1.bits.CEN = 1;
    cr1.bits.OPM = 1;   // one pulse mode
    cr1.bits.DIR = 1;   // downcount
    write32(&TIM->CR1.val, cr1.val);
}

extern "C" void TIM5_irq_handler() {
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;

    int nsec = 0;
    if (drivers_) {
        drivers_->handleInterrupt(&nsec);
    }
    // Convert nsec to timescale is 100 ns
    /*if (nsec) {
        nsec /= 100;
        if (nsec == 0) {
            nsec = 1;       // minimum 100 nsec interval
        }
    }*/

    write16(&TIM5->SR, 0);  // clear all pending bits
    nvic_irq_clear(50);
    /*if (nsec) {
        startCounter(TIM5, nsec);
    }*/
}


LoadSensorDriver::LoadSensorDriver(const char *name) : FwObject(name) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;

    estate_ = Idle;
    bitCnt_ = 0;
    drivers_ = static_cast<IrqHandlerInterface *>(this);

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

    // adc clock on APB1 = 144/4 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 3);             // APB1[3] TIM5EN
    write32(&RCC->APB1ENR, t1);

    write32(&TIM5->CR1.val, 0);         // stop counter
    // time scale 100 nsec
    write16(&TIM5->PSC, system_clock_hz() / 2 / 10000000 - 1);             // prescaler = 1: CK_CNT = (F_ck_psc/(PSC+1))
    write16(&TIM5->DIER, 1);            // [0] UIE - update interrupt enabled

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(50, 5);
}

void LoadSensorDriver::Init() {
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterInterface(static_cast<RunInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));

    for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
        chn_[i].port->Init();
    }
}

void LoadSensorDriver::handleInterrupt(int *argv) {
    uint8_t ready = 0;
    *argv = 0;
    switch (estate_) {
    case Idle:
        return;
    case Conversion:
        gpio_pin_clear(&SPI3_SCK);
        estate_ = WaitReady;
        *argv = 200;
        break;
    case WaitReady:
        for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
            selectChannel(i);
            chn_[i].shifter = 0;
            if (gpio_pin_get(&SPI3_MISO) == 0) {
                ready |= 1 << i;
            }
        }
        selectChannel(-1);    // deselect all
        if (ready == ((1 << GARDEMARIN_LOAD_SENSORS_TOTAL) - 1)) {
            estate_ = SCK_HIGH;
            bitCnt_ = 0;
        }
        *argv = 10;
        break;
    case SCK_HIGH:
        gpio_pin_set(&SPI3_SCK);
        estate_ = SCK_LOW;
        *argv = 1000;   // 0.2 us; typical 1 us; max 50 us
        break;
    case SCK_LOW:
        gpio_pin_clear(&SPI3_SCK);
        estate_ = Reading;
        *argv = 1000;   // 0.2 us; typical 1 us
        break;
    case Reading:
        for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
            selectChannel(i);
            chn_[i].shifter <<= 1;
            chn_[i].shifter |= gpio_pin_get(&SPI3_MISO);
        }
        selectChannel(-1);    // deselect all

        if (++bitCnt_ >= 27) {
            estate_ = Sleep;
            for (int i = 0; i < GARDEMARIN_LOAD_SENSORS_TOTAL; i++) {
                chn_[i].port->setSensorValue(chn_[i].shifter);
            }
        } else {
            estate_ = SCK_HIGH;
        }
        *argv = 10;
        break;
    case Sleep:
        //gpio_pin_set(&SPI3_SCK);
        //*argv = 60000;
        estate_ = Idle;
        break;
    default:
        estate_ = Idle;
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
#ifdef USE_TIM5
    if (estate_ == Idle) {
        estate_ = Conversion;
        startCounter((TIM_registers_type *)TIM5_BASE, 10);
    }
#else
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
#endif
}


LoadSensorPort::LoadSensorPort(FwObject *parent, int idx) : 
    FwAttribute(CELL_CONFIG[idx].attr_value_name),
    parent_(parent),
    alpha_(CELL_CONFIG[idx].attr_alpha_name, "linear calibration rate"),
    zero_(CELL_CONFIG[idx].attr_zero_name, "zero level in gram"),
    tara_(CELL_CONFIG[idx].attr_tara_name, "tara weight in gram"),
    gram_(CELL_CONFIG[idx].attr_gram_name, "value in gram"),
    gramflt_(CELL_CONFIG[idx].attr_gramflt_name, "[g], 1/10 filtration"),
    idx_(idx),
    fltacc_(0) {

    gpio_pin_as_output(&CELL_CONFIG[idx].cs_gpio_cfg,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&CELL_CONFIG[idx].cs_gpio_cfg);

    make_int32(0);
    alpha_.make_float(INIT_ALPHA[idx]);
    zero_.make_float(INIT_ZERO[idx]);
    tara_.make_float(INIT_TARA[idx]);
    gram_.make_float(0);
    gramflt_.make_float(0);
    v3of4[0] = 0;
    v3of4[1] = 0;
    v3of4[2] = 0;
}

void LoadSensorPort::Init() {
    parent_->RegisterPortInterface(CELL_CONFIG[idx_].attr_value_name,
                                    static_cast<SensorInterface *>(this));

    parent_->RegisterAttribute(static_cast<FwAttribute *>(this));
    parent_->RegisterAttribute(&gramflt_);
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
    phys += zero_.to_float() + tara_.to_float();

    float dltphys;
    int validcnt = 0;
    for (int i = 0; i < 3; i++) {
        dltphys = phys - v3of4[i];
        if (dltphys < 0) {
            dltphys = -dltphys;
        }

        // +/- 100 grams 3 of 4 measurements to use as valid
        if (dltphys < 100.0) {
            validcnt++;
        }
    }
    if (validcnt >= 2) {
        gram_.make_float(phys);

        fltacc_ -= 0.1f * fltacc_;
        fltacc_ += phys;
        gramflt_.make_float(0.1f * fltacc_);
    }

    v3of4[2] = v3of4[1];
    v3of4[1] = v3of4[0];
    v3of4[0] = phys;
}
