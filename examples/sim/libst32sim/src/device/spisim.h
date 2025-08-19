/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

class SpiSim : public DeviceGeneric {
 public:
    SpiSim(const char *name, uint64_t addr, size_t sz, int irqidx);

    // DeviceGeneric method
    virtual void update(double dt) override;

 private:
    // Control register 1
    class CR1_TYPE : public Reg32Generic {
     public:
        CR1_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        // [6] SPE: counter enable
        bool isEnabled() {
            return (regvalue_ & (1 << 6));
        }
    };

    // Control register 2
    class CR2_TYPE : public Reg32Generic {
     public:
        CR2_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
    };

    // Status register
    class SR_TYPE : public Reg32Generic {
     public:
        SR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
        virtual uint32_t read_action(uint32_t prev) override {
            prev |= (1 << 1);   // [1] TXE tx buffer empty
            prev &= ~(1 << 7);  // [7] BSY is busy
            return prev;
        }
    };

    class DR_TYPE : public Reg32Generic {
     public:
        DR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
    };

 private:
    int irqidx_;
    CR1_TYPE CR1;       // Control reg 1
    CR2_TYPE CR2;       // Control reg 2
    SR_TYPE SR;         // Status
    DR_TYPE DR;         // Data

};
