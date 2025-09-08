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

#include <prjtypes.h>
#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <RunInterface.h>
#include <CanInterface.h>
#include <IrqInterface.h>
#include <gpio_drv.h>
#include <can.h>

class CanDriver : public FwObject,
                  public RunInterface,
                  public CanInterface,
                  public IrqHandlerInterface {
 public:
    CanDriver(const char *name, int busid);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // RunInterface
    virtual void setRun() override;
    virtual void setStop() override;
    virtual void setSleep() override {}

    // CanInterface:
    virtual void SetBaudrated(uint32_t baud) override;
    virtual void StartListenerMode() override;
    virtual void RegisterCanListener(CanListenerInterface *iface) override {}
    virtual int ReadCanFrame(can_frame_type *frame) override;

    // IrqHandlerInterface
    virtual void handleInterrupt(int *argv) override;

    // Common methods
    virtual void triggerError();

 protected:
    virtual uint32_t hwid2canid(uint32_t hwid);

    class ErrTriggerAttribute : public FwAttribute {
     public:
        ErrTriggerAttribute(CanDriver *parent, const char *name)
            : FwAttribute(name) , parent_(parent) {}

        virtual void post_write() override {
            parent_->triggerError();
        }
     protected:
        CanDriver *parent_;
    };

    class RxCounterAttribute : public FwAttribute {
     public:
        RxCounterAttribute(const char *name) : FwAttribute(name), last_(0) {}

        virtual void pre_read() override {
            u_.u32 -= last_;
            last_ = u_.u32;
        }

        void increment() { u_.u32++; }
     protected:
        uint32_t last_;
    };

 protected:
    FwAttribute baudrate_;
    FwAttribute mode_;
    RxCounterAttribute rxcnt_;
    FwAttribute pgm_;
    FwAttribute errcnt_;
    FwAttribute lasterr_;
    ErrTriggerAttribute errTrigger_;

    static const int CAN_RX_FRAMES_MAX = 4;

    int busid_;
    gpio_pin_type gpio_cfg_rx_;
    gpio_pin_type gpio_cfg_tx_;
    CAN_registers_type *dev_;

    int rxframe_wcnt;
    int rxframe_rcnt;
    can_frame_type rxframes[CAN_RX_FRAMES_MAX];

    enum CpuTypes {
        CPU_Unknown,
        CPU_M1,
        CPU_M2,
        CPU_A1,
        CPU_A2,
        CPU_B1,
        CPU_B2,
        CPU_D1,
        CPU_D2,
        CPU_Total
    };
    struct DebugMessageType {
        char buf[32];
        int cnt;
    };
    DebugMessageType dbgmsg_[CPU_Total];
};
