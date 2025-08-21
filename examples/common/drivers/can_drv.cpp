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

// Defined in CAN injector (TODO: change on interface or attribute)
extern "C" void CAN_SOF_set_wait_sync();

extern "C" void CAN1_FIFO0_irq_handler() {
    CAN_SOF_set_wait_sync();

    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("can1", "IrqHandlerInterface"));
    if (iface) {
        int fifoid = 0;
        iface->handleInterrupt(&fifoid);
    }
    nvic_irq_clear(20);
}

extern "C" void CAN1_FIFO1_irq_handler() {
    CAN_SOF_set_wait_sync();

    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("can1", "IrqHandlerInterface"));
    if (iface) {
        int fifoid = 1;
        iface->handleInterrupt(&fifoid);
    }
    nvic_irq_clear(21);
}

extern "C" void CAN1_SCE_irq_handler() {
    CAN_registers_type *dev = (CAN_registers_type *)CAN1_BASE;
    // [6:4] LEC. Last Error Code
    //      000 No error
    //      001 Stuff error
    //      010 Form error
    //      011 Ack error
    //      100 Bit recessive error
    //      101 Bit dominant error
    //      110 CRC error
    //      11 Set by software
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute("can1", "errcnt"));
    if (atr) {
        atr->make_uint32(atr->to_uint32() + 1);
    } else {
        uart_printk("err %d\r\n", __LINE__);
    }
    // [2] ERRI error interrupt.
    write32(&dev->MCR.val, 1 << 2);
    nvic_irq_clear(22);
}

extern "C" void CAN2_FIFO0_irq_handler() {
    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("can2", "IrqHandlerInterface"));
    if (iface) {
        int fifoid = 0;
        iface->handleInterrupt(&fifoid);
    }
    nvic_irq_clear(64);
}

extern "C" void CAN2_FIFO1_irq_handler() {
    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("can2", "IrqHandlerInterface"));
    if (iface) {
        int fifoid = 1;
        iface->handleInterrupt(&fifoid);
    }
    nvic_irq_clear(65);
}
extern "C" void CAN2_SCE_irq_handler() {
    CAN_registers_type *dev = (CAN_registers_type *)CAN2_BASE;
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute("can2", "errcnt"));
    if (atr) {
        atr->make_uint32(atr->to_uint32() + 1);
    } else {
        uart_printk("err %d\r\n", __LINE__);
    }
    // [2] ERRI error interrupt.
    write32(&dev->MCR.val, 1 << 2);
    nvic_irq_clear(66);
}


CanDriver::CanDriver(const char *name, int busid) : FwObject(name),
    baudrate_("baudrate"),
    mode_("mode"),
    rxcnt_("rxcnt"),
    errcnt_("errcnt"),
    test1_("test1"),
    busid_(busid) {
    rxframe_rcnt = 0;
    rxframe_wcnt = 1;

    mode_.make_int8(0);
    test1_.make_uint32(0xcafef00d);

    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    uint32_t t1;

    if (busid == 0) {
        dev_ = (CAN_registers_type *)CAN1_BASE;

        //    81 PD0  CAN1_RX    AF9
        //    82 PD1  CAN1_TX    AF9
        gpio_cfg_tx_.port = (GPIO_registers_type *)GPIOD_BASE;
        gpio_cfg_tx_.pinidx = 1;
        gpio_pin_as_alternate(&gpio_cfg_tx_, 9);

        gpio_cfg_rx_.port = (GPIO_registers_type *)GPIOD_BASE;
        gpio_cfg_rx_.pinidx = 0;
        gpio_pin_as_alternate(&gpio_cfg_rx_, 9);
    } else {
        dev_ = (CAN_registers_type *)CAN2_BASE;

        //    91 PB5  CAN2_RX    AF9
        //    92 PB6  CAN2_TX    AF9
        gpio_cfg_tx_.port = (GPIO_registers_type *)GPIOB_BASE;
        gpio_cfg_tx_.pinidx = 5;
        gpio_pin_as_alternate(&gpio_cfg_tx_, 9);

        gpio_cfg_rx_.port = (GPIO_registers_type *)GPIOB_BASE;
        gpio_cfg_rx_.pinidx = 6;
        gpio_pin_as_alternate(&gpio_cfg_rx_, 9);
    }

    // Enable clock CAN1 and CAN2
    t1 = read32(&RCC->APB1ENR);
    t1 |= (0x3 << 25);            // APB1[26] CAN2 EN; APB1[25] CAN1 EN
    write32(&RCC->APB1ENR, t1);


    SetBaudrated(500000);

    // prio: 0 highest; 7 is lowest
    if (busid == 0) {
        nvic_irq_enable(20, 3);   // CAN1_RX0
        nvic_irq_enable(21, 3);   // CAN1_RX1
        nvic_irq_enable(22, 3);   // CAN1_SCE
    } else {
        nvic_irq_enable(64, 3);   // CAN2_RX0
        nvic_irq_enable(65, 3);   // CAN2_RX1
        nvic_irq_enable(66, 3);   // CAN2_SCE
    }
    StartListenerMode();
}

void CanDriver::Init() {
    RegisterInterface(static_cast<RunInterface *>(this));
    RegisterInterface(static_cast<CanInterface *>(this));
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterAttribute(&baudrate_);
    RegisterAttribute(&mode_);
    RegisterAttribute(&rxcnt_);
    RegisterAttribute(&errcnt_);
    RegisterAttribute(&test1_);
}

void CanDriver::PostInit() {
}

void CanDriver::SetBaudrated(uint32_t baud) {
    CAN_MCR_type mcr;
    CAN_MSR_type msr;
    CAN_BTR_type btr;

    mcr.val = 0;
    mcr.b.INRQ = 1;
    write32(&dev_->MCR.val, mcr.val);

    // wait entering initialzation stage
    do {
        msr.val = read32(&dev_->MSR.val);
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
    write32(&dev_->BTR.val, btr.val);

    // Switch to Normal mode
    mcr.val = 0;
    write32(&dev_->MCR.val, mcr.val);

    baudrate_.make_int32(baud);
}

uint32_t CanDriver::hwid2canid(uint32_t hwid) {
    uint32_t ret = (hwid >> 3) & 0x3FFFF;  // EXID[17:0]
    ret = (ret << 11) | (hwid >> 18);
    return ret;
}

void CanDriver::handleInterrupt(int *argv) {
    CAN_RF_type rf;
    can_frame_type *f;
    int fifoidx = argv[0];

    do {
        f = &rxframes[rxframe_wcnt];
        // TODO fifo: is full
        if (rxframe_wcnt != rxframe_rcnt) {
            if (++rxframe_wcnt >= CAN_RX_FRAMES_MAX) {
                rxframe_wcnt = 0;
            }
        }
        
        f->busid = busid_;
        f->id = read32(&dev_->sFIFOMailBox[fifoidx].RIR);
        f->id = hwid2canid(f->id);

        f->timestamp = read32(&dev_->sFIFOMailBox[fifoidx].RDTR);
        f->dlc = (uint8_t)(f->timestamp & 0xF);
        f->timestamp >>= 16;

        f->data.u32[0] = read32(&dev_->sFIFOMailBox[fifoidx].RDLR);
        f->data.u32[1] = read32(&dev_->sFIFOMailBox[fifoidx].RDHR);

        rf.val = 0;
        rf.b.RFOM = 1;   // release bit to decrement pending messages
        write32(&dev_->RF[fifoidx].val, rf.val);

        rf.val = read32(&dev_->RF[fifoidx].val);
        rxcnt_.make_uint32(rxcnt_.to_uint32() + 1);
#if 0
if (busid_ == 0) {
    //          static const uint32 ID =  (uint32) (0x1F00601FUL | Utilities::BRC_NR | (static_cast<uint32>(Utilities::COM5008_DEVICE)<<5));
    //if ((f->id & 0x1f00001f) == 0x1f00001f)
    {
        // 1d441147 = 1_1101_0100_0100_0001_0001_0100_0111
        //                                          0_
        uart_printk("%08x\r\n", f->id);
    }
}
#endif
    } while (rf.b.FMP != 0);
}

// Total 18 filters available
void CanDriver::StartListenerMode() {
    uint32_t t1;

    // Registers 0x200... exist only in CAN1
    if (busid_ == 0) {
        // [13:8] CAN2FSB: 0=no assigned filters to CAN1, 28=all filters assigned to CAN1
        // [0] FINIT
        write32(&dev_->FMR, (14 << 8) | 1);          // Assign filters[0..13] to CAN1; [14..27] to CAN2
        write32(&dev_->FM1R, 0);                     // all filters in Mask mode=0; 1=List mode
        write32(&dev_->FFA1R, 0x3f80 | (0x3f80 << 14));               // 0 Fillter assigned to FIFO0; 1 assigned to FIFO1

        write32(&dev_->sFilterRegister[0].FR1, 0);   // identifier
        write32(&dev_->sFilterRegister[0].FR2, 0);   // mask in mask mode
        write32(&dev_->sFilterRegister[14].FR1, 0);   // identifier
        write32(&dev_->sFilterRegister[14].FR2, 0);   // mask in mask mode

        write32(&dev_->FA1R, (1 << 0) | (1 << 14));   // [0] FACT0: activate filter 0 (or 14 for CAN1)

        t1 = read32(&dev_->FMR);
        t1 &= ~1;   // FINIT = 0
        write32(&dev_->FMR, t1);
    }


    // Enable Rx interrupts in FIFO0 and FIFO1
    setRun();
    mode_.make_int8(2);
}

    // Enable Rx interrupts in FIFO0 and FIFO1
void CanDriver::setRun() {
    CAN_IER_type ier;

    ier.val = 0;
    ier.b.FMPIE0 = 1;
    ier.b.FMPIE1 = 1;
    ier.b.ERRIE = 1;
    write32(&dev_->IER.val, ier.val);
}

void CanDriver::setStop() {
    write32(&dev_->IER.val, 0);  // disable all interrupts
}

// TODO: fifo
int CanDriver::ReadCanFrame(can_frame_type *frame) {
    int total = rxframe_wcnt - rxframe_rcnt - 1;
    if (total < 0) {
        total += CAN_RX_FRAMES_MAX;
    } else if (total >= CAN_RX_FRAMES_MAX)  {
        total -= CAN_RX_FRAMES_MAX;
    }
    
    if (total == 0) {
        return 0;
    }
    if (++rxframe_rcnt >= CAN_RX_FRAMES_MAX) {
        rxframe_rcnt = 0;
    }
    *frame = rxframes[rxframe_rcnt];
    return 1;
}
