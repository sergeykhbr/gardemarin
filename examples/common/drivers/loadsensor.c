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
#include <uart.h>
#include "loadsensor.h"

void load_sensor_select(int idx) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOD_BASE;
    uint32_t t1 = read32(&P->ODR);
    switch (idx) {
    case 0:
        t1 &= ~(1 << 2);
        break;
    case 1:
        t1 &= ~(1 << 3);
        break;
    case 2:
        t1 &= ~(1 << 4);
        break;
    case 3:
        t1 &= ~(1 << 7);
        break;
    default:;
    }
    write32(&P->ODR, t1);
}

void load_sensor_deselect_all() {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOD_BASE;
    // [15:0] ODRy: port output data
    uint32_t t1 = read32(&P->ODR);
    t1 |= 0x9C;  // MISO to z-state
    write32(&P->ODR, t1);
}

void load_sensor_set_sck(int val) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOC_BASE;
    uint32_t t1 = read32(&P->ODR);
    t1 &= ~(0x1 << 10); 
    t1 |= (val << 10);
    write32(&P->ODR, t1);
}

int load_sensor_get_miso() {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOC_BASE;
    return (read32(&P->IDR) >> 11) & 0x1;
}

// {PD[7], PD[4:2]} = CSn[3:0]. Mask 1 bit: 0x9C; Mask 2 bits: 0xC3F0
// PC[12] = SPI3_MOSI  AF6 -> AF0 output (unused by HX711)
// PC[11] = SPI3_MISO  AF6 -> AF0 input
// PC[10] = SPI3_SCK   AF6 -> AF0 output
void load_sensor_init(load_sensor_type *data) {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOD_BASE;
    uint32_t t1;
    // CSn chip as output. HIGH is set MISO to Z state

    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&P->MODER);
    t1 &= ~(0xC3F0);
    t1 |= 0x4150;           // {7,4,3,2} as output
    write32(&P->MODER, t1);

    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    t1 = read32(&P->OTYPER);
    t1 &= ~0x9C;
    write32(&P->OTYPER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&P->OSPEEDR);
    t1 &= ~0xC3F0;
    write32(&P->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&P->PUPDR);
    t1 &= ~0xC3F0;
    write32(&P->PUPDR, t1);

    load_sensor_deselect_all();


    // SPI3 controller is not used because of 25-bits transition CH.A, Gain 128
    // PORTC:
    //    PC12 SPI3_MOSI  AF6 -> AF0 output (unused by HX711)
    //    PC11 SPI3_MISO  AF6 -> AF0 input
    //    PC10 SPI3_SCK   AF6 -> AF0 output
    P = (GPIO_registers_type *)GPIOC_BASE;

    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&P->MODER);
    t1 &= ~(0x3F << (2*10));
    t1 |= (0x11 << (2*10));
    write32(&P->MODER, t1);

    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    t1 = read32(&P->OTYPER);
    t1 &= ~(0x7 << 10);
    write32(&P->OTYPER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&P->OSPEEDR);
    t1 &= ~(0x3F << 2*10);
    t1 |= (0x15 << 2*10);
    write32(&P->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&P->PUPDR);
    t1 &= ~(0x3F << (2*10));
    t1 |= (0x1 << (2*11));   // MISO pull-up
    write32(&P->PUPDR, t1);

    // [15:0] ODRy: port output data
    t1 = read32(&P->ODR);
    t1 &= ~(0x7 << 10); 
    write32(&P->ODR, t1);

    // Let's check what HX711 are connected to PC[11] MISO:
    data->hx711_not_found = 0;
    for (int i = 0; i < 4; i++) {
        load_sensor_select(i);

        // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
        t1 = read32(&P->PUPDR);
        t1 &= ~(0x3 << (2*11));
        t1 |= (0x1 << (2*11));   // pull-up
        write32(&P->PUPDR, t1);
        if (load_sensor_get_miso() == 1) {
            // pull-down
            t1 &= ~(0x3 << (2*11));
            t1 |= (0x2 << (2*11));   // pull-down
            write32(&P->PUPDR, t1);
            if (load_sensor_get_miso() == 0) {
                data->hx711_not_found |= 1 << i;
            }
        }
        // restore pull-up:
        t1 &= ~(0x3 << (2*11));
        t1 |= (0x1 << (2*11));   // pull-up
        write32(&P->PUPDR, t1);
        
        if (data->hx711_not_found & (1 << i)) {
            uart_printf("HX711[%d] not found\r\n", i);
        } else {
            uart_printf("HX711[%d] detected. MISO=%d\r\n",
                       i, load_sensor_get_miso());
        }

        load_sensor_deselect_all();
    }
}

void load_sensor_read(load_sensor_type *data) {
    data->ready = 0;
    for (int i = 0; i < 4; i++) {
        load_sensor_select(i);

        data->value[i] = 0;
        if (load_sensor_get_miso() == 0) {
            data->ready |= 1 << i;
        }
        load_sensor_deselect_all();
    }

    // 27 clocks to read DOUT with gain 64
    for (int i = 0; i < 4; i++) {
        data->value[i] = 0;
    }

    for (int i = 0; i < 27; i++) {
        load_sensor_set_sck(1);
        system_delay_ns(1000);   // 0.2 us; typical 1 us; max 50 us

        load_sensor_set_sck(0);
        system_delay_ns(1000);   // 0.2 us; typical 1 us

        for (int i = 0; i < 4; i++) {
            load_sensor_select(i);
            data->value[i] <<= 1;
            data->value[i] |= load_sensor_get_miso();
            load_sensor_deselect_all();
        }
    }


    for (int i = 0; i < 4; i++) {
        uart_printf("Scales[%d], rdy:%d, val:%08x\r\n", 
                    i,
                    (data->ready >> i) & 1,
                    data->value[i]);
    }
}

void load_sensor_sleep(load_sensor_type *sleep) {
    load_sensor_set_sck(1);
    system_delay_ns(60000);
}
