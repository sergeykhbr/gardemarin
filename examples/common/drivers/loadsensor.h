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

class LoadSensorPort : public FwAttribute,
                       public SensorInterface {
 public:
    LoadSensorPort(FwObject *parent,
                   int idx);

    // SensorInterface:
    virtual int32_t getSensorValue() override { return to_int32(); }

    // Common interface
    void Init();     // register attribute in parent class
    void setSensorValue(uint32_t val);

 protected:
    FwObject *parent_;
    FwAttribute alpha_;
    FwAttribute zero_;
    FwAttribute tara_;
    FwAttribute gram_;
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
    virtual uint64_t getTimerInterval() { return 500; }
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

