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

#include "display_spi.h"


class DisplaySPI_F4xx : public DisplaySPI {
 public:
    DisplaySPI_F4xx(const char *name) : DisplaySPI(name) {}

 protected:
    virtual void init_GPIO();
    virtual void init_SPI();
    virtual SPI_registers_type *spi_bar() { return (SPI_registers_type *)SPI3_BASE; }
};


