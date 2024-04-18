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
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include "ledstrip.h"

// LED[0] PB[10] TIM2_CH3
// LED[1] PE[0]
// LED[2] PE[1]
// LED[3] PE[15]
static const gpio_pin_type gpio_cfg[GARDEMARIN_LED_STRIP_TOTAL] = {
    {(GPIO_registers_type *)GPIOB_BASE, 10},
    {(GPIO_registers_type *)GPIOE_BASE, 0},
    {(GPIO_registers_type *)GPIOE_BASE, 1},
    {(GPIO_registers_type *)GPIOE_BASE, 15}
};

LedStripDriver::LedStripDriver(const char *name) : FwObject(name),
    tim_hz_("tim_hz"),
    red_hz_("red_hz"),
    red_duty_("red_duty"),
    blue_hz_("blue_hz"),
    blue_duty_("blue_duty"),
    white_hz_("white_hz"),
    white_duty_("white_duty"),
    mixed_hz_("mixed_hz"),
    mixed_duty_("mixed_duty"),
    red_(static_cast<FwObject *>(this), &gpio_cfg[0], "red", 0),
    blue_(static_cast<FwObject *>(this), &gpio_cfg[1], "blue", 1),
    white_(static_cast<FwObject *>(this), &gpio_cfg[2], "white", 2),
    mixed_(static_cast<FwObject *>(this), &gpio_cfg[3], "mixed", 3) {

    tim_cnt_ = 0;

    // access by index:
    chn_[0].hz = &red_hz_;
    chn_[0].duty = &red_duty_;
    chn_[0].port = &red_;
    chn_[1].hz = &blue_hz_;
    chn_[1].duty = &blue_duty_;
    chn_[1].port = &blue_;
    chn_[2].hz = &white_hz_;
    chn_[2].duty = &white_duty_;
    chn_[2].port = &white_;
    chn_[3].hz = &mixed_hz_;
    chn_[3].duty = &mixed_duty_;
    chn_[3].port = &mixed_;
}

void LedStripDriver::Init() {
    RegisterAttribute(&tim_hz_);
    RegisterAttribute(&red_hz_);
    RegisterAttribute(&red_duty_);
    RegisterAttribute(&blue_hz_);
    RegisterAttribute(&blue_duty_);
    RegisterAttribute(&white_hz_);
    RegisterAttribute(&white_duty_);
    RegisterAttribute(&mixed_hz_);
    RegisterAttribute(&mixed_duty_);

    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterPortInterface("red", static_cast<PwmInterface *>(&red_));
    RegisterPortInterface("blue", static_cast<PwmInterface *>(&blue_));
    RegisterPortInterface("white", static_cast<PwmInterface *>(&white_));
    RegisterPortInterface("mixed", static_cast<PwmInterface *>(&mixed_));
}

void LedStripDriver::handleInterrupt(int *argv) {
    uint32_t cnt;
    ColorChannelType *pch;
    ++tim_cnt_;

    for (int i = 0; i < GARDEMARIN_LED_STRIP_TOTAL; i++) {
        pch = &chn_[i];
        cnt = tim_cnt_ % pch->cnt_modulo;
        if (cnt < pch->cnt_switch) {
            pch->port->disablePwm();
        } else {
            pch->port->enablePwm();
        }
    }
}

LedColorPort::LedColorPort(FwObject *parent,
                           const gpio_pin_type *pin,
                           const char *portname,
                           int idx) :
    parent_(parent),
    pin_(pin),
    idx_(idx) {

    gpio_pin_as_output(pin,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);
    disablePwm();
}

void LedColorPort::setPwmHz(int hz) {
    LedStripDriver *p = static_cast<LedStripDriver *>(parent_);
    p->setChannelPwmHz(idx_, hz);
}

void LedColorPort::setPwmDutyCycle(int duty) {
    LedStripDriver *p = static_cast<LedStripDriver *>(parent_);
    p->setChannelPwmDutyCycle(idx_, duty);
}

void LedColorPort::enablePwm() {
    gpio_pin_set(pin_);
}

void LedColorPort::disablePwm() {
    gpio_pin_clear(pin_);
}
