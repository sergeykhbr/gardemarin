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

#pragma once

#include "prjtypes.h"

#if defined(__cplusplus)
extern "C"
{
#endif

void fw_malloc_init();
void *fw_malloc(int size);
void fw_register_ram_data(const char *name, void *data);
void *fw_get_ram_data(const char *name);


#if defined(__cplusplus)
}  // extern "C"
#endif
