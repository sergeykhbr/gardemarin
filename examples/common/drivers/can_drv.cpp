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

/** Instead of using interface, use this gloval var. Bad, but is is faster.
 */
can_msg_history_type msg_history_;


// Defined in CAN injector (TODO: change on interface or attribute)
extern "C" void CAN1_EndOfFrame();
extern "C" void CAN2_EndOfFrame();

extern "C" void CAN1_FIFO0_irq_handler() {
    CAN1_EndOfFrame();

    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("can1", "IrqHandlerInterface"));
    if (iface) {
        int fifoid = 0;
        iface->handleInterrupt(&fifoid);
    }
    nvic_irq_clear(20);
}

extern "C" void CAN1_FIFO1_irq_handler() {
    CAN1_EndOfFrame();

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
    CAN_ESR_type esr;
    char errmsg[5] = "e0\r\n";
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute("can1", "errcnt"));
    // [6:4] LEC. Last Error Code
    //      000 No error
    //      001 Stuff error
    //      010 Form error
    //      011 Ack error
    //      100 Bit recessive error
    //      101 Bit dominant error
    //      110 CRC error
    //      111 Set by software
    // [2] BOFF. Bus off flag
    // [1] EPVF. Error passive flag
    // [0] EWGF. Error warning flag
    if (atr) {
        atr->make_uint32(atr->to_uint32() + 1);
    } else {
        uart_printk("err %d\r\n", __LINE__);
    }
    esr.val = read32(&dev->ESR.val);
    errmsg[1] += static_cast<char>(esr.b.LEC);

    atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute("can1", "lasterr"));
    if (atr) {
        atr->make_uint32(esr.val);
    } else {
        uart_printk("err %d\r\n", __LINE__);
    }
    /*uart_printk(errmsg);

    if (esr.b.BOFF) {
        uart_printk("err BOFF\r\n");
    }
    if (esr.b.EPVF) {
        uart_printk("err EPVF\r\n");
    }
    if (esr.b.EWGF) {
        uart_printk("err EWGF\r\n");
    }*/

    // [2] ERRI error interrupt.
    write32(&dev->MSR.val, 1 << 2);
    nvic_irq_clear(22);
}

extern "C" void CAN2_FIFO0_irq_handler() {
    CAN2_EndOfFrame();
    IrqHandlerInterface *iface = reinterpret_cast<IrqHandlerInterface *>(
            fw_get_object_interface("can2", "IrqHandlerInterface"));
    if (iface) {
        int fifoid = 0;
        iface->handleInterrupt(&fifoid);
    }
    nvic_irq_clear(64);
}

extern "C" void CAN2_FIFO1_irq_handler() {
    CAN2_EndOfFrame();
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
    write32(&dev->MSR.val, 1 << 2);
    nvic_irq_clear(66);
}


CanDriver::CanDriver(const char *name, int busid) : FwObject(name),
    baudrate_("baudrate"),
    mode_("mode"),
    rxcnt_("rxcnt"),
    pgm_("pgm"),
    errcnt_("errcnt"),
    lasterr_("lasterr"),
    errTrigger_(this, "errTrigger"),
    busid_(busid) {
    rxframe_rcnt = 0;
    rxframe_wcnt = 1;
    
    for (int i = 0; i < CPU_Total; i++) {
        dbgmsg_[i].buf[2] = ':';
        dbgmsg_[i].buf[3] = 0;
        dbgmsg_[i].cnt = 3;
    }
    dbgmsg_[CPU_Unknown].buf[0] = '-';
    dbgmsg_[CPU_Unknown].buf[1] = '-';
    dbgmsg_[CPU_M1].buf[0] = 'M';
    dbgmsg_[CPU_M1].buf[1] = '1';
    dbgmsg_[CPU_M2].buf[0] = 'M';
    dbgmsg_[CPU_M2].buf[1] = '2';
    dbgmsg_[CPU_A1].buf[0] = 'A';
    dbgmsg_[CPU_A1].buf[1] = '1';
    dbgmsg_[CPU_A2].buf[0] = 'A';
    dbgmsg_[CPU_A2].buf[1] = '2';
    dbgmsg_[CPU_D1].buf[0] = 'D';
    dbgmsg_[CPU_D1].buf[1] = '1';

    msg_history_.pos = 0;
    msg_history_.total = 0;
    /*msg_history_.pos = 3;
    msg_history_.total = 2;
    for (int i = 0; i < 30; i++) {
        msg_history_.buf[0][i] = 'A';
        msg_history_.buf[1][i] = 'B';
        msg_history_.buf[2][i] = 'C';
    }
    msg_history_.buf[0][30] = '\0';
    msg_history_.buf[1][30] = '\0';
    msg_history_.buf[2][30] = '\0';*/

    mode_.make_int8(0);
    pgm_.make_int8(-1);

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
    RegisterAttribute(&pgm_);
    RegisterAttribute(&errcnt_);
    RegisterAttribute(&lasterr_);
    RegisterAttribute(&errTrigger_);
}

void CanDriver::PostInit() {
}

void CanDriver::triggerError() {
    // [6:4] LEC. Last error code. It can be set to 111 by SW.
    write32(&dev_->ESR.val, 0x7 << 4);
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
    uint32_t ret = (hwid >> 3);
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
        rxcnt_.increment();

        // Detect and read PGM state
        if ((f->id & 0x1f00601f) == 0x1f00601f)
        {
            // 1_1111_0000_0000_0110_0000_0001_1111
            // hearbeat frame:
            // [12:0]       = rsrv1         0_0000_0001_1111
            // [14:13]      = frametype     11=heartbeat 01=targetframe; 00=BC-frame
            // [19:15]      = rsrv2         0000_0
            // [23:21]      = source        000
            // [28:24]      = bctype        0=SBC-frame; 8=MBC-frame

            // Data frame
            // [28:25]      = channel       1_111
            // {[24],[7:5]} = subchannel    1...1000
            // [23,16]      = target        0000_0000
            // [15:8]       = sender        0110_0000 (always const in heartbeat)
            // [7:5] see subchann
            // [4:0]        = counter       1_1111
            //uart_printk("%d %02x %02x %02x\r\n", busid_, (f->id>>16) & 0xFF, f->data.u8[1]);
            pgm_.make_int8(f->data.u8[1]);
        } else if ((f->id & 0x1e000000) == 0x1a000000) {
            int cpuidx = CPU_Unknown;
            DebugMessageType *msg;
            switch ((f->id >> 8) & 0xFF) {
            case 0xA3:
                cpuidx = CPU_M1;
                break;
            case 0x23:
                cpuidx = CPU_M2;
                break;
            case 0xA5:
                cpuidx = CPU_A1;
                break;
            case 0x25:
                cpuidx = CPU_A2;
                break;
            case 0xA8:
                cpuidx = CPU_D1;
                break;
            default:;
            }
            msg = &dbgmsg_[cpuidx];
            for (uint8_t i = 0; i < f->dlc; i++) {
                if (msg->cnt >= static_cast<int>(sizeof(msg->buf) - 1)) {
                    continue;
                }
                msg->buf[msg->cnt++] = f->data.s8[i];
                msg->buf[msg->cnt] = 0;
            }
            if ((f->id & 0x1f) == 0x1f) {
                // the last msg in a sequence
                char *tbuf = msg_history_.buf[msg_history_.pos++];
                if (msg_history_.pos >= CAN_MSG_HISTORY_LENGTH) {
                    msg_history_.pos = 0;
                }
                for (int i = 0; i <= msg->cnt; i++) {
                    tbuf[i] = msg->buf[i];
                }
                for (int i = msg->cnt; i < 30; i++) {   // 20 symbols of 24; 30 of 16 and 40 of 12
                    tbuf[i] = ' ';
                }
                tbuf[30] = 0;
                msg_history_.total++;
                msg->buf[3] = 0;
                msg->cnt = 3;
            }
        }
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
    ier.b.LECIE = 1;    // Last Error code interrupt
    //ier.b.BOFIE = 1;    // Bus-off interrupt
    //ier.b.EPVIE = 1;    // Error passive interrupt
    //ier.b.EWGIE = 1;    // Error warning interrupt
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
