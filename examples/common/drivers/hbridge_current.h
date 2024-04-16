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


class SensorCurrent : public SensorInterface {
 public:
    SensorCurrent(FwObject *parent, int idx);

    // Common interface
    void Init();

    // SensorInterface:
    virtual void setSensorValue(uint32_t val) override;
    virtual void setSensorOffset(uint32_t offset) override {}
    virtual void setSensorAlpha(double alpha) override {}
    virtual uint32_t getSensorValue() override { return value_.to_uint32(); }
    virtual double getSensorPhysical() override { return ampere_.to_float(); }

 protected:
    FwObject *parent_;
    int idx_;     // sensor index 0..1

    FwAttribute value_;
    FwAttribute ampere_;
};
