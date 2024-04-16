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

#include <s32k148regs.h>
#include "s32k148sim.h"
#include "cansim.h"
#include "gpiosim.h"
#include "lpitsim.h"
#include "nvicsim.h"
#include "pccsim.h"
#include "portsim.h"
#include "scbsim.h"
#include "scgsim.h"
#include "uartsim.h"
#include "wdogsim.h"

typedef unsigned (__stdcall* fw_thread_type)(void *args);


S32K148Sim::S32K148Sim(const char *name) :
    DeviceGeneric(name, 0, 0xFFFFFFFFul) {
    devlist_.push_back(new CANSim("can0", CAN0_BASE, sizeof(CAN_Type)));
    devlist_.push_back(new GPIOSim("pta", PTA_BASE, sizeof(GPIO_Type)));
    devlist_.push_back(new GPIOSim("ptc", PTC_BASE, sizeof(GPIO_Type)));
    devlist_.push_back(new GPIOSim("ptd", PTD_BASE, sizeof(GPIO_Type)));
    devlist_.push_back(new GPIOSim("pte", PTE_BASE, sizeof(GPIO_Type)));
    devlist_.push_back(new LPITSim("lpit0", LPIT0_BASE, sizeof(LPIT_Type)));
    devlist_.push_back(nvic_ = new NVICSim("s32_nvic", S32_NVIC_BASE, sizeof(S32_NVIC_Type)));
    devlist_.push_back(new PCCSim("pcc", PCC_BASE, sizeof(PCC_Type)));
    devlist_.push_back(new PORTSim("porta", PORTA_BASE, sizeof(PORT_Type)));
    devlist_.push_back(new PORTSim("portc", PORTC_BASE, sizeof(PORT_Type)));
    devlist_.push_back(new PORTSim("portd", PORTD_BASE, sizeof(PORT_Type)));
    devlist_.push_back(new PORTSim("porte", PORTE_BASE, sizeof(PORT_Type)));
    devlist_.push_back(new SCBSim("s32_scb", S32_SCB_BASE, sizeof(S32_SCB_Type)));
    devlist_.push_back(new SCGSim("scg", SCG_BASE, sizeof(SCG_Type)));
    devlist_.push_back(new UARTSim("uart1", LPUART1_BASE, sizeof(LPUART_Type)));
    devlist_.push_back(new WDOGSim("wdog", WDOG_BASE, sizeof(WDOG_Type)));

    for (auto it = devlist_.begin(); it != devlist_.end(); it++) {
        (*it)->init();
        addMap(static_cast<ISimMemoryInterface *>(*it));
    }
    hFwThread_ = 0;
    memset(vector_, 0, sizeof(vector_));
    eventIsrAsync_ = CreateEventW(NULL, TRUE, FALSE, L"eventIsrAsync");

}

void S32K148Sim::runFirmware(void *fw) {
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
            (*it)->update(0.001);
        }
        handleInterrupts();

        ResumeThread(hFwThread_);
        utils_update();
    }
}

void S32K148Sim::stopFirmware() {
    enabled_ = false;
    if (hFwThread_) {
        WaitForSingleObject(hFwThread_, 50000);
    }
    hFwThread_ = 0;
}

void S32K148Sim::registerIsr(int idx, isr_type handler) {
    if (idx < 0 || idx >= Interrupt_Total) {
        printf("wrong ISR index %d\n", idx);
        return;
    }
    vector_[idx] = handler;
}

void S32K148Sim::requestIrq(int idx) {
    if (idx < 0) {
        // TODO: unmaskable interrupts
    } else if (nvic_->requestIrq(idx)) {
        SetEvent(eventIsrAsync_);
    }
}

void S32K148Sim::handleInterrupts() {
    int idx;
    while ((idx = nvic_->nextPendingIrq()) >= 0) {
        vector_[idx]();
    }
}

int S32K148Sim::setOption(int optname, void *optval) {
    int param1 = *reinterpret_cast<int *>(optval);
    switch (optname)
    {
    case S32K148_OPT_VXL_DRIVER:
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
