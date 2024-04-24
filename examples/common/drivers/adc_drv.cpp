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
#include <stdio.h>
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include "adc_drv.h"

// 0  ADC1_IN0 = hdrg0 dc0
// 1  ADC1_IN3 = hdrg0 dc1
// 2  ADC1_IN4 = hdrg1 dc0
// 3  ADC1_IN5 = hdrg1 dc1
// 4  ADC1_IN6 = hdrg2 dc0
// 5  ADC1_IN8 = hdrg2 dc1
// 6  ADC1_IN9 = hdrg3 dc0
// 7  ADC1_IN10 = hdrg3 dc1
// 8  ADC1_IN16 = temperature sensor
// 9  ADC1_IN17 = V_REFINT
// 10 ADC1_IN18 = VBAT
static const gpio_pin_type GPIO_CFG[8] = {
    {(GPIO_registers_type *)GPIOA_BASE, 0},   // PA0 ADC123_IN0
    {(GPIO_registers_type *)GPIOA_BASE, 3},   // PA3 ADC123_IN3
    {(GPIO_registers_type *)GPIOA_BASE, 4},   // PA4 ADC12_IN4
    {(GPIO_registers_type *)GPIOA_BASE, 5},   // PA5 ADC12_IN5
    {(GPIO_registers_type *)GPIOA_BASE, 6},   // PA6 ADC12_IN6
    {(GPIO_registers_type *)GPIOB_BASE, 0},   // PB0 ADC12_IN8
    {(GPIO_registers_type *)GPIOB_BASE, 1},   // PB1 ADC12_IN9
    {(GPIO_registers_type *)GPIOC_BASE, 0}    // PC0 ADC123_IN10
};

static uint32_t chanData_[GARDEMARIN_ADC_CHANNEL_USED] = {0};
static int chanSelected_ = 0;

// TODO: DMA engine when ADC tested
extern "C" void ADC1_irq_handler() {
    ADC_registers_type *adc1 = (ADC_registers_type *)ADC1_BASE;
    adc_sr_reg_type sr;

    sr.val = read32(&adc1->SR.val);
    chanData_[chanSelected_] = read32(&adc1->DR);
    if (sr.bits.EOC || ++chanSelected_ > GARDEMARIN_ADC_CHANNEL_USED) {
        // End of sequence or channel overrun
        chanSelected_ = 0;
    }

    nvic_irq_clear(18);
}

AdcDriver::AdcDriver(const char *name) : FwObject(name),
    in0_(static_cast<FwObject *>(this), "in0", 0),
    in3_(static_cast<FwObject *>(this), "in3", 1),
    in4_(static_cast<FwObject *>(this), "in4", 2),
    in5_(static_cast<FwObject *>(this), "in5", 3),
    in6_(static_cast<FwObject *>(this), "in6", 4),
    in8_(static_cast<FwObject *>(this), "in8", 5),
    in9_(static_cast<FwObject *>(this), "in9", 6),
    in10_(static_cast<FwObject *>(this), "in10", 7),
    temp_(static_cast<FwObject *>(this), "temp", 8),
    vint_(static_cast<FwObject *>(this), "vint", 9),
    vbat_(static_cast<FwObject *>(this), "vbat", 10),
    temperature_(this, "temperature"),
    voltageRef_(this, "Vref"),
    voltageBattery_(this, "Vbat")
{
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    ADC_common_registers_type *adcx = (ADC_common_registers_type *)ADC_BASE;
    ADC_registers_type *adc1 = (ADC_registers_type *)ADC1_BASE;

    // adc clock on APB2 = 144/2 = 72 MHz
    uint32_t t1 = read32(&RCC->APB2ENR);
    t1 |= (1 << 8);             // APB2[8] ADC1
    write32(&RCC->APB2ENR, t1);

    // common ADC_CR
    t1 = (1 << 23)    // TSVREFE: temperature sensor
       | (1 << 22)    // VBATE: VBAT enable
       | (3 << 16)    // prescaler=8: PCLK/8 = 72/8 = 9 MHz
       | (0xF << 8);  // DELAY: between 2 sampling phases: 16=20*TADCCLK
    write32(&adcx->CCR, t1);

    adc_cr2_reg_type cr2;
    cr2.val = 0;
    cr2.bits.ADON = 1;   // enable ADC from power down mode
    write32(&adc1->CR2.val, cr2.val);

    // ADC sampling time: 000=3cycles; 001=15cycles;... 111=480cycles
    t1 = (1 << 3*0)  // SMP0
       | (1 << 3*1)
       | (1 << 3*2)
       | (1 << 3*3)
       | (1 << 3*4)
       | (1 << 3*5)
       | (1 << 3*6)
       | (1 << 3*7)
       | (1 << 3*8)
       | (1 << 3*9); // SMP9
    write32(&adc1->SMPR2, t1);

    t1 = (1 << 3*0)  // SMP10
       | (1 << 3*1)
       | (1 << 3*2)
       | (1 << 3*3)
       | (1 << 3*4)
       | (1 << 3*5)
       | (1 << 3*6)
       | (1 << 3*7)
       | (1 << 3*8); // SMP18
    write32(&adc1->SMPR1, t1);

    // Regular section: 11 tranformations total
    t1 = (0 << 5*0)    // SQ1 = ADC1_IN0
       | (3 << 5*1)    // SQ2 = ADC1_IN3
       | (4 << 5*2)    // SQ3 = ADC1_IN4
       | (5 << 5*3)    // SQ4 = ADC1_IN5
       | (6 << 5*4)    // SQ5 = ADC1_IN6
       | (8 << 5*5);   // SQ6 = ADC1_IN8
    write32(&adc1->SQR3, t1);

    t1 = (9 << 5*0)     // SQ7 = ADC1_IN9
       | (10 << 5*1)    // SQ8 = ADC1_IN10
       | (16 << 5*2)    // SQ9 = ADC1_IN16 temperature
       | (17 << 5*3)    // SQ10 = ADC1_IN17 V_REFINT
       | (18 << 5*4);   // SQ11 = ADC1_IN18 V_BAT
    write32(&adc1->SQR2, t1);

    t1 = GARDEMARIN_ADC_CHANNEL_USED << 20;      // sequence length
    write32(&adc1->SQR1, t1);

    for (int i = 0; i < 8; i++) {
        gpio_pin_as_analog(&GPIO_CFG[i]);
    }

    adc_cr1_reg_type cr1;
    cr1.val = 0;
    cr1.bits.EOCIE = 1;   // EOC irq enabled
    cr1.bits.SCAN = 1;    // scan mode enabe
    write32(&adc1->CR1.val, cr1.val);

    cr2.val = 0;
    cr2.bits.ADON = 1;
    cr2.bits.CONT = 1;   // continue conversion in SCAN_MODE
    cr2.bits.EOCS = 1;   // generate irq after each conversion in a sequence
    cr2.bits.SWSTART = 1;
    write32(&adc1->CR2.val, cr2.val);

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(18, 3);
}

void AdcDriver::Init() {
    RegisterAttribute(&temperature_);
    RegisterAttribute(&voltageRef_);
    RegisterAttribute(&voltageBattery_);

    in0_.Init();
    in3_.Init();
    in4_.Init();
    in5_.Init();
    in6_.Init();
    in8_.Init();
    in9_.Init();
    in10_.Init();
    temp_.Init();
    vint_.Init();
    vbat_.Init();
}

AdcChannel::AdcChannel(FwObject *parent, const char *name, int idx) :
    parent_(parent), name_(name), idx_(idx) {
    
}

void AdcChannel::Init() {
    static_cast<AdcDriver *>(parent_)->RegisterPortInterface(
                        name_, static_cast<SensorInterface *>(this));
}

uint32_t AdcChannel::getSensorValue() {
    return chanData_[idx_];
}
