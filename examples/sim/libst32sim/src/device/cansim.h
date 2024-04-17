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

#include <devgen.h>
#include <reggen.h>
#include <simutils.h>
#include <shm_common.h>
#include <can_common.h>

class CANSim : public DeviceGeneric {
 public:
    CANSim(const char *name, uint64_t addr, size_t sz);
    virtual ~CANSim();

    // DeviceGeneric method
    virtual void update(double dt) override;

    /** Common methods */
    enum EStateType {
        State_Off,
        State_Init,
        State_Sleep,
        State_Idle,
        State_Rx,
        State_Tx
    };

    EStateType getState() { return estate_; }
    void setState(EStateType v) { estate_ = v; }

 private:

    bool isBusy();
    void write_frame();
    void writeFrameToShm();
    void writeFrameToVxl();
    int checkRxFrameID();

 private:
    class CAN_MCR_TYPE : public Reg32Generic {
     public:
        CAN_MCR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
            set(0);    // reset value
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
    };

    class CAN_MSR_TYPE : public Reg32Generic {
     public:
        CAN_MSR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
     protected:
        virtual uint32_t read_action(uint32_t prev) override;
    };

    class CAN_RAMn_FRAME : public ISimMemoryInterface {
     public:
        CAN_RAMn_FRAME(DeviceGeneric *parent, const char *name,
                uint64_t off, int idx) {
            parent_ = parent;
            memAddr_ = off;
            memSize_ = sizeof(CanFrameType);
            mem_ = reinterpret_cast<uint8_t *>(&frame_);
            name_ = std::string(name);
            parent_->addMap(static_cast<ISimMemoryInterface *>(this));
            newData_ = false;
        }
        virtual ~CAN_RAMn_FRAME() {
            mem_ = 0;
        }

        virtual void operation(MemoryOperationType *memop) override {
            uint64_t off = memop->addr - getBaseAddress();
            if (memop->type == Memop_Read) {
                memop->payload.u32[0] = frame_.regs[off >> 2];
            } else {
                frame_.regs[off >> 2] = memop->payload.u32[0];
            }
        }

        CanFrameType *frame() { return &frame_; }
        // true = Tx; false = Rx
        bool TX() { return frame_.bits.CODE == 0xC; }
        bool RX() { return frame_.bits.CODE == 0x4; }
        void CLRTX() { frame_.bits.CODE = 0; }
        uint8_t DLC() { return frame_.bits.DLC; }
        uint32_t ID() { return (frame_.bits.IDEXT << 11) | frame_.bits.IDSTD;}
        uint8_t *DATA() { return frame_.bits.data; }

     private:
        std::string name_;
        DeviceGeneric *parent_;
        int idx_;
        CanFrameType frame_;
        bool newData_;
    };

 private:
    CAN_MCR_TYPE MCR;
    CAN_MSR_TYPE MSR;
    Reg32Generic RXMGMASK;
    Reg32Generic IMASK1;
    Reg32Generic IFLAG1;
    Reg32Generic *RXIMR[32];
    CAN_RAMn_FRAME *RAMn_FRAME[32];

    EStateType estate_;
    int msgsel_;

    sharemem_def shmem_handle_;
    ShmMemType *shcan_;
    int busId_;
    bool irqRequest_;
};