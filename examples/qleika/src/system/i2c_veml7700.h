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

void i2c_init();
void veml7700_configure();

void i2c_reset();
int is_i2c_busy();
int is_i2c_error();
int get_i2c_err_state();  // previous to error state
int get_lux();
void update_lux();

#ifdef __cplusplus
}
#endif

