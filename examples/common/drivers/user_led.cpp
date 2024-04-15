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
#include "user_led.h"

//    PE2 - User LED. 0=LED is on (inversed)
static const gpio_pin_type USER_LED0 = {(GPIO_registers_type *)GPIOE_BASE, 2};

UserLedDriver::UserLedDriver(const char *name) : FwObject(name),
    state_("state") {

    gpio_pin_as_output(&USER_LED0,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);

    setBinEnabled();
}

void UserLedDriver::Init() {
    RegisterInterface(static_cast<BinInterface *>(this));

    RegisterAttribute(&state_);
}

void UserLedDriver::setBinEnabled() {
    // inversed
    gpio_pin_clear(&USER_LED0);
    state_.make_uint32(1);
}

void UserLedDriver::setBinDisabled() {
    // inversed
    gpio_pin_set(&USER_LED0);
    state_.make_uint32(0);
}
