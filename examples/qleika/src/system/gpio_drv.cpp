/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include <mcu.h>
#include "gpio_drv.h"

void gpio_pin_as_input(const gpio_pin_type *p,
                        uint32_t speed,
                        uint32_t pushpull) {
    uint32_t t1;
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    // [3:2] = 00 General purpose output push-pull
    // [1:0] = 00 Input mode, max speed 10 MHz
    t1 |= (0x0 << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);
}

void gpio_pin_as_output(const gpio_pin_type *p,
                        uint32_t odrain,
                        uint32_t speed,
                        uint32_t pushpull) {
    uint32_t t1;
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    // [3:2] = 00 General purpose output push-pull
    // [1:0] = 01 Output mode, max speed 10 MHz
    t1 |= (0x1 << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);
}


void gpio_pin_as_alternate(const gpio_pin_type *p,
                           uint32_t ADx) {

    uint32_t t1;
    t1 = read32(&p->port->CR[p->pinidx>>3]);
    t1 &= ~(0xF << ((p->pinidx & 0x7) * 4));
    // [3:2] = 10 Alternate push-pull
    // [1:0] = 00  (alternate could be enabled only when = 00)
    t1 |= (0x8 << ((p->pinidx & 0x7) * 4));
    write32(&p->port->CR[p->pinidx >> 3], t1);
}

void gpio_pin_set(const gpio_pin_type *p) {
    write16(&p->port->BSRRL, (1 << p->pinidx));
}

void gpio_pin_clear(const gpio_pin_type *p) {
    write32(&p->port->BRR, (1 << p->pinidx));
}
