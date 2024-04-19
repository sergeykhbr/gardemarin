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
#include <fwapi.h>

typedef struct FwFifo {
    char *arr;
    int wcnt;
    int rcnt;
    int size;
} FwFifo;

void fw_fifo_init(FwFifo *ff, int sz);

int fw_fifo_get_count(FwFifo *ff);

int fw_fifo_is_empty(FwFifo *ff);

int fw_fifo_is_full(FwFifo *ff);

void fw_fifo_put(FwFifo *ff, char v);

void fw_fifo_get(FwFifo *ff, char *v);
