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
#include "gpio_drv.h"

void gpio_pin_as_output(gpio_pin_type *p,
                        uint32_t odrain,
                        uint32_t speed,
                        uint32_t pushpull) {
    uint32_t t1;

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
}

void gpio_pin_set(gpio_pin_type *p) {
    write16(&p->port->BSRRL, (1 << p->pinidx));
}

void gpio_pin_clear(gpio_pin_type *p) {
    write16(&p->port->BSRRH, (1 << p->pinidx));
}
