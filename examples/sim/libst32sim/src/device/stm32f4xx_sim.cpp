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

#include <stm32f4xx_map.h>
#include "stm32f4xx_sim.h"
#include "cansim.h"
#include "gpiosim.h"
#include "nvicsim.h"
#include "pccsim.h"
#include "scbsim.h"
#include "rccsim.h"
#include "syscfgsim.h"
#include "uartsim.h"
#include "wdogsim.h"
#include "systicksim.h"
#include "extisim.h"
#include "adcsim.h"
#include "timsim.h"
#include "rtcsim.h"
#include "spisim.h"

typedef unsigned (__stdcall* fw_thread_type)(void *args);


ST32F4xxSim::ST32F4xxSim(const char *name) :
    DeviceGeneric(name, 0, 0xFFFFFFFFul) {
    devlist_.push_back(new SysTickSim("stck", SysTick_BASE, sizeof(SysTick_registers_type)));
    devlist_.push_back(new CANSim("can1", CAN1_BASE, sizeof(CAN_registers_type)));
    devlist_.push_back(new CANSim("can2", CAN2_BASE, sizeof(CAN_registers_type)));
    devlist_.push_back(new SpiSim("spi3", SPI3_BASE, sizeof(SPI_registers_type), 51));
    devlist_.push_back(new GPIOSim("pa", GPIOA_BASE, sizeof(GPIO_registers_type)));
    devlist_.push_back(new GPIOSim("pb", GPIOB_BASE, sizeof(GPIO_registers_type)));
    devlist_.push_back(new GPIOSim("pc", GPIOC_BASE, sizeof(GPIO_registers_type)));
    devlist_.push_back(new GPIOSim("pd", GPIOD_BASE, sizeof(GPIO_registers_type)));
    devlist_.push_back(new GPIOSim("pe", GPIOE_BASE, sizeof(GPIO_registers_type)));
    devlist_.push_back(new GPIOSim("pf", GPIOF_BASE, sizeof(GPIO_registers_type)));
    devlist_.push_back(nvic_ = new NVICSim("nvic", NVIC_BASE, sizeof(NVIC_registers_type)));
    devlist_.push_back(new UARTSim("uart1", USART1_BASE, sizeof(USART_registers_type), 37));
    devlist_.push_back(new UARTSim("uart2", USART2_BASE, sizeof(USART_registers_type), 38));
    devlist_.push_back(new TimSim("tim2", TIM2_BASE, sizeof(TIM_registers_type), 28));
    devlist_.push_back(new TimSim("tim3", TIM3_BASE, sizeof(TIM_registers_type), 29));
    devlist_.push_back(new RtcSim("rtc", RTC_BASE, sizeof(RTC_registers_type), 3));
    devlist_.push_back(new SCBSim("scb", SCB_BASE, sizeof(SCB_registers_type)));
    devlist_.push_back(new RCCSim("rcc", RCC_BASE, sizeof(RCC_registers_type)));
    devlist_.push_back(new SYSCFGSim("syscfg", SYSCFG_BASE, sizeof(SYSCFG_registers_type)));
    devlist_.push_back(new EXTISim("exti", EXTI_BASE, sizeof(EXTI_registers_type)));
    devlist_.push_back(new AdcSim("adc", ADC1_BASE, 0x400));       // include ADC1,2,3 and common registers
    //devlist_.push_back(new WDOGSim("wdog", WWDG_BASE, sizeof(WDG_registers_type)));

    for (auto it = devlist_.begin(); it != devlist_.end(); it++) {
        (*it)->init();
        addMap(static_cast<ISimMemoryInterface *>(*it));
    }
    nmi_request_ = 0;
    hFwThread_ = 0;
    memset(vector_, 0, sizeof(vector_));
    eventIsrAsync_ = CreateEventW(NULL, TRUE, FALSE, L"eventIsrAsync");

}

void ST32F4xxSim::runFirmware(void *fw) {
    fw_thread_type fw_thread = (fw_thread_type)fw;
    hFwThread_ = (HANDLE)_beginthreadex(0, 0, fw_thread, NULL, 0, 0);
    enabled_ = true;

    while (enabled_) {
        // Interrupt can make interval lesser than 1 ms
        ResetEvent(eventIsrAsync_);
        WaitForSingleObject(eventIsrAsync_, 1);

        // Simulate single core CPU:
        SuspendThread(hFwThread_);
        for (auto it = devlist_.begin(); it != devlist_.end(); it++) {
            (*it)->update(0.0005);
        }
        handleInterrupts();

        ResumeThread(hFwThread_);
        utils_update();
    }
}

void ST32F4xxSim::stopFirmware() {
    enabled_ = false;
    if (hFwThread_) {
        WaitForSingleObject(hFwThread_, 50000);
    }
    hFwThread_ = 0;
}

void ST32F4xxSim::registerIsr(int idx, isr_type handler) {
    if (idx < 0 && idx > -16) {
        vector_[idx + Nmi_Total] = handler;
    } else if (idx < Interrupt_Total) {
        vector_[idx + Nmi_Total] = handler;
    } else {
        printf("wrong ISR index %d\n", idx);
    }
}

void ST32F4xxSim::requestIrq(int idx) {
    if (idx > -16 && idx < 0) {
        nmi_request_ |= 1 << (idx + 15);
        SetEvent(eventIsrAsync_);
    } else if (nvic_->requestIrq(idx)) {
        SetEvent(eventIsrAsync_);
    }
}

void ST32F4xxSim::handleInterrupts() {
    int idx = 0;
    while (nmi_request_) {
        if (nmi_request_ & 0x1) {
            vector_[idx]();
        }
        nmi_request_ >>= 1;
        idx++;
    }
    while ((idx = nvic_->nextPendingIrq()) >= 0) {
        if (vector_[idx + Nmi_Total]) {
            vector_[idx + Nmi_Total]();
        }
    }
}

int ST32F4xxSim::setOption(int optname, void *optval) {
    int param1 = *reinterpret_cast<int *>(optval);
    switch (optname)
    {
    case SIM_OPT_VXL_DRIVER:
        for (auto it = devlist_.begin(); it != devlist_.end(); it++) {
            if (strcmp((*it)->deviceName(), "can0") != 0) {
                continue;
            }
            if (param1) {
                //static_cast<CANSim *>(*it)->enableVxlDriver();
            }
        }
        break;
    default:;
    }
    return 0;
}
