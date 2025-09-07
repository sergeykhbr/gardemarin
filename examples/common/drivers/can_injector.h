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
#include <IrqInterface.h>

class CanInjectorDriver : public FwObject,
                          public IrqHandlerInterface {
 public:
    explicit CanInjectorDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    //IrqHandlerInterface
    virtual void handleInterrupt(int *argv);

    // Common methods:
    virtual void CAN1_eof();
    virtual void CAN2_eof();
    virtual void injectionEnable();
    virtual void injectionDisable();
    virtual void setInjectBit();
    virtual void releaseInjectBit();
    virtual uint32_t getInjectCnt() { return timerStrobHandler_.getInjectCnt(); }
    virtual uint8_t getState() { return timerStrobHandler_.getState(); }

 protected:
    class TimerStrobHandler : public IrqHandlerInterface {
     public:
        TimerStrobHandler(CanInjectorDriver *parent) : parent_(parent),
            state_(State_Idle), inject_ena_(false), injectCnt_(0) {
            cr1_run_.val = 0;
            cr1_run_.bits.CEN = 1;
            cr1_run_.bits.OPM = 1;   // one pulse mode
            cr1_run_.bits.DIR = 1;   // downcount
        }

        virtual void handleInterrupt(int *argv);
        // Common methods:
        virtual void injectEnable();
        virtual void injectRestart();
        virtual void injectDisable() { inject_ena_ = false; }
        virtual uint32_t getInjectCnt() { return injectCnt_; }
        virtual uint32_t getState() { return static_cast<uint8_t>(state_); }
     protected:
        CanInjectorDriver *parent_;
        enum {
            State_Idle,
            State_WaitSof,
            State_WaitCanSymbol,
            State_Injection
        } state_;
        tim_cr1_reg_type cr1_run_;
        bool inject_ena_;
        uint32_t injectCnt_;
    };

 protected:
    class InjectErrorAction : public FwAttribute {
     public:
        InjectErrorAction(CanInjectorDriver *parent, const char *name)
            : FwAttribute(name) , parent_(parent) {}

        virtual void post_write() override;
     protected:
        CanInjectorDriver *parent_;
    };

    class InjectErrorCounter : public FwAttribute {
     public:
        InjectErrorCounter(CanInjectorDriver *parent, const char *name)
            : FwAttribute(name) , parent_(parent) {}

        virtual void pre_read() override {
            u_.u32 = parent_->getInjectCnt();
        }
     protected:
        CanInjectorDriver *parent_;
    };

    class InjectState : public FwAttribute {
     public:
        InjectState(CanInjectorDriver *parent, const char *name)
            : FwAttribute(name) , parent_(parent) {}

        virtual void pre_read() override {
            u_.u8 = parent_->getState();
        }
     protected:
        CanInjectorDriver *parent_;
    };

    class BusUtilizationAttribute : public FwAttribute {
     public:
        BusUtilizationAttribute(CanInjectorDriver *parent, const char *name)
            : FwAttribute(name) , parent_(parent), busy_(0), total_(0) {}

        virtual void pre_read() override;
        virtual void setData(uint32_t busy, uint32_t total) {
            busy_ += busy;
            total_ += total;
        }
     protected:
        CanInjectorDriver *parent_;
        uint32_t busy_;
        uint32_t total_;
    };
 protected:
    InjectErrorAction injectAction_;
    InjectErrorCounter injectCnt_;
    InjectState injectState_;
    FwAttribute baud_cnt_;
    BusUtilizationAttribute can1_util_;
    TimerStrobHandler timerStrobHandler_;
    bool state_;
    uint32_t can1_sof_time_;
};
