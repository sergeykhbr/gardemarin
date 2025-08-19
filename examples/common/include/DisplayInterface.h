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

#include "CommonInterface.h"

class DisplayInterface : public CommonInterface {
 public:
    DisplayInterface() : CommonInterface("DisplayInterface") {}

    virtual void clearScreen() = 0;
    // Font Size 24, total 8 lines, 19 symbols per line
    virtual void outputText24Line(char *str, int linepos, int symbpos, uint32_t clr, uint32_t bkgclr) = 0;
};
