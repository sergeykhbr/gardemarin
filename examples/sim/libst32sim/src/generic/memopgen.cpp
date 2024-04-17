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

#include <simutils.h>
#include "memopgen.h"

ISimMemoryInterface::ISimMemoryInterface() : ISimFace("ISimMemoryInterface") {
    memAddr_ = 0;
    memSize_ = 0;
    mem_ = 0;
    mapmax_ = 1;
    mapcnt_ = 0;
    maplist_ = new ISimMemoryInterface *[mapmax_];
}

ISimMemoryInterface::~ISimMemoryInterface() {
    delete [] maplist_;
    if (mem_) {
        delete [] mem_;
    }
}

void ISimMemoryInterface::operation(MemoryOperationType *memop) {
    uint64_t off;
    uint64_t sz;
    for (int i = 0; i < mapcnt_; i++) {
        off = maplist_[i]->getBaseAddress();
        sz = maplist_[i]->getSize();
        if (memop->addr >= off && (memop->addr + memop->size) <= (off + sz)) {
            maplist_[i]->operation(memop);
            return;
        } 
    }
    if (mem_) {
        off = memop->addr - getBaseAddress();
        if (memop->type == Memop_Read) {
            memcpy(memop->payload.u8, &mem_[off], memop->size);
        } else {
            memcpy(&mem_[off], memop->payload.u8, memop->size);
        }
    } else {
        printf("Unmapped access to %" RV_PRI64 "x\n", memop->addr);
    }
}

void ISimMemoryInterface::addMap(ISimMemoryInterface *dev) {
    if (mapcnt_ == mapmax_) {
        ISimMemoryInterface **t1 = new ISimMemoryInterface *[2*mapmax_];
        memcpy(t1, maplist_, mapcnt_*sizeof(ISimMemoryInterface *));
        delete [] maplist_;
        maplist_ = t1;
        mapmax_ = 2*mapmax_;
    }
    maplist_[mapcnt_++] = dev;
}

