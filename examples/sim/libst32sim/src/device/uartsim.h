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

class UARTSim : public DeviceGeneric {
 public:
    UARTSim(const char *name, uint64_t addr, size_t sz, int irqidx);

    // DeviceGeneric method
    virtual void update(double dt) override;

    void clearTxEmpty() { txempty_ = false; }

 private:

    // status register:
    //   [7] TXE, transmit data register empty
    class SR_TYPE : public Reg32Generic {
     public:
       SR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
           : Reg32Generic(parent, name, off) {
       }
     protected:
        virtual uint32_t read_action(uint32_t prev) override;
    };

    // Data register
    class DR_TYPE : public Reg32Generic {
     public:
        DR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
    };

    // Control register 1
    class CR1_TYPE : public Reg32Generic {
     public:
        CR1_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
        bool isTxEmptyIrqEna() {
            // [7] TXEIE transmission irq ena
            return regvalue_ & (1 << 7) ? true: false;
        }
        bool isTxCompleteIrqEna() {
            // [6] TCIE transmission complete irq ena
            return regvalue_ & (1 << 6) ? true: false;
        }
        bool isRxNonEmptyIrqEna() {
            // [5] RXNEIE rxne irq ena
            return regvalue_ & (1 << 5) ? true: false;
        }
    };

 private:
    SR_TYPE SR;
    DR_TYPE DR;
    CR1_TYPE CR1;
    int irqidx_;
    bool txempty_;
};