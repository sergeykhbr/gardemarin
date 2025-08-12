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
    SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;
    if (drv_display_spi_tx_) {
        drv_display_spi_tx_->handleInterrupt(0);
    }
}

static void startCounter(TIM_registers_type *TIM, int usec) {
    tim_cr1_reg_type cr1;

    write32(&TIM->ARR, static_cast<uint32_t>(usec));
    write32(&TIM->CNT, static_cast<uint32_t>(usec));

    cr1.val = 0;
    cr1.bits.CEN = 1;
    cr1.bits.OPM = 1;   // one pulse mode
    cr1.bits.DIR = 1;   // downcount
    write32(&TIM->CR1.val, cr1.val);
}

extern "C" void CanMonitor_TIM5_irq_handler() {
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;

/*    int nsec = 0;
    if (drivers_) {
        drivers_->handleInterrupt(&nsec);
    }
    // Convert nsec to timescale is 100 ns
    if (nsec) {
        nsec /= 100;
        if (nsec == 0) {
            nsec = 1;       // minimum 100 nsec interval
        }
    }

    write16(&TIM5->SR, 0);  // clear all pending bits
    nvic_irq_clear(50);
    if (nsec) {
        startCounter(TIM5, nsec);
    }*/
}
