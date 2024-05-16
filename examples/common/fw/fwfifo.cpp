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


#include "fwfifo.h"

void fw_fifo_init(FwFifo *ff, int sz) {
    ff->arr = reinterpret_cast<char *>(fw_malloc(sz));
    ff->wcnt = 1;
    ff->rcnt = 0;
    ff->size = sz;
}

int fw_fifo_get_count(FwFifo *ff) {
    int ret = ff->wcnt - ff->rcnt - 1;
    if (ret < 0) {
        ret += ff->size;
    } else if (ret >= ff->size) {
        ret -= ff->size;
    }
    return ret;
}

int fw_fifo_is_empty(FwFifo *ff) {
    return fw_fifo_get_count(ff) == 0;
}

int fw_fifo_is_full(FwFifo *ff) {
    return ff->wcnt == ff->rcnt;
}

void fw_fifo_put(FwFifo *ff, char v) {
    if (fw_fifo_is_full(ff)) {
        return;
    }
    ff->arr[ff->wcnt] = v;

    int tcnt = ff->wcnt + 1;
    if (tcnt >= ff->size) {
        tcnt = 0;
    }
    ff->wcnt = tcnt;
}

void fw_fifo_get(FwFifo *ff, char *v) {
    if (fw_fifo_is_empty(ff)) {
        return;
    }
    int tcnt = ff->rcnt + 1;
    if (tcnt >= ff->size) {
        tcnt = 0;
    }
    *v = ff->arr[tcnt];
    ff->rcnt = tcnt;
}
