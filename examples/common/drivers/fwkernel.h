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

#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include "relais.h"
#include "ledstrip.h"
#include "can_drv.h"
#include "dbc.h"

class KernelClass : public FwObject {
 public:
    explicit KernelClass(const char *name);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

 private:
    /** @brief Kernel Version attribute */
    FwAttribute version_;

    RelaisDriver relais0_;
    RelaisDriver relais1_;
    LedStripDriver ledrbw_;
    CanDriver can1_;
    CanDriver can2_;
    DbcConverter dbc_;        // CAN database converter
};
