/*
 *  Copyright 2020 Sergey Khabarov, sergeykhbr@gmail.com
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

/** Serial Interface registers exposed into shared memory */
static const int SHMEM_BUS_TOTAL = 2;
static const int SHMEM_BUF_SIZE = 64;

#pragma pack(1)
union CanFrameType {
    char b8[72];
    struct bit_fields {
        uint32_t timestamp : 16;        // [0][15:0] free running counter
        uint32_t DLC : 4;               // [0][19:16] Length of Data bytes
        uint32_t RTR : 1;               // [0][20] Remote Transmition Request
        uint32_t IDE : 1;               // [0][21] ID Extended Bit
        uint32_t SRR : 1;               // [0][22] Substitute Remote Request
        uint32_t rsv023 : 1;            // [0][23]
        uint32_t CODE : 4;              // [0][27:24] Message buffer code: 0xC = Tx Data Frame (RTR must be 0)
        uint32_t rsv028 : 1;            // [0][28]
        uint32_t ESI : 1;               // [0][29] Error state indicator
        uint32_t BRS : 1;               // [0][30] Bit Rate Switch inside CAN FD frame
        uint32_t EDL : 1;               // [0][31] Extended Data Length: 1=CANFD
        uint32_t IDEXT : 18;            // [1][17:0]
        uint32_t IDSTD : 11;            // [1][28:18] Frame ID
        uint32_t PRIO : 3;              // [1][31:29] Local Prioirty
        uint8_t data[64];
    } bits;
    uint32_t regs[18];
};
#pragma pack()

struct shm_fifo_type {
    CanFrameType buf[SHMEM_BUF_SIZE];
    int32_t wrcnt;
    int32_t rdcnt;
};

struct shm_bus_type {
    shm_fifo_type dev2pc;
    shm_fifo_type pc2dev;
};

struct ShmMemType {
    int size;                   // Use it as 'data was initialized' flag
    shm_bus_type bus[SHMEM_BUS_TOTAL];
};

static void shm_init(ShmMemType *shm) {
    memset(shm, 0, sizeof(ShmMemType));
    for (int i = 0; i < SHMEM_BUS_TOTAL; i++) {
        shm->bus[i].dev2pc.wrcnt = 1;
        shm->bus[i].pc2dev.wrcnt = 1;
    }
    shm->size = sizeof(ShmMemType);
}

static bool shm_is_empty(shm_fifo_type *fifo) {
    int diff = fifo->wrcnt - fifo->rdcnt;
    if (diff < 0) {
        diff += SHMEM_BUF_SIZE;
    }
    return diff == 1;
}

static bool shm_is_full(shm_fifo_type *fifo) {
    return fifo->rdcnt == fifo->wrcnt;
}

static int shm_get(shm_fifo_type *fifo, CanFrameType &out) {
    if (shm_is_empty(fifo)) {
        return 0;
    }
    if (++fifo->rdcnt >= SHMEM_BUF_SIZE) {
        fifo->rdcnt = 0;
    }
    out = fifo->buf[fifo->rdcnt];
    return 1;
}

static int shm_set(shm_fifo_type *fifo, CanFrameType in) {
    if (shm_is_full(fifo)) {
        return 0;
    }
    fifo->buf[fifo->wrcnt] = in;
    if (++fifo->wrcnt >= SHMEM_BUF_SIZE) {
        fifo->wrcnt = 0;
    }
    return 1;
}

