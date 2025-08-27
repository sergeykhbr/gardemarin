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

//#define USE_UART2_TX_AS_INJECTOR

// Error injector:
//    26 PA3 ADC123_IN3 CAN SOF listener on 81 CAN1_RX (PA[3] mapped to EXTI3)
//    23 PA0 ADC123_IN0 error injector on 82 CAN1_TX (TIM5_CH1)
//    86 PD5 temporary error injector UART2_TX
static const gpio_pin_type gpio_cfg_sof_ = {
    (GPIO_registers_type *)GPIOA_BASE, 3
};

static const gpio_pin_type gpio_cfg_injector_ = {
#ifdef USE_UART2_TX_AS_INJECTOR
//    (GPIO_registers_type *)GPIOD_BASE, 5
    (GPIO_registers_type *)GPIOA_BASE, 0
#else
    (GPIO_registers_type *)GPIOD_BASE, 1        //    82 PD1  CAN1_TX    AF9
#endif
};


// To reducde IRQ latency sue global variables instead of interface functions:
CanInjectorDriver *iinjector_ = 0;
IrqHandlerInterface *drv_can_injector_exti_ = 0;
IrqHandlerInterface *drv_can_injector_tim_ = 0;

// Called from CAN1 driver Rx irq handler
extern "C" void CAN1_EndOfFrame() {
    if (iinjector_) {
        iinjector_->CAN1_eof();
    }
}

// Called from CAN2 driver Rx irq handler
extern "C" void CAN2_EndOfFrame() {
    if (iinjector_) {
        iinjector_->CAN2_eof();
    }
}

CanInjectorDriver::CanInjectorDriver(const char *name)
    : FwObject(name),
    injectAction_(this, "inject"),
    injectCnt_(this, "cnt"),
    injectState_(this, "state"),
    baud_cnt_("baud_cnt"),
    can1_util_(this, "can1_util"),
    timerStrobHandler_(this),
    state_(0),
    can1_sof_time_(0) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SYSCFG_registers_type *SYSCFG = (SYSCFG_registers_type *)SYSCFG_BASE;
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    TIM_registers_type *TIM4 = (TIM_registers_type *)TIM4_BASE;     // 16-bit counter
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;   // 32-bit counter
    uint32_t t1;

    // adc clock on APB1 = 144/4 = 36 MHz
    t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 3);             // APB1[3] TIM5EN
    t1 |= (1 << 2);             // APB1[2] TIM4EN
    write32(&RCC->APB1ENR, t1);

    t1 = read32(&RCC->APB2ENR);
    t1 |= 1 << 14;            // APB2[14] SYSCFGEN includes EXTI control registers
    write32(&RCC->APB2ENR, t1);


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

    // TIM4: is used to form inject strob after SOF triggered
    write32(&TIM4->CR1.val, 0);         // stop counter
    // time scale 100 nsec, CAN symbol 2 us = 20 ticks
    write16(&TIM4->PSC, system_clock_hz() / 2 / 10000000 - 1);             // prescaler = 1: CK_CNT = (F_ck_psc/(PSC+1))
    write16(&TIM4->DIER, 1);            // [0] UIE - update interrupt enabled
    nvic_irq_enable(30, 1); // 30 TIM4; 50 TIM5

    // TIM5: is used to calculated Bus Utilization. 500 kHz.
    write32(&TIM5->CR1.val, 0);         // stop counter
    write16(&TIM5->PSC, 71);            // prescaler = 1: CK_CNT = (F_ck_psc/(PSC+1)). (36 MHz / 71+2) = 500 kHz
    write16(&TIM5->DIER, 0);            // no interrupts
    write32(&TIM5->CNT, 0);
    tim_cr1_reg_type cr1;
    cr1.val = 0;
    cr1.bits.CEN = 1;
    write32(&TIM5->CR1.val, cr1.val);   // upcounter, do not stop on event
}

void CanInjectorDriver::Init() {
    iinjector_ = this;
    drv_can_injector_exti_ = static_cast<IrqHandlerInterface *>(this);
    drv_can_injector_tim_ = static_cast<IrqHandlerInterface *>(&timerStrobHandler_);

    RegisterAttribute(&injectAction_);
    RegisterAttribute(&injectCnt_);
    RegisterAttribute(&injectState_);
    RegisterAttribute(&baud_cnt_);
    RegisterAttribute(&can1_util_);
}

// Called from CAN driver on Rx interrupt
void CanInjectorDriver::CAN1_eof() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    write32(&EXTI->PR, 1 << gpio_cfg_sof_.pinidx);   // Pending register, cleared by programming it to 1
    nvic_irq_clear(9);
    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(9, 1);

    timerStrobHandler_.injectRestart();

    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;
    uint32_t cnt = read32(&TIM5->CNT);
    can1_util_.setData(cnt - can1_sof_time_, cnt);
    write32(&TIM5->CNT, 0);
}

void CanInjectorDriver::CAN2_eof() {
}

// Called by EXTI: SOF interrupt handler
void CanInjectorDriver::handleInterrupt(int *argv) {
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;
    if (argv == 0) {
        timerStrobHandler_.handleInterrupt(0);
        can1_sof_time_ = read32(&TIM5->CNT);
    } else {
        // todo: can2 support
    }
}

// Called from TIM4 event
void CanInjectorDriver::TimerStrobHandler::handleInterrupt(int *argv) {
    TIM_registers_type *TIM = (TIM_registers_type *)TIM4_BASE;
    switch (state_) {
    case State_WaitSof:
        // Select 5 symbol to atack.
        //      Initial delay SOF -> here 1.2 us    (12 ticks)
        //      symbol_number * 20 = 200 (symbol_number = 40)
        write32(&TIM->ARR, 1972u);               // TIM4: 16-bit; 812 = 40-th symbol
        write32(&TIM->CNT, 1972u);               // TIM4: 16-bit; 812 = 40-th symbol
        write32(&TIM->CR1.val, cr1_run_.val);
        state_ = State_WaitCanSymbol;
        break;
    case State_WaitCanSymbol:
        // Corrupt 5 CAN symbols
        parent_->setInjectBit();
        write32(&TIM->ARR, 20u);       // 100 = 5 corrupt symbols
        write32(&TIM->CNT, 20u);       // 100 = 5 corrupt symbols
        write32(&TIM->CR1.val, cr1_run_.val);
        state_ = State_Injection;
        break;
    case State_Injection:
        write32(&TIM->CR1.val, 0);         // stop counter
        parent_->releaseInjectBit();
        if (inject_ena_) {
            state_ = State_WaitSof;
            injectCnt_++;
        } else {
            state_ = State_Idle;
        }
        break;
    default:
        state_ = State_Idle;
        write32(&TIM->CR1.val, 0);         // stop counter
    }
}

void CanInjectorDriver::TimerStrobHandler::injectEnable() {
    inject_ena_ = true;
    state_ = State_WaitSof;
}

void CanInjectorDriver::TimerStrobHandler::injectRestart() {
    if (!inject_ena_ || state_ == State_WaitSof) {
        return;
    }
    TIM_registers_type *TIM = (TIM_registers_type *)TIM4_BASE;
    write32(&TIM->CR1.val, 0);         // stop counter
    parent_->releaseInjectBit();
    state_ = State_WaitSof;
}

void CanInjectorDriver::setInjectBit() {
    gpio_pin_as_output(&gpio_cfg_injector_,
                        GPIO_NO_OPEN_DRAIN,
                        GPIO_FAST,
                        GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&gpio_cfg_injector_);
}

void CanInjectorDriver::releaseInjectBit() {
    gpio_pin_set(&gpio_cfg_injector_);
#ifdef USE_UART2_TX_AS_INJECTOR
    // Test separate pin
    gpio_pin_as_input(&gpio_cfg_injector_,
                      GPIO_SLOW,
                      GPIO_NO_PUSH_PULL);
#else
    gpio_pin_as_alternate(&gpio_cfg_injector_, 9);
#endif

}

void CanInjectorDriver::injectionEnable() {
    timerStrobHandler_.injectEnable();
}

void CanInjectorDriver::injectionDisable() {
    timerStrobHandler_.injectDisable();
}

void CanInjectorDriver::InjectErrorAction::post_write() {
    if (u_.u8) {
        parent_->injectionEnable();
    } else {
        parent_->injectionDisable();
    }
}

void CanInjectorDriver::BusUtilizationAttribute::pre_read() {
    if (total_) {
        u_.f = 100.0f * static_cast<float>(busy_) / total_;
    }
    
    busy_ = 0;
    total_ = 0;
}
