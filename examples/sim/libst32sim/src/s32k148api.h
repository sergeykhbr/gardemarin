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

#ifdef __cplusplus
extern "C" {
#endif

/** Enable disable usage of VXL driver. Disable by default (=0) */
#define S32K148_OPT_VXL_DRIVER 1

typedef void (*isr_type)(void);

/** Create and initialize all simulation objects including S32K148 model */
void s32k148_init();

/** Destroy and free resources allocated for the simulation objects */
void s32k148_destroy();

/** Register interrupt handler in the simulation context. These functions could
    be received from NVIC model but it won't work in a case of 64-bits
    simulation. Additionally unmaskable interrupts do not transmit into
    NVIC */
void s32k148_register_isr(int idx, isr_type handler);

/** Start firmware in a seprate thread and control by simulation context
    scheduler. */
void s32k148_run_firmware(void *entry);

/** Raise request interrupt from external device */
void s32k148_request_interrupt(int idx);

/** Set simulation model option */
int s32k148_set_opt(int optname, void *optval);

#ifdef __cplusplus
}  // extern "C"
#endif
