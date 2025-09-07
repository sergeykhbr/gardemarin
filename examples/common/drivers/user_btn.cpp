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
#include <mcu.h>
#include <fwapi.h>
#include <uart.h>
#include <gpio_drv.h>
#include "user_btn.h"

//    PC13 - User btn (internal pull-up). 0=btn is pressed
static const gpio_pin_type ubtn_pin = {
    (GPIO_registers_type *)GPIOC_BASE, 13
};

// EXTI15_10
extern "C" void Btn_IRQHandler() {
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;

    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("ubtn0", "IrqHandlerInterface"));
    if (iface) {
        int state = static_cast<int>(gpio_pin_get(&ubtn_pin));
        iface->handleInterrupt(&state);
    }

    write32(&EXTI->PR, 1 << ubtn_pin.pinidx);   // Pending register, cleared by programming it to 1
    nvic_irq_clear(40);
}


UserButtonDriver::UserButtonDriver(const char *name)
    : FwObject(name),
    event_(0),
    ms_cnt_(0),
    keyPosition_(0),
    keyPositionPrev_(0),
    keyPositionChangedMs_(0),
    listener_(0) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SYSCFG_registers_type *SYSCFG = (SYSCFG_registers_type *)SYSCFG_BASE;
    EXTI_registers_type *EXTI = (EXTI_registers_type *)EXTI_BASE;
    uint32_t t1;

    t1 = read32(&RCC->APB2ENR);
    t1 |= 1 << 14;            // APB2[14] SYSCFGEN includes EXTI control registers
    write32(&RCC->APB2ENR, t1);


    gpio_pin_as_input(&ubtn_pin,
                      GPIO_SLOW,
                      GPIO_PULL_UP);

    // connect EXTI13 request to port PC
    t1 = read32(&SYSCFG->EXTICR[3]);
    t1 &= ~0xF0;
    t1 |= 0x20;    // [7:4] EXIT13[3:0] 0000=PA; 0001=PB; 0010=PC;..
    write32(&SYSCFG->EXTICR[3], t1);


    write32(&EXTI->RTSR, read32(&EXTI->RTSR) | (1 << ubtn_pin.pinidx));    // Rising trigger selection
    write32(&EXTI->FTSR, read32(&EXTI->FTSR) | (1 << ubtn_pin.pinidx));    // Falling trigger selection
    write32(&EXTI->IMR, read32(&EXTI->IMR) | (1 << ubtn_pin.pinidx));     // Interrupt mask: 0 line is masked; 1=unmasked

    write32(&EXTI->PR, 1 << ubtn_pin.pinidx);   // Pending register, cleared by programming it to 1

}

void UserButtonDriver::Init() {
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));
    RegisterInterface(static_cast<KeyInterface *>(this));

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(40, 3);
}

void UserButtonDriver::handleInterrupt(int *state) {
    // inversed, if LOW btn is pressed
    if (*state == 0) {
        if (keyPosition_ == BTN_POS_PRESSED) {
            event_ |= BTN_EVENT_RELEASED;
            event_ |= BTN_EVENT_PRESSED;
        } else {
            event_ |= BTN_EVENT_PRESSED;
        }
        keyPosition_ = BTN_POS_PRESSED;
    } else {
        if (keyPosition_ == BTN_POS_RELEASED) {
            event_ |= BTN_EVENT_PRESSED;
            event_ |= BTN_EVENT_RELEASED;
        } else {
            event_ |= BTN_EVENT_RELEASED;
        }
        keyPosition_ = BTN_POS_RELEASED;
    }
    keyPositionChangedMs_ = ms_cnt_;
}

void UserButtonDriver::registerKeyListener(KeyListenerInterface *iface) {
    FwList *item = reinterpret_cast<FwList *>(fw_malloc(sizeof(FwList)));
    fwlist_set_payload(item, iface);
    fwlist_add(&listener_, item);
}

void UserButtonDriver::callbackTimer(uint64_t tickcnt) {
    ms_cnt_ = tickcnt;
    if ((ms_cnt_ - keyPositionChangedMs_) < 5) {
        return;
    }

    uint32_t ev = event_;
    event_ ^= ev;
#ifdef x_WIN32
if ((tickcnt % 5000) == 4999) {
    ev |= BTN_EVENT_PRESSED; 
}
#endif
    if (ev & BTN_EVENT_PRESSED) {
        FwList *p = listener_;
        KeyListenerInterface *iface;
        while (p) {
            iface = reinterpret_cast<KeyListenerInterface *>(fwlist_get_payload(p));
            iface->keyPressed();
            p = p->next;
        }
    }
    if (ev & BTN_EVENT_RELEASED) {
        FwList *p = listener_;
        KeyListenerInterface *iface;
        while (p) {
            iface = reinterpret_cast<KeyListenerInterface *>(fwlist_get_payload(p));
            iface->keyReleased();
            p = p->next;
        }
    }
}
