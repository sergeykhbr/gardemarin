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

#include <prjtypes.h>
#include "system_handlers.h"
#include <prjtypes.h>
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include <uart.h>
#include <spi.h>
#include <display_spi.h>

extern IrqHandlerInterface *drv_display_spi_tx_;

extern "C" void CanMonitor_SPI3_irq_handler() {
    /*SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;
    if (drv_display_spi_tx_) {
        drv_display_spi_tx_->handleInterrupt(0);
    }*/
}

extern "C" void CanMonitor_TIM5_irq_handler() {
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;
    int nsec = 0;
    if (drv_display_spi_tx_) {
        drv_display_spi_tx_->handleInterrupt(&nsec);
    }

    write16(&TIM5->SR, 0);  // clear all pending bits
    nvic_irq_clear(50);
}
