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

#ifdef _WIN32
char __heap_start__[1 << 19];
char *__stack_limit = &__heap_start__[sizeof(__heap_start__) - 0x400];
#else
extern char __heap_start__[];  // defined in linker script
extern char __stack_limit[];
#endif

#define SP_MAGIC_VALUE 0xCCCCCCCC
#define RAM_ENTRIES_MAX 128

typedef struct ram_data_type {
    char name[8];
    void *pattern;
} ram_data_type;

typedef struct memanager_type {
    uint32_t end;                                 // pinter to heap end
    uint32_t allocated_sz;                        // heap size used
    void *magic;                                  // pointer to itself to mark initialization
    int data_cnt;                                 // Number of registered RAM modules
    ram_data_type data[RAM_ENTRIES_MAX];          // registered RAM modules
} memanager_type;


static memanager_type *get_root_data() {
    return (memanager_type *)__heap_start__;
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

    if (pool->end >= (uint32_t)__stack_limit) {
        printf("heap limit reached %08x\n", pool->end);
    }
    return ret;
}

void fw_malloc_init() {
    memanager_type *pool = get_root_data();
   // 4-bytes alignment
    memset(pool, 0, sizeof(memanager_type));
    pool->allocated_sz = (sizeof(memanager_type) + 3) & ~0x3ull;
    pool->end = ((uint32_t)__heap_start__) + pool->allocated_sz;
    pool->data_cnt = 0;
    pool->magic = pool;
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
    if (pool->data_cnt >= RAM_ENTRIES_MAX) {
        printf("mempool limit reached %d\n", RAM_ENTRIES_MAX);
    }
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
