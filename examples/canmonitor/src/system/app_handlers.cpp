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
extern IrqHandlerInterface *drv_can_injector_exti_;         // can1 sof
extern IrqHandlerInterface *drv_can_injector_tim_;          // inject error bits

extern "C" void CanMonitor_SPI3_irq_handler() {
    /*SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;
    if (drv_display_spi_tx_) {
        drv_display_spi_tx_->handleInterrupt(0);
    }*/
}

extern "C" void CanMonitor_TIM4_irq_handler() {
    TIM_registers_type *TIM4 = (TIM_registers_type *)TIM4_BASE;
    if (drv_can_injector_tim_) {
        drv_can_injector_tim_->handleInterrupt(0);
    }

    write16(&TIM4->SR, 0);  // clear all pending bits
    nvic_irq_clear(30);
}

// Irq[9] EXTI3 (connectected to CAN1 Rx)
extern "C" void EXTI3_CanSofListener_IRQHandler() {
    if (drv_can_injector_exti_) {
        drv_can_injector_exti_->handleInterrupt(0);
    }
    nvic_irq_disable(9);    // disable EXTI, must be re-enabled by CAN end-of-frame
}
