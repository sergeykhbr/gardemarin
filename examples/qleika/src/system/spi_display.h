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

void display_init();
void display_clearScreen();
void display_clearLines(int start, int total, uint16_t clr);
void display_draw_point(uint16_t x, uint16_t y, uint32_t clr);
void display_outputText24Line(char *str, int linepos, int symbpos, uint32_t clr, uint32_t bkgclr);
void display_splash_screen();

#ifdef __cplusplus
}
#endif

