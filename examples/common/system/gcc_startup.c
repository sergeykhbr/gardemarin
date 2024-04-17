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

extern void fwmain(void);

#if!defined _WIN32

void init_data_bss(void)
{
    /* Declare pointers for various data sections. These pointers
     * are initialized using values pulled in from the linker file */
    uint8_t * bss_start;
    const uint8_t * bss_end;

    extern uint32_t __BSS_START[];
    extern uint32_t __BSS_END[];

    /* BSS */
    bss_start       = (uint8_t *)__BSS_START;
    bss_end         = (uint8_t *)__BSS_END;

    /* Clear the zero-initialized data section */
    while (bss_end != bss_start)
    {
        *bss_start = 0;
        bss_start++;
    }
}

// These magic symbols are provided by the linker.
extern void
(*__preinit_array_start[]) (void) __attribute__((weak));
extern void
(*__preinit_array_end[]) (void) __attribute__((weak));
extern void
(*__init_array_start[]) (void) __attribute__((weak));
extern void
(*__init_array_end[]) (void) __attribute__((weak));
extern void
(*__fini_array_start[]) (void) __attribute__((weak));
extern void
(*__fini_array_end[]) (void) __attribute__((weak));

// Iterate over all the preinit/init routines (mainly static constructors).
inline void
__attribute__((always_inline))
__run_init_array (void)
{
    int count;
    int i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
    {
        __preinit_array_start[i] ();
    }

    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
    {
        __init_array_start[i]();
    }
}

// Run all the cleanup routines (mainly static destructors).
inline void
__attribute__((always_inline))
__run_fini_array (void)
{
    int count;
    int i;

    count = __fini_array_end - __fini_array_start;

    for (i = count; i > 0; i--)
    {
       __fini_array_start[i - 1] ();
    }
}

extern char __DATA_START[];
extern char __DATA_SIZE[];
extern char __DATA_LOAD_START[];

void copy_data_section(void)
{
    memcpy(__DATA_START, __DATA_LOAD_START, (int)__DATA_SIZE);
//    for (int i = 0; i < (int)__DATA_SIZE; i++) {
//        __DATA_START[i] = __DATA_LOAD_START[i];
//    }
}


int __main (void)
{
    init_data_bss();

    // Call the standard library initialisation (mandatory for C++ to
    // execute the constructors for the static objects).
   __run_init_array();

   copy_data_section();

    // Call the main entry point, and save the exit code.
    fwmain();

    // Run the C++ static destructors.
    __run_fini_array();

    return 0;
}

#endif  // !_WIN32
