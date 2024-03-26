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
#include "loadsensor.h"

void load_sensor_init(load_sensor_type *data) {
    GPIO_registers_type *P = data->csn_port;
    uint32_t t1;
    // CSn chip as output. HIGH is set MISO to Z state

    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&P->MODER);
    t1 &= ~(0x3 << (2*data->csn_bitidx));
    t1 |= (0x1 << (2*data->csn_bitidx));
    write32(&P->MODER, t1);

    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    t1 = read32(&P->OTYPER);
    t1 &= ~(0x1 << data->csn_bitidx);
    write32(&P->OTYPER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&P->OSPEEDR);
    t1 &= ~(0x3 << 2*data->csn_bitidx);
    write32(&P->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&P->PUPDR);
    t1 = (0x3 << (2*data->csn_bitidx));
    write32(&P->PUPDR, t1);

    // [15:0] ODRy: port output data
    t1 = read32(&P->ODR);
    t1 |= (1 << data->csn_bitidx);  // MISO to z-state
    write32(&P->ODR, t1);


    // SPI3 controller is not used because of 25-bits transition CH.A, Gain 128
    // PORTC:
    //    PC12 SPI3_MOSI  AF6 -> AF0 output (unused by HX711)
    //    PC11 SPI3_MISO  AF6 -> AF0 input
    //    PC10 SPI3_SCK   AF6 -> AF0 output
    P = (GPIO_registers_type *)GPIOC_BASE;

    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&P->MODER);
    t1 &= ~(0x3C << (2*10));
    t1 |= (0x11 << (2*10));
    write32(&P->MODER, t1);

    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    t1 = read32(&P->OTYPER);
    t1 &= ~(0x7 << 10);
    write32(&P->OTYPER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&P->OSPEEDR);
    t1 &= ~(0x3C << 2*10);
    t1 |= (0x15 << 2*10);
    write32(&P->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&P->PUPDR);
    t1 = (0x3C << (2*10));
    write32(&P->PUPDR, t1);

    // [15:0] ODRy: port output data
    t1 = read32(&P->ODR);
    t1 &= ~(0x7 << 10); 
    write32(&P->ODR, t1);
}

// MISO is LOW when data is ready (HX711 DOUT pin)
int load_sensor_is_ready(load_sensor_type *data) {
    GPIO_registers_type *P = data->csn_port;
    uint32_t t1;
    uint32_t ready = 0;

    // [15:0] ODRy: port output data
    t1 = read32(&P->ODR);
    t1 &= ~(1 << data->csn_bitidx);  // CSn -> LOW to read MISO
    write32(&P->ODR, t1);

    t1 = read32(&P->IDR);
    if ((t1 & (1 << 11)) == 0) {
        ready = 1;
    }

    t1 = read32(&P->ODR);
    t1 |= (1 << data->csn_bitidx);  // CSn -> HIGH to disable MISO
    write32(&P->ODR, t1);

    return ready;
}
