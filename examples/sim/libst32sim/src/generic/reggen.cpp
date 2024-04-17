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

#include "reggen.h"

Reg32Generic::Reg32Generic(DeviceGeneric *parent, const char *name, uint64_t off) {
    parent_ = parent;
    memAddr_ = off;
    memSize_ = 4;
    regvalue_ = 0;
    mem_ = reinterpret_cast<uint8_t *>(&regvalue_);
    name_ = std::string(name);
    parent_->addMap(static_cast<ISimMemoryInterface *>(this));
}

Reg32Generic::~Reg32Generic() {
    mem_ = 0;
}

void Reg32Generic::operation(MemoryOperationType *memop){
    uint64_t off = memop->addr - getBaseAddress();
    if (memop->type == Memop_Read) {
        memop->payload.u32[0] = read_action(regvalue_);
    } else {
        regvalue_ = write_action(memop->payload.u32[0]);
    }
}
