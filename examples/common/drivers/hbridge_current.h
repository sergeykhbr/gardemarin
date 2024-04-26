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

#include <gardemarin.h>
#include <prjtypes.h>
#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <SensorInterface.h>


class SensorCurrent : public FwAttribute {
 public:
    SensorCurrent(FwObject *parent, const char *name, const char *adcport);

    // FwAttribute
    virtual void pre_read() override;

    // Common interface
    void Init();
    void PostInit();

 protected:
    int32_t getRawValue();

 protected:
    FwObject *parent_;
    const char *adcport_;
    SensorInterface *isensor_;
};
