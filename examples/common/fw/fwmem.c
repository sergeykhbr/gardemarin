/*
 *  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <prjtypes.h>
#include <string.h>
#include <stdio.h>
#include <uart.h>

#ifdef _WIN32
char __heap_start__[1 << 19];
char *__stack_limit = &__heap_start__[sizeof(__heap_start__) - 0x400];
#else
extern char __heap_start__[];  // defined in linker script
extern char __stack_limit[];
#endif

#define SP_MAGIC_VALUE 0xCCCCCCCC
#define RAM_ENTRIES_MAX 32

typedef struct ram_data_type {
    char name[8];
    void *pattern;
} ram_data_type;

typedef struct memanager_type {
    char *end;                                    // pinter to heap end
    uint32_t allocated_sz;                        // heap size used
    void *magic;                                  // pointer to itself to mark initialization
    int data_cnt;                                 // Number of registered RAM modules
    ram_data_type data[RAM_ENTRIES_MAX];          // registered RAM modules
} memanager_type;

static memanager_type memanager_;

static memanager_type *get_root_data() {
    return &memanager_;
}

/*
 * @brief Increase program data space.
 * @details Malloc and related functions depend on this
 */
char *_sbrk(int incr) {
    memanager_type *pool = get_root_data();
    char *ret = (char *)pool->end;
    incr = (incr + 3) & (~3); // align value to 4
    pool->allocated_sz += incr;
    pool->end += incr;
    return ret;
}

void fw_malloc_init() {
    memanager_type *pool = get_root_data();
    // Do not clear memory if it was already initialzed to continue after reset
    if (pool->magic != pool) {
        memset(pool, 0, sizeof(memanager_type));
        pool->end = (char *)__heap_start__;
        pool->magic = pool;
    }
}

void *fw_malloc(int size) {
    return _sbrk(size);  
}

void fw_register_ram_data(const char *name, void *data) {
    memanager_type *pool = get_root_data();
    ram_data_type *p = &pool->data[pool->data_cnt++];
    memcpy(p->name, name, 8);
    p->name[7] = 0;
    p->pattern = data;
//    if (pool->data_cnt >= RAM_ENTRIES_MAX) {
//        uart_printk("mempool limit reached %d\n", RAM_ENTRIES_MAX);
//    }
}

void *fw_get_ram_data(const char *name) {
    memanager_type *pool = get_root_data();
    ram_data_type *p;

    if (pool->magic != pool) {
        return 0;   // not initialized
    }

    for (int i = pool->data_cnt-1; i >= 0; i--) {
        p = &pool->data[i];
        if (strcmp(name, p->name) == 0) {
            return p->pattern;
        }
    }
    return 0;
}
