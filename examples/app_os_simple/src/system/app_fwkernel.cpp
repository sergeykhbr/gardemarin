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

#include <fwapi.h>
#include <uart.h>
#include "app_fwkernel.h"
#include <string.h>
#include <new>

/**
 * @brief Kernel module constructor. Instance of this KernelClass registered
 *        as the first element in the single linked list of FwObjects.
 */
AppKernelClass::AppKernelClass(const char *name) : KernelClassGeneric(name),
    relais0_("relais0", 0),
    relais1_("relais1", 1),
    ledrbw_("ledrbw"),
    can1_("can1", 0),
    can2_("can2", 1),
    scales_("scales"),
    uled0_("uled0"),
    ubtn0_("ubtn0"),
    adc1_("adc1"),
    hbrg0_("hbrg0", 0),
    hbrg1_("hbrg1", 1),
    hbrg2_("hbrg2", 2),
    hbrg3_("hbrg3", 3),
    temp0_("temp0"),
    soil0_("soil0"),
    settings_("usrset")
{
    version_.make_uint32(0x20240804);
    output_.make_int32(0);
}

extern "C" void kernel_init() {
    new (fw_malloc(sizeof(AppKernelClass))) AppKernelClass("kernel");
}

