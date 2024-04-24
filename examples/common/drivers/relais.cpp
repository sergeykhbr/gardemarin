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
#include <gardemarin.h>
#include <stm32f4xx_map.h>
#include "relais.h"

//    PD13 - relais 0
//    PD14 - relais 1
static const gpio_pin_type gpio_cfg[GARDEMARIN_RELAIS_TOTAL] = {
    {(GPIO_registers_type *)GPIOD_BASE, 13},
    {(GPIO_registers_type *)GPIOD_BASE, 14}
};


RelaisDriver::RelaisDriver(const char *name, int instidx) : FwObject(name),
    idx_(instidx),
    state_(this, "State") {
    state_.make_int16(0);

    gpio_pin_as_output(&gpio_cfg[idx_],
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
    gpio_pin_set(&gpio_cfg[idx_]);
}

void RelaisDriver::setBinDisabled() {
    gpio_pin_clear(&gpio_cfg[idx_]);
}

uint8_t RelaisDriver::getBinState() {
    return static_cast<uint8_t>(gpio_pin_get(&gpio_cfg[idx_]));
}
