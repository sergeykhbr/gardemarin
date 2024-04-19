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

class DcMotor : public PwmInterface {
 public:
    DcMotor(FwObject *parent, int idx);

    // Common interface
    void Init();

    // PwmInterface:
    // Typical: 200 - 400 Hz for LED Strip
    //          18000 - 24000 Hz for brushless motors
    //          80000 - 100000 Hz for brushed motors/pumps
    virtual void setPwmHz(int hz) override;
    // Percentage 1..100 duty cycle
    //   100 = No PWM fully enabled
    //   0 = disabled
    virtual void setPwmDutyCycle(int duty) override;
    virtual void enablePwm() override;
    virtual void disablePwm() override;

 protected:
    class PwmDutyAttribute : public FwAttribute {
     public:
        explicit PwmDutyAttribute(const char *name, PwmInterface *ipwm) :
            FwAttribute(name,"duty cycle in percentage: 0=dis, 100=fully ena"), ipwm_(ipwm) {
            make_int8(0);
        }

        virtual void post_write() override {
            if (to_int8()) {
                ipwm_->enablePwm();
            } else {
                ipwm_->disablePwm();
            }
        }
     private:
        PwmInterface *ipwm_;
    };

 protected:
    FwObject *parent_;
    int idx_;     // dcmotor index 0..1

    FwAttribute direction_;
    FwAttribute hz_;
    PwmDutyAttribute duty_;
};
