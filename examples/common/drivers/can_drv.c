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
#include <stm32f4xx_map.h>
#include <memanager.h>
#include <uart.h>
#include "can_drv.h"

void can_set_baudrated(can_type *p, int idx, uint32_t baud) {
    CAN_registers_type *dev = p->ctrl[idx].dev;
    CAN_MCR_type mcr;
    CAN_MSR_type msr;
    CAN_BTR_type btr;

    mcr.val = 0;
    mcr.b.INRQ = 1;
    write32(&dev->MCR.val, mcr.val);

    // wait entering initialzation stage
    do {
        msr.val = read32(&dev->MSR.val);
    } while (msr.b.INAK == 0);

    // Let's make 12 samples per bit (example 500KBaud * 12 = 6MHz)
    uint32_t pclk = (uint32_t)system_clock_hz() / 4;    // APB1: 144/4 = 36 MHz 
    // sync = 1
    // bs1 = 8
    // bs2 = 3
    // sync + bs1 + bs2 = 12 samples per bit
    btr.val = 0;
    btr.b.BRP = pclk / (12 * baud) - 1;
    btr.b.TS1 = 8 - 1;
    btr.b.TS2 = 3 - 1;
    btr.b.SJW = 1 - 1;   // tuning value of BS1/BS2 if the edge detected outside of sync interval
    btr.b.SILM = 1;      // silent mode
    write32(&dev->BTR.val, btr.val);

    // Switch to Normal mode
    mcr.val = 0;
    write32(&dev->MCR.val, mcr.val);
}

void can_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    can_type *p = (can_type *)fw_get_ram_data(CAN_DRV_NAME);
    uint32_t t1;
    if (p == 0) {
         return;
    }

    p->ctrl[0].dev = (CAN_registers_type *)CAN1_BASE;
    p->ctrl[1].dev = (CAN_registers_type *)CAN2_BASE;

    //    PD1  CAN1_TX    AF9
    //    PD0  CAN1_RX    AF9
    p->ctrl[0].gpio_cfg_tx.port = (GPIO_registers_type *)GPIOD_BASE;
    p->ctrl[0].gpio_cfg_tx.pinidx = 1;
    gpio_pin_as_alternate(&p->ctrl[0].gpio_cfg_tx, 9);

    p->ctrl[0].gpio_cfg_rx.port = (GPIO_registers_type *)GPIOD_BASE;
    p->ctrl[0].gpio_cfg_rx.pinidx = 0;
    gpio_pin_as_alternate(&p->ctrl[0].gpio_cfg_rx, 9);

    //    PB6  CAN2_RX    AF9
    //    PB5  CAN2_TX    AF9
    p->ctrl[1].gpio_cfg_tx.port = (GPIO_registers_type *)GPIOB_BASE;
    p->ctrl[1].gpio_cfg_tx.pinidx = 5;
    gpio_pin_as_alternate(&p->ctrl[1].gpio_cfg_tx, 9);

    p->ctrl[1].gpio_cfg_rx.port = (GPIO_registers_type *)GPIOB_BASE;
    p->ctrl[1].gpio_cfg_rx.pinidx = 6;
    gpio_pin_as_alternate(&p->ctrl[1].gpio_cfg_rx, 9);


    // Enable clock CAN1 and CAN2
    t1 = read32(&RCC->APB1ENR);
    t1 |= (0x3 << 25);            // APB1[26] CAN2 EN; APB1[25] CAN1 EN
    write32(&RCC->APB1ENR, t1);


    can_set_baudrated(p, 0, 500000);
}
