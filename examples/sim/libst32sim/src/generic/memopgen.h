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

#include <inttypes.h>
#include <string>
#include <CommonInterface.h>

typedef enum EMemopAction {
    Memop_Read,
    Memop_Write  
} EMemopAction;

struct MemoryOperationType {
    EMemopAction type;
    size_t size;
    uint64_t addr;
    union PayloadType {
        uint8_t u8[8];
        uint16_t u16[4];
        uint32_t u32[2];
        uint64_t u64;
    } payload;
};

class ISimMemoryInterface : public CommonInterface {
 public:
    ISimMemoryInterface();
    virtual ~ISimMemoryInterface();

    virtual void operation(MemoryOperationType *memop);

    virtual uint64_t getBaseAddress() { return memAddr_; }

    virtual uint64_t getSize() { return memSize_; }

    virtual void addMap(ISimMemoryInterface *dev);

 protected:
    uint64_t memAddr_;
    size_t memSize_;
    uint8_t *mem_;
    // The std::list cannot be used beacuse of locking before SuspendThread
    // and then there's no possiblity to use iterator while thread is suspended:
    //std::list<ISimMemoryInterface *>map_;
    ISimMemoryInterface **maplist_;
    int mapmax_;
    int mapcnt_;
};
