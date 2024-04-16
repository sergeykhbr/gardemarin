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
#include <PwmInterface.h>
#include <IrqInterface.h>
#include <gpio_drv.h>


class LedColorPort : public PwmInterface {
 public:
    LedColorPort(FwObject *parent,
                 const gpio_pin_type *pin,
                 const char *portname,
                 int idx);

    // PwmInterface:
    virtual void setPwmHz(int hz) override;
    virtual void setPwmDutyCycle(int duty) override;
    virtual void enablePwm() override;
    virtual void disablePwm() override;

 protected:
    FwObject *parent_;
    const gpio_pin_type *pin_;
    int idx_;
};

// Use one PWM generator TIM2_CH3 at high frequency (2..100 kHz) to form
// LED channel PWM with the frequency 200 Hz (default)
class LedStripDriver : public FwObject,
                       public IrqHandlerInterface {
 public:
    LedStripDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // IrqHandler interface:
    virtual void handleInterrupt(int *argv) override;

 public:
    // Accessed from channels:
    void setChannelPwmHz(int chidx, int hz) {}
    void setChannelPwmDutyCycle(int chidx, int duty) {}


 protected:
    FwAttribute tim_hz_;
    FwAttribute red_hz_;
    FwAttribute red_duty_;
    FwAttribute blue_hz_;
    FwAttribute blue_duty_;
    FwAttribute white_hz_;
    FwAttribute white_duty_;
    FwAttribute mixed_hz_;
    FwAttribute mixed_duty_;

    LedColorPort red_;
    LedColorPort blue_;
    LedColorPort white_;
    LedColorPort mixed_;

    uint32_t tim_cnt_;

    // for the fast access initialize in constructor the following pointers
    // instead of using new() operator
    struct ColorChannelType {
        FwAttribute *hz;
        FwAttribute *duty;
        LedColorPort *port;
        uint32_t cnt_modulo;      // tim_cnt % cnt_module
        uint32_t cnt_switch;      // cnt_switch < (tim_cnt % cnt_module) is "ON, otherwise "OFF"
    } chn_[GARDEMARIN_LED_STRIP_TOTAL];
};
