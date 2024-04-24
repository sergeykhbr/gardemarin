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

// ADC_SR: status register
typedef struct adc_sr_bits_type {
    volatile uint32_t AWD : 1;    // [0] rc_w0 Analog watchdog: 0=no; 1=occured
    volatile uint32_t EOC : 1;    // [1] rc_w0 Regular channel end of conversion: 1=complete. cleared by SW or by reading ADC_DR
    volatile uint32_t JEOC : 1;   // [2] rc_w0 Injected channel end of conversion: 1=complete
    volatile uint32_t JSTRT : 1;  // [3] rc_w0 Injected channel start flag: 1=has started
    volatile uint32_t STRT : 1;   // [4] rc_w0 Regular channel start flag: 1=has started
    volatile uint32_t OVR : 1;    // [5] rc_w0 Overrun has occured
} adc_sr_bits_type;

typedef union adc_sr_reg_type {
    adc_sr_bits_type bits;
    volatile uint32_t val;
} adc_sr_reg_type;


// Control register 1
typedef struct adc_cr1_bits_type {
    volatile uint32_t AWDCH : 5;    // [4:0] rw Analog watchdog channel select bits
    volatile uint32_t EOCIE : 1;    // [5] rw Interrup enable for EOC
    volatile uint32_t AWDIE : 1;    // [6] rw Analog watchdog interrupt enable
    volatile uint32_t JEOCIE : 1;   // [7] rw Interrup enable for injected channels
    volatile uint32_t SCAN : 1;     // [8] rw Scan mode: 1=enable
    volatile uint32_t AWDSGL : 1;   // [9] rw Enable analog watchdog on a single channel in scan mode (AWDCH[4:0])
    volatile uint32_t JAUTO : 1;    // [10] rw Automatic injected group conversion: 1=enable
    volatile uint32_t DISCEN : 1;   // [11] rw Discountinuous mode on regular channel: 1=enable
    volatile uint32_t JDISCEN : 1;  // [12] rw Discountinuous mode on injected channesl: 1=enable
    volatile uint32_t DISCNUN : 3;  // [15:13] rw Discountinuous mode channel count: 000=1 channel; 001=2;...;111=8 channels
    uint32_t rsrv21_16 : 6;         // [21:16]
    volatile uint32_t JAWDEN : 1;   // [22] rw Analog watchdog on rejected channel: 1=enable
    volatile uint32_t AWDEN : 1;    // [23] rw Analog watchdog on regular channel: 1=enable
    volatile uint32_t RES : 2;      // [25:24] rw Resolution: 00=12-bit, 01=10-bit, 10=8-bit, 11=6-bit
    volatile uint32_t OVRIE : 1;    // [26] rw Overrun irq: 1=enable
    uint32_t rsrv31_27 : 5;         // [31:27]
} adc_cr1_bits_type;

typedef union adc_cr1_reg_type {
    adc_cr1_bits_type bits;
    volatile uint32_t val;
} adc_cr1_reg_type;


// Control register 2
typedef struct adc_cr2_bits_type {
    volatile uint32_t ADON : 1;     // [0] rw A/D converter ON/OFF: 0=go to power done mode; 1=enabled
    volatile uint32_t CONT : 1;     // [1] rw Continuous conversion: 1=enabled
    uint32_t rsrv7_2 : 6;           // [7:2]
    volatile uint32_t DMA : 1;      // [8] rw Direct memory access mode: 0=DMA disabled; 1=DMA enabled
    volatile uint32_t DDS : 1;      // [9] rw DMA disable selection (for single ADC mode)
    volatile uint32_t EOCS : 1;     // [10] rw End of conversion: 0=EOC is set at the end of each sequence of regular conversion. Ovr is enabled only if DMA=1; 1=Overrun enabled
    volatile uint32_t ALIGN : 1;    // [11] rw Data alignment: 0=right; 1=left alignment
    uint32_t rsrv15_12 : 4;         // [15:12]
    volatile uint32_t JEXTSEL : 4;  // [19:16] rw External event select for injected group: 0000=Timer 1 CC4 event; ...
    volatile uint32_t JEXTEN : 2;   // [21:20] rw External trigger enable for injected channels
    volatile uint32_t JSWSTART : 1; // [22] rw Start conversion of injected channels: 0=reset state
    uint32_t rsrv23 : 1;            // [23]
    volatile uint32_t EXTSEL : 4;   // [27:24] rw External event select for regular group: 0000=Timer 1 CC1 event; ...
    volatile uint32_t EXTEN : 2;    // [29:28] rw External trigger enable for regular channels: 00=trigger disabled; 01=risinge edge; 10=falling edge; 11=both edges
    volatile uint32_t SWSTART : 1;  // [30] rw Start conversion of regular channels: 0=reset state; 1=start if ADON=1
    uint32_t rsrv31 : 1;            // [31]
} adc_cr2_bits_type;

typedef union adc_cr2_reg_type {
    adc_cr2_bits_type bits;
    volatile uint32_t val;
} adc_cr2_reg_type;


/** 
  * @brief Analog to Digital Converter  
  */

typedef struct ADC_registers_type
{
    adc_sr_reg_type SR;     /*!< ADC status register,                         Address offset: 0x00 */
    adc_cr1_reg_type CR1;    /*!< ADC control register 1,                      Address offset: 0x04 */      
    adc_cr2_reg_type CR2;    /*!< ADC control register 2,                      Address offset: 0x08 */
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
