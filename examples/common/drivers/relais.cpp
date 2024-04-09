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
#include <fwapi.h>
#include <stm32f4xx_map.h>
#include "relais.h"

RelaisDriver::RelaisDriver(const char *name, int instidx) : FwObject(name),
    state_("State") {
    gpio_cfg_.port = (GPIO_registers_type *)GPIOD_BASE;

    //    PD13 - relais 0
    //    PD14 - relais 1
    if (instidx == 0) {
        gpio_cfg_.pinidx = 13;
    } else {
        gpio_cfg_.pinidx = 14;
    }
    state_.make_int16(0);

    gpio_pin_as_output(&gpio_cfg_,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);

    // for bullet proofness
    setBinDisabled();
}

void RelaisDriver::Init() {
    RegisterInterface(static_cast<BinInterface *>(this));
    RegisterAttribute(&state_);
}

void RelaisDriver::setBinEnabled() {
    gpio_pin_set(&gpio_cfg_);
    state_.make_int16(1);
}

void RelaisDriver::setBinDisabled() {
    gpio_pin_clear(&gpio_cfg_);
    state_.make_int16(0);
}
