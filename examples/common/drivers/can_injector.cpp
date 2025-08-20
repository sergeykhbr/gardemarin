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
#include <string.h>
#include <stdio.h>
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include <uart.h>
#include <gpio_drv.h>
#include "can_injector.h"


// Error injector:
//    26 PA3 ADC123_IN3 CAN SOF listener on 81 CAN1_RX (PA[3] mapped to EXTI3)
//    23 PA0 ADC123_IN0 error injector on 82 CAN1_TX (TIM5_CH1)
//    86 PD5 temporary error injector UART2_TX
static const gpio_pin_type gpio_cfg_sof_ = {
    (GPIO_registers_type *)GPIOA_BASE, 3
    //(GPIO_registers_type *)GPIOA_BASE, 0
};

static const gpio_pin_type gpio_cfg_injector_ = {
    //(GPIO_registers_type *)GPIOA_BASE, 0
    (GPIO_registers_type *)GPIOD_BASE, 5
    //(GPIO_registers_type *)GPIOD_BASE, 1        //    82 PD1  CAN1_TX    AF9
};


IrqHandlerInterface *iinjector_irq_ = 0;

// Irq[9] EXTI3
extern "C" void EXTI3_CanSofListener_IRQHandler() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    if (iinjector_irq_) {
        iinjector_irq_->handleInterrupt(0);
    }
    nvic_irq_disable(9);
}

extern "C" void CAN_SOF_set_wait_sync() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    write32(&EXTI->PR, 1 << gpio_cfg_sof_.pinidx);   // Pending register, cleared by programming it to 1
    nvic_irq_clear(9);

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(9, 1);
}


CanInjectorDriver::CanInjectorDriver(const char *name)
    : FwObject(name),
    inject_(this, "inject"),
    state_(0) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SYSCFG_registers_type *SYSCFG = (SYSCFG_registers_type *)SYSCFG_BASE;
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    uint32_t t1;

    t1 = read32(&RCC->APB2ENR);
    t1 |= 1 << 14;            // APB2[14] SYSCFGEN includes EXTI control registers
    write32(&RCC->APB2ENR, t1);


    /*gpio_pin_as_input(&gpio_cfg_injector_,
                        GPIO_FAST,
                        GPIO_NO_PUSH_PULL);*/

    gpio_pin_as_input(&gpio_cfg_sof_,
                        GPIO_FAST,
                        GPIO_NO_PUSH_PULL);


    // connect EXTI3 request to port PA
    t1 = read32(&SYSCFG->EXTICR[gpio_cfg_sof_.pinidx >> 2]);
    t1 &= ~(0xF << (4*gpio_cfg_sof_.pinidx));
    t1 |= 0x0;    // [15:12] EXTI3[3:0] 0000=PA; 0001=PB; 0010=PC;..
    write32(&SYSCFG->EXTICR[gpio_cfg_sof_.pinidx >> 2], t1);


    write32(&EXTI->RTSR, read32(&EXTI->RTSR) & ~(1<< gpio_cfg_sof_.pinidx));    // Clear Rising trigger selection
    write32(&EXTI->FTSR, read32(&EXTI->FTSR) | (1 << gpio_cfg_sof_.pinidx));    // Falling trigger selection
    write32(&EXTI->IMR, read32(&EXTI->IMR) | (1 << gpio_cfg_sof_.pinidx));     // Interrupt mask: 0 line is masked; 1=unmasked

    write32(&EXTI->PR, 1 << gpio_cfg_sof_.pinidx);   // Pending register, cleared by programming it to 1

}

void CanInjectorDriver::Init() {
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    iinjector_irq_ = static_cast<IrqHandlerInterface *>(this);
    RegisterAttribute(&inject_);
}

void CanInjectorDriver::handleInterrupt(int *state) {
    // inversed, if LOW btn is pressed
    /*if (state_) {
        setInjection();
    } else {
        releaseInjection();
    }/
    state_ = !state_;*/

    setInjection();
    uart_printk("!\r\n");
    releaseInjection();
}

void CanInjectorDriver::setInjection() {
    gpio_pin_as_output(&gpio_cfg_injector_,
                        GPIO_NO_OPEN_DRAIN,
                        GPIO_FAST,
                        GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&gpio_cfg_injector_);
}

void CanInjectorDriver::releaseInjection() {
    gpio_pin_set(&gpio_cfg_injector_);
#if 1
    // Test separate pin
#else
    gpio_pin_as_alternate(&gpio_cfg_injector_, 9);
#endif
}

void CanInjectorDriver::InjectError::post_write() {
    if (u_.u8) {
        parent_->setInjection();
        uart_printf("set_inject\r\n");
    } else {
        parent_->releaseInjection();
        uart_printf("release_inject\r\n");
    }
}
