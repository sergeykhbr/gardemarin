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

#include "devgen.h"

DeviceGeneric::DeviceGeneric(const char *name, uint64_t addr, size_t sz) {
    strcpy(devname_, name);
    memAddr_ = addr;
    memSize_ = sz;
}

void DeviceGeneric::init() {
    if (memSize_ < 1024 * 1024) {
        mem_ = new uint8_t[memSize_];
    }
}

