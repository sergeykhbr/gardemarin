/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

void bkp_init();
int bkp_get_watering_mode();
void bkp_set_watering_mode(int val);

// safety range:
//  0 = disable pwm
//  25 is maximum to avoid overheating
uint32_t bkp_get_pwm_duty();
void bkp_set_pwm_duty(uint32_t val);

int bkp_is_rtc_initilized();
void bkp_set_rtc_initialized();

int bkp_get_rtc_correction();
void bkp_set_rtc_correction(int rtc_corr);

int bkp_get_temperature_correction();
void bkp_set_temperature_correction(int t_corr);


#ifdef __cplusplus
}
#endif

