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

class TimSim : public DeviceGeneric {
 public:
    TimSim(const char *name, uint64_t addr, size_t sz, int irqidx);

    // DeviceGeneric method
    virtual void update(double dt) override;

 private:
    // Interrupt enabler
    class DIER_TYPE : public Reg32Generic {
     public:
        DIER_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        bool isUpdateIrqEnabled() {
            // [0] UIE: update interrupt enable
            return (regvalue_ & (1 << 0));
        }
    };

    // Control register 1
    class CR1_TYPE : public Reg32Generic {
     public:
        CR1_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        // [0] CEN: counter enable
        bool isEnabled() {
            return (regvalue_ & (1 << 0));
        }
        // [3] OPM: one-pulse mode
        bool isOnePulseMode() {
            return (regvalue_ & (1 << 3));
        }
    };

    // Status register
    class SR_TYPE : public Reg32Generic {
     public:
        SR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
    };

 private:
    int irqidx_;
    CR1_TYPE CR1;       // Control reg 1
    DIER_TYPE DIER;
    SR_TYPE SR;
    Reg32Generic ARR;   // reloading value

};
