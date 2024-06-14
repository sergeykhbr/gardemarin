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

class UserSettings : public FwObject {
 public:
    explicit UserSettings(const char *name);

    // FwObject interface:
    virtual void Init() override;

 protected:

 protected:
    FwAttribute lastServiceTime_;
    FwAttribute requestToService_;
    FwAttribute wateringPerDrain_;
    FwAttribute wateringInterval_;
    FwAttribute wateringDuration_;
    FwAttribute lastWatering_;
    FwAttribute oxygenSaturationInterval_;
    FwAttribute dayStart_;
    FwAttribute dayEnd_;
    FwAttribute dayDuty0_;
    FwAttribute dayDuty1_;
    FwAttribute dayDuty2_;
    FwAttribute dayDuty3_;
};
