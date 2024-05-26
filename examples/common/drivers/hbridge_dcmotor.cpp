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

#include <prjtypes.h>
#include <fwapi.h>
#include "hbridge_dcmotor.h"
#include "hbridge.h"

struct DcMotorNamesType {
    const char *portname;
    const char *direction_name;
    const char *hz_name;
    const char *duty_name;
};

static const DcMotorNamesType DCMOTOR_CFG[GARDEMARIN_DCMOTOR_PER_HBDRIGE] = {
    {"dc0", "dc0_direction", "dc0_hz", "dc0_duty"},
    {"dc1", "dc1_direction", "dc1_hz", "dc1_duty"}
};

DcMotor::DcMotor(FwObject *parent, int idx) :
    parent_(parent),
    idx_(idx),
    direction_(DCMOTOR_CFG[idx].direction_name, "0=forward;1=backward"),
    hz_(DCMOTOR_CFG[idx].hz_name, "Typical: 100kHz brushed motor; 18-24kHz brushless motor"),
    duty_(DCMOTOR_CFG[idx].duty_name, static_cast<PwmInterface *>(this)) {
    direction_.make_int8(0);
    hz_.make_int32(20000);
}

void DcMotor::Init() {
    parent_->RegisterPortInterface(DCMOTOR_CFG[idx_].portname,
                                   static_cast<PwmInterface *>(this));

    parent_->RegisterAttribute(&direction_);
    parent_->RegisterAttribute(&hz_);
    parent_->RegisterAttribute(&duty_);
}

void DcMotor::setPwmHz(int hz) {
}

// Percentage 1..100 duty cycle
//   100 = No PWM fully enabled
//   0 = disabled
void DcMotor::setPwmDutyCycle(int duty) {
}

// DC motor
// mode = 0 (4-pins control)
//    in[1]  in[0]     out[1] out[0]
//     0       0         z     z 
//     0       1         0     1    forward rotation
//     1       0         1     0    backward rotation
//     1       1         break break 
void DcMotor::enablePwm() {
    static_cast<HBridgeDriver *>(parent_)->startDcMotor(idx_, direction_.to_int8());
}

void DcMotor::disablePwm() {
    static_cast<HBridgeDriver *>(parent_)->stopDcMotor(idx_);
}
