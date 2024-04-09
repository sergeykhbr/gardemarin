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
#include <fwapi.h>
#include <uart.h>
#include "can_drv.h"

uint32_t hwid2canid(uint32_t hwid) {
    uint32_t ret = (hwid >> 3) & 0x3FFFF;  // EXID[17:0]
    ret = (ret << 11) | (hwid >> 18);
    return ret;
}

/** Generic interrupt handler
 */
extern "C" void can_rx_generic_handler(int busid,
                            int fifoidx,
                            int irqidx) {
    can_type *p = (can_type *)fw_get_ram_data(CAN_DRV_NAME);
    can_bus_type *bus = &p->bus[busid];
    CAN_registers_type *dev = bus->dev;
    CAN_RF_type rf;

    do {
        if (p->rx_frame_cnt < CAN_RX_FRAMES_MAX) {
            can_frame_type *f = &p->rxframes[p->rx_frame_cnt++];
            f->busid = busid;
            f->id = read32(&dev->sFIFOMailBox[fifoidx].RIR);
            f->id = hwid2canid(f->id);

            f->timestamp = read32(&dev->sFIFOMailBox[fifoidx].RDTR);
            f->dlc = (uint8_t)(f->timestamp & 0xF);
            f->timestamp >>= 16;

            f->data.u32[0] = read32(&dev->sFIFOMailBox[fifoidx].RDLR);
            f->data.u32[1] = read32(&dev->sFIFOMailBox[fifoidx].RDHR);
        }

        rf.val = 0;
        rf.b.RFOM = 1;   // release bit to decrement pending messages
        write32(&dev->RF[fifoidx].val, rf.val);

        rf.val = read32(&dev->RF[fifoidx].val);
    } while (rf.b.FMP != 0);

    nvic_irq_clear(irqidx);
}

extern "C" void CAN1_FIFO0_irq_handler() {
    can_rx_generic_handler(0, 0, 20);
}

extern "C" void CAN1_FIFO1_irq_handler() {
    can_rx_generic_handler(0, 1, 21);
}

extern "C" void CAN2_FIFO0_irq_handler() {
    can_rx_generic_handler(1, 0, 64);
}

extern "C" void CAN2_FIFO1_irq_handler() {
    can_rx_generic_handler(1, 1, 65);
}


void can_set_baudrated(can_type *p, int busid, uint32_t baud) {
    CAN_registers_type *dev = p->bus[busid].dev;
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

// Total 18 filters available
extern "C" void can_bus_listener_start(can_type *p, int busid) {
    CAN_registers_type *dev = p->bus[busid].dev;
    CAN_IER_type ier;
    uint32_t t1;

    // [13:8] CAN2FSB: 0=no assigned filters to CAN1, 28=all filters assigned to CAN1
    // [0] FINIT
    write32(&dev->FMR, (14 << 8) | 1);          // Assign filters[0..13] to CAN1; [14..27] to CAN2
    write32(&dev->FM1R, 0);                     // all filters in Mask mode=0; 1=List mode
    write32(&dev->FFA1R, 0x3f80 << (14*busid)); // 0 Filleter assigned to FIFO0; 1 assigned to FIFO1

    write32(&dev->sFilterRegister[14*busid].FR1, 0);   // identifier
    write32(&dev->sFilterRegister[14*busid].FR2, 0);   // mask in mask mode

    write32(&dev->FA1R, 1 << (14*busid));   // [0] FACT0: activate filter 0 (or 14 for CAN1)

    t1 = read32(&dev->FMR);
    t1 &= ~1;   // FINIT = 0
    write32(&dev->FMR, t1);


    // Enable Rx interrupts in FIFO0 and FIFO1
    ier.val = 0;
    ier.b.FMPIE0 = 1;
    ier.b.FMPIE1 = 1;
    write32(&dev->IER.val, ier.val);
}

extern "C" void can_bus_listener_stop(can_type *p, int busid) {
    CAN_registers_type *dev = p->bus[busid].dev;
    write32(&dev->IER.val, 0);  // disable all interrupts
}

extern "C" void can_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    can_type *p = (can_type *)fw_get_ram_data(CAN_DRV_NAME);
    uint32_t t1;
    if (p == 0) {
         return;
    }

    p->bus[0].dev = (CAN_registers_type *)CAN1_BASE;
    p->bus[1].dev = (CAN_registers_type *)CAN2_BASE;

    //    81 PD0  CAN1_RX    AF9
    //    82 PD1  CAN1_TX    AF9
    p->bus[0].gpio_cfg_tx.port = (GPIO_registers_type *)GPIOD_BASE;
    p->bus[0].gpio_cfg_tx.pinidx = 1;
    gpio_pin_as_alternate(&p->bus[0].gpio_cfg_tx, 9);

    p->bus[0].gpio_cfg_rx.port = (GPIO_registers_type *)GPIOD_BASE;
    p->bus[0].gpio_cfg_rx.pinidx = 0;
    gpio_pin_as_alternate(&p->bus[0].gpio_cfg_rx, 9);

    //    91 PB5  CAN2_RX    AF9
    //    92 PB6  CAN2_TX    AF9
    p->bus[1].gpio_cfg_tx.port = (GPIO_registers_type *)GPIOB_BASE;
    p->bus[1].gpio_cfg_tx.pinidx = 5;
    gpio_pin_as_alternate(&p->bus[1].gpio_cfg_tx, 9);

    p->bus[1].gpio_cfg_rx.port = (GPIO_registers_type *)GPIOB_BASE;
    p->bus[1].gpio_cfg_rx.pinidx = 6;
    gpio_pin_as_alternate(&p->bus[1].gpio_cfg_rx, 9);


    // Enable clock CAN1 and CAN2
    t1 = read32(&RCC->APB1ENR);
    t1 |= (0x3 << 25);            // APB1[26] CAN2 EN; APB1[25] CAN1 EN
    write32(&RCC->APB1ENR, t1);


    can_set_baudrated(p, 0, 500000);
    can_set_baudrated(p, 1, 500000);

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(20, 3);   // CAN1_RX0
    nvic_irq_enable(21, 3);   // CAN1_RX1

    nvic_irq_enable(64, 3);   // CAN2_RX0
    nvic_irq_enable(65, 3);   // CAN2_RX1
}
