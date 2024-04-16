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
#include <RunInterface.h>
#include <TimerInterface.h>
#include <SensorInterface.h>
#include <gpio_drv.h>

class LoadSensorPort : public SensorInterface {
 public:
    LoadSensorPort(FwObject *parent,
                   int idx);

    // SensorInterface:
    virtual void setSensorValue(uint32_t val) override;
    virtual void setSensorOffset(uint32_t offset) override { offset_.make_uint32(offset); }
    virtual void setSensorAlpha(double alpha) override { alpha_.make_double(alpha); }
    virtual uint32_t getSensorValue() override { return value_.to_uint32(); }
    virtual double getSensorPhysical() override { return gram_.to_float(); }

    // Common interface
    virtual void Init();     // register attribute in parent class

 protected:
    FwObject *parent_;
    const char *portname_;
    FwAttribute value_;
    FwAttribute gram_;
    FwAttribute offset_;
    FwAttribute alpha_;
    int idx_;
};

class LoadSensorDriver : public FwObject,
                         public RunInterface,
                         public TimerListenerInterface {
 public:
    LoadSensorDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // RunInterface
    virtual void setRun() override {}
    virtual void setStop() override {}
    virtual void setSleep() override;

    // TimerListenerInterface
    virtual void callbackTimer(uint64_t tickcnt) override;

 protected:
    // Accessed from channels:
    void selectChannel(int chidx);

 protected:

    // for the fast access initialize in constructor the following pointers
    // instead of using new() operator
    struct SensorType {
        LoadSensorPort *port;
    } chn_[GARDEMARIN_LOAD_SENSORS_TOTAL];
};

