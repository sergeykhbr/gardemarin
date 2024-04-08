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

#pragma once

#include <inttypes.h>

typedef struct adc_cr_bits_type {
    volatile uint32_t AWD : 1;    // [0] Analog watchdog: 0=no; 1=occured
    volatile uint32_t EOC : 1;    // [1] Regular channel end of conversion: 1=complete
    volatile uint32_t JEOC : 1;   // [2] Injected channel end of conversion: 1=complete
    volatile uint32_t JSTRT : 1;  // [3] Injected channel start flag: 1=has started
    volatile uint32_t STRT : 1;   // [4] Regular channel start flag: 1=has started
    volatile uint32_t OVR : 1;    // [5] Overrun has occured
} adc_cr_bits_type;

typedef union adc_cr_reg_type {
    adc_cr_bits_type bits;
    volatile uint32_t val;
} adc_cr_reg_type;

/** 
  * @brief Analog to Digital Converter  
  */

typedef struct ADC_registers_type
{
    volatile uint32_t SR;     /*!< ADC status register,                         Address offset: 0x00 */
    volatile uint32_t CR1;    /*!< ADC control register 1,                      Address offset: 0x04 */      
    volatile uint32_t CR2;    /*!< ADC control register 2,                      Address offset: 0x08 */
    volatile uint32_t SMPR1;  /*!< ADC sample time register 1,                  Address offset: 0x0C */
    volatile uint32_t SMPR2;  /*!< ADC sample time register 2,                  Address offset: 0x10 */
    volatile uint32_t JOFR1;  /*!< ADC injected channel data offset register 1, Address offset: 0x14 */
    volatile uint32_t JOFR2;  /*!< ADC injected channel data offset register 2, Address offset: 0x18 */
    volatile uint32_t JOFR3;  /*!< ADC injected channel data offset register 3, Address offset: 0x1C */
    volatile uint32_t JOFR4;  /*!< ADC injected channel data offset register 4, Address offset: 0x20 */
    volatile uint32_t HTR;    /*!< ADC watchdog higher threshold register,      Address offset: 0x24 */
    volatile uint32_t LTR;    /*!< ADC watchdog lower threshold register,       Address offset: 0x28 */
    volatile uint32_t SQR1;   /*!< ADC regular sequence register 1,             Address offset: 0x2C */
    volatile uint32_t SQR2;   /*!< ADC regular sequence register 2,             Address offset: 0x30 */
    volatile uint32_t SQR3;   /*!< ADC regular sequence register 3,             Address offset: 0x34 */
    volatile uint32_t JSQR;   /*!< ADC injected sequence register,              Address offset: 0x38*/
    volatile uint32_t JDR1;   /*!< ADC injected data register 1,                Address offset: 0x3C */
    volatile uint32_t JDR2;   /*!< ADC injected data register 2,                Address offset: 0x40 */
    volatile uint32_t JDR3;   /*!< ADC injected data register 3,                Address offset: 0x44 */
    volatile uint32_t JDR4;   /*!< ADC injected data register 4,                Address offset: 0x48 */
    volatile uint32_t DR;     /*!< ADC regular data register,                   Address offset: 0x4C */
} ADC_registers_type;

typedef struct ADC_common_registers_type
{
    volatile uint32_t CSR;    /*!< ADC Common status register,                  Address offset: ADC1 base address + 0x300 */
    volatile uint32_t CCR;    /*!< ADC common control register,                 Address offset: ADC1 base address + 0x304 */
    volatile uint32_t CDR;    /*!< ADC common regular data register for dual
                             AND triple modes,                            Address offset: ADC1 base address + 0x308 */
} ADC_common_registers_type;
