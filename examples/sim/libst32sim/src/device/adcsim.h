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

class AdcSim : public DeviceGeneric {
 public:
    AdcSim(const char *name, uint64_t addr, size_t sz);

    // DeviceGeneric method
    virtual void update(double dt) override;

    // Common methods:
    void clearPendingBit() {
        SR.clearEndOfSequence();
    }

 private:
    // Status
    class SR_TYPE : public Reg32Generic {
     public:
        SR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        void setEndOfSequence() { regvalue_ |= 0x2; }
        void clearEndOfSequence() { regvalue_ &= ~0x2; }
    };

    // Control register 1
    class CR1_TYPE : public Reg32Generic {
     public:
        CR1_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        bool isIrqEnabled() {
            // [5] EOCIE: interrupt enable for EOC
            return (regvalue_ & (1 << 5));
        }
    };

    // Control register 2
    class CR2_TYPE : public Reg32Generic {
     public:
        CR2_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        bool isEnabled() {
            return (regvalue_ & 0x1) && (regvalue_ & 0x2);  // enable and continuous mode
        }
    };

    // Data register
    class DR_TYPE : public Reg32Generic {
     public:
        DR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
        virtual uint32_t read_action(uint32_t prev) override {
            static_cast<AdcSim *>(parent_)->clearPendingBit();
            return prev;
        }
    };

 private:
    SR_TYPE SR;
    CR1_TYPE CR1;       // Control reg 1
    CR2_TYPE CR2;       // Control reg 1
    DR_TYPE DR;         // data

    int seqcnt_;
};
