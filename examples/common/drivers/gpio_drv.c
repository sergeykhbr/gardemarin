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
#include <mcu.h>
#include "gpio_drv.h"

void gpio_pin_as_input(const gpio_pin_type *p,
                        uint32_t speed,
                        uint32_t pushpull) {
    uint32_t t1;

#if __F103x
    // MODE[1:0]
    //      00 Input mode
    //      01 Output mode, max speed 10 MHz
    //      10 Output mode, max speed 2 MHz
    //      11 Output mode, max speed 50 MHz
    // CNF[3:2] Input mode (MODE[1:0] == 00)
    //       00 Analog mode
    //       01 Floating input (reset state)
    //       10 Input with pull-up/pull-down
    //       11 reserved
    // CNF[3:2] Output mode (MODE[1:0] != 00)
    //       00 General purpose output push-pull
    //       01 general puspose output Open-drain
    //       10 Alternate function output push-pull
    //       11 Alternate function output open-drain
    t1 = read32(&p->port->CR[p->pinidx >> 3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    t1 |= (0x8 << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);

#else
    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&p->port->MODER);
    t1 &= ~(0x3 << 2*p->pinidx);
    write32(&p->port->MODER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&p->port->OSPEEDR);
    t1 &= ~(0x3 << 2*p->pinidx);
    t1 |= speed << 2*p->pinidx;
    write32(&p->port->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&p->port->PUPDR);
    t1 &= ~(0x3 << 2*p->pinidx);
    t1 |= pushpull << 2*p->pinidx;
    write32(&p->port->PUPDR, t1);
#endif
}

void gpio_pin_as_output(const gpio_pin_type *p,
                        uint32_t odrain,
                        uint32_t speed,
                        uint32_t pushpull) {
    uint32_t t1;

#if __F103x
    // MODE[1:0]
    //      00 Input mode
    //      01 Output mode, max speed 10 MHz
    //      10 Output mode, max speed 2 MHz
    //      11 Output mode, max speed 50 MHz
    // CNF[3:2] Input mode (MODE[1:0] == 00)
    //       00 Analog mode
    //       01 Floating input (reset state)
    //       10 Input with pull-up/pull-down
    //       11 reserved
    // CNF[3:2] Output mode (MODE[1:0] != 00)
    //       00 General purpose output push-pull
    //       01 general puspose output Open-drain
    //       10 Alternate function output push-pull
    //       11 Alternate function output open-drain
    uint32_t mode = 0x1;
    if (speed == GPIO_VERY_FAST) {
        mode = 0x3;   // 50 MHz
    } else if (speed == GPIO_SLOW) {
        mode = 0x2;    // 2 MHz
    }
    if (odrain == GPIO_OPEN_DRAIN) {
        mode |= 0x4;
    }
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    t1 |= (mode << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);

#else
    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&p->port->MODER);
    t1 &= ~(0x3 << 2*p->pinidx);
    t1 |= (0x1 << 2*p->pinidx);
    write32(&p->port->MODER, t1);

    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    t1 = read32(&p->port->OTYPER);
    t1 &= ~(1 << p->pinidx);
    t1 |= odrain << p->pinidx;
    write32(&p->port->OTYPER, t1);

    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = read32(&p->port->OSPEEDR);
    t1 &= ~(0x3 << 2*p->pinidx);
    t1 |= speed << 2*p->pinidx;
    write32(&p->port->OSPEEDR, t1);

    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down; 11=reserved
    t1 = read32(&p->port->PUPDR);
    t1 &= ~(0x3 << 2*p->pinidx);
    t1 |= pushpull << 2*p->pinidx;
    write32(&p->port->PUPDR, t1);
#endif
}

void gpio_pin_as_alternate(const gpio_pin_type *p,
                           uint32_t ADx) {

    uint32_t t1;

#if __F103x
    // MODE[1:0]
    //      00 Input mode
    //      01 Output mode, max speed 10 MHz
    //      10 Output mode, max speed 2 MHz
    //      11 Output mode, max speed 50 MHz
    // CNF[3:2] Input mode (MODE[1:0] == 00)
    //       00 Analog mode
    //       01 Floating input (reset state)
    //       10 Input with pull-up/pull-down
    //       11 reserved
    // CNF[3:2] Output mode (MODE[1:0] != 00)
    //       00 General purpose output push-pull
    //       01 general puspose output Open-drain
    //       10 Alternate function output push-pull
    //       11 Alternate function output open-drain
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    t1 |= (0xB << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);

#else
    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&p->port->MODER);
    t1 &= ~(0x3 << 2*p->pinidx);
    t1 |= (0x2 << 2*p->pinidx);
    write32(&p->port->MODER, t1);

    t1 = read32(&p->port->AFR[p->pinidx >> 3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    t1 |= (ADx << ((p->pinidx & 0x7) * 4));
    write32(&p->port->AFR[p->pinidx >> 3], t1);
#endif
}

void gpio_pin_as_analog(const gpio_pin_type *p) {
    uint32_t t1;

#if __F103x
    // MODE[1:0]
    //      00 Input mode
    //      01 Output mode, max speed 10 MHz
    //      10 Output mode, max speed 2 MHz
    //      11 Output mode, max speed 50 MHz
    // CNF[3:2] Input mode (MODE[1:0] == 00)
    //       00 Analog mode
    //       01 Floating input (reset state)
    //       10 Input with pull-up/pull-down
    //       11 reserved
    // CNF[3:2] Output mode (MODE[1:0] != 00)
    //       00 General purpose output push-pull
    //       01 general puspose output Open-drain
    //       10 Alternate function output push-pull
    //       11 Alternate function output open-drain
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);
#else
    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&p->port->MODER);
    t1 |= (0x3 << 2*p->pinidx);
    write32(&p->port->MODER, t1);
#endif
}

void gpio_pin_set(const gpio_pin_type *p) {
    write16(&p->port->BSRRL, (1 << p->pinidx));
}

void gpio_pin_clear(const gpio_pin_type *p) {
#if __F103x
    write32(&p->port->BRR, (1 << p->pinidx));
#else
    write16(&p->port->BSRRH, (1 << p->pinidx));
#endif
}

uint32_t gpio_pin_get(const gpio_pin_type *p) {
    return (read32(&p->port->IDR) >> p->pinidx) & 0x1;
}
