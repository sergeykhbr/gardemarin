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
#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <CanInterface.h>
#include <gpio_drv.h>
#include <can.h>

#pragma once

class CanDriver : public FwObject,
                  public CanInterface {
 public:
    CanDriver(const char *name, int busid);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // CanInterface:
    virtual void RxInterruptHandler(int fifoid) override;
    virtual void SetBaudrated(uint32_t baud) override;
    virtual void StartListenerMode() override;
    virtual void Stop() override;
    virtual void RegisterCanListener(CanListenerInterface *iface) override {}
    virtual int ReadCanFrame(can_frame_type *frame) override;

 protected:
    virtual uint32_t hwid2canid(uint32_t hwid);

 protected:
    FwAttribute baudrate_;
    FwAttribute mode_;
    FwAttribute test1_;

    static const int CAN_RX_FRAMES_MAX = 4;

    int busid_;
    gpio_pin_type gpio_cfg_rx_;
    gpio_pin_type gpio_cfg_tx_;
    CAN_registers_type *dev_;

    int rxframe_wcnt;
    int rxframe_rcnt;
    can_frame_type rxframes[CAN_RX_FRAMES_MAX];
};
