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

#pragma once

#include <fwkernelgen.h>
#include "can_drv.h"
#include "can_injector.h"
#include "user_led.h"
#include "user_btn.h"
#include "display_spi.h"

class AppKernelClass : public KernelClassGeneric {
 public:
    explicit AppKernelClass(const char *name);

    // FwObject interface:
    //virtual void PostInit() override;

 private:
    /** @brief Kernel Version attribute */
    CanDriver can1_;
    CanDriver can2_;
    CanInjectorDriver injector0_;
    UserLedDriver uled0_;
    UserButtonDriver ubtn0_;
    DisplaySPI disp0_;
};
