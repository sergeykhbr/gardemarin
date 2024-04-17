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

class Reg32Generic : public ISimMemoryInterface {
 public:
    Reg32Generic(DeviceGeneric *parent, const char *name, uint64_t off);
    virtual ~Reg32Generic();

    virtual void operation(MemoryOperationType *memop) override;

    // Side effect methods:
    virtual uint32_t read_action(uint32_t prev) { return prev; }
    virtual uint32_t write_action(uint32_t newval) { return newval; }

    // No side effect methods
    virtual void set(uint32_t val) { regvalue_ = val; }
    virtual uint32_t get() { return regvalue_; }

 protected:
    std::string name_;
    DeviceGeneric *parent_;
    uint32_t regvalue_;
};
