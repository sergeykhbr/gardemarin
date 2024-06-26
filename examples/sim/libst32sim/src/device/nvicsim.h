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

class NVICSim : public DeviceGeneric {
 public:
    NVICSim(const char *name, uint64_t addr, size_t sz);
    virtual ~NVICSim();

    /** Common NVIC methods: */
    bool requestIrq(int idx);
    int nextPendingIrq();       // return -1 if no enabled pending bits

    void setIE(int idx) { ie_[idx >> 5] |= 1ul << (idx & 0x1f); }
    void clearIE(int idx) { ie_[idx >> 5] &= ~(1ul << (idx & 0x1f)); }
    void setIP(int idx) { ip_[idx >> 5] |= 1ul << (idx & 0x1f); }
    void clearIP(int idx) { ip_[idx >> 5] &= ~(1ul << (idx & 0x1f)); }

 private:
    class ISER_TYPE : public Reg32Generic {
     public:
        ISER_TYPE(DeviceGeneric *parent, const char *name, uint64_t off, int sidx)
            : Reg32Generic(parent, name, off) {
            startidx_ = sidx;
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
     private:
        int startidx_;
    };

    class ICER_TYPE : public Reg32Generic {
     public:
        ICER_TYPE(DeviceGeneric *parent, const char *name, uint64_t off, int sidx)
            : Reg32Generic(parent, name, off) {
            startidx_ = sidx;
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
     private:
        int startidx_;
    };

    class ISPR_TYPE : public Reg32Generic {
     public:
        ISPR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off, int sidx)
            : Reg32Generic(parent, name, off) {
            startidx_ = sidx;
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
     private:
        int startidx_;
    };

    class ICPR_TYPE : public Reg32Generic {
     public:
        ICPR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off, int sidx)
            : Reg32Generic(parent, name, off) {
            startidx_ = sidx;
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
     private:
        int startidx_;
    };

 private:
    ISER_TYPE *ISER[8];      // Interrupt set enable
    ICER_TYPE *ICER[8];      // Interrupt clear enable (disable)
    ISPR_TYPE *ISPR[8];      // Interrupt set pending
    ICPR_TYPE *ICPR[8];      // Interrupt clear pending

    uint32_t ie_[8];            // interrupt enabled mask
    uint32_t ip_[8];            // pending bit
    uint32_t queue_[256];
};