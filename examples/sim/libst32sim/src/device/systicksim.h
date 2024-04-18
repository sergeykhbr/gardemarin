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

class SysTickSim : public DeviceGeneric {
 public:
    SysTickSim(const char *name, uint64_t addr, size_t sz);

    // DeviceGeneric method
    virtual void update(double dt) override;

 private:
    // Control and Status
    class CSR_TYPE : public Reg32Generic {
     public:
        CSR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }

        bool isEnabled() { return (regvalue_ & 0x1) ? true: false; }
        bool isIrqEnabled() { return (regvalue_ & 0x2) ? true: false; }
    };

    // Current value
    class CVR_TYPE : public Reg32Generic {
     public:
        CVR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
        virtual uint32_t read_action(uint32_t prev) override;
    };

 private:
    CSR_TYPE CSR;
    Reg32Generic RVR;   // reload value
    CVR_TYPE CVR;       // Current value
    Reg32Generic CALIB; // calibration value

    uint32_t sysclk_hz_;
    double time_;
};
