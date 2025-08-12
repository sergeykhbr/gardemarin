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

#include <fwkernelgen.h>
#include "relais.h"
#include "ledstrip.h"
#include "can_drv.h"
#include "loadsensor.h"
#include "user_led.h"
#include "user_btn.h"
#include "adc_drv.h"
#include "hbridge.h"
#include "ds18b20_drv.h"
#include "soil_drv.h"
#include "usrsettings.h"

class AppKernelClass : public KernelClassGeneric {
 public:
    explicit AppKernelClass(const char *name);

    // FwObject interface:
    //virtual void PostInit() override;

 private:
    /** @brief Kernel Version attribute */
    RelaisDriver relais0_;
    RelaisDriver relais1_;
    LedStripDriver ledrbw_;
    CanDriver can1_;
    CanDriver can2_;
    LoadSensorDriver scales_;
    UserLedDriver uled0_;
    UserButtonDriver ubtn0_;
    AdcDriver adc1_;
    HBridgeDriver hbrg0_;
    HBridgeDriver hbrg1_;
    HBridgeDriver hbrg2_;
    HBridgeDriver hbrg3_;
    Ds18b20Driver temp0_;
    SoilDriver soil0_;
    UserSettings settings_;
};
