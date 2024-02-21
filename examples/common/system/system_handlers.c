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
#include "system_handlers.h"

extern void system_init(void);
extern int __main(void);

void __attribute__ ((section(".hard_reset"), noreturn))
Reset_Handler()
{
    DisableIrqGlobal();
    system_init();

    __main();

    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
NMI_Handler()
{
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
HardFault_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
MemManage_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
BusFault_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
UsageFault_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
SVC_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
DebugMon_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
PendSV_Handler() {
    while(1)
    {
    }
}

void __attribute__ ((section(".hard_reset"), weak))
SysTick_Handler() {
}

