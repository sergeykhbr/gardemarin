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

#include <can.h>
#include <sim_api.h>
#include <string>
#include "cansim.h"

CANSim::CANSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    MCR(static_cast<DeviceGeneric *>(this), "MCR", addr + 0x0),
    MSR(static_cast<DeviceGeneric *>(this), "MSR", addr + 0x4),
    RXMGMASK(static_cast<DeviceGeneric *>(this), "RXMGMASK", addr + 0x10),
    IMASK1(static_cast<DeviceGeneric *>(this), "IMASK1", addr + 0x28),
    IFLAG1(static_cast<DeviceGeneric *>(this), "IFLAG1", addr + 0x30) {

    char tstr[64];
    for (int i = 0; i < 32; i++) {
        // RAM registers
        sprintf_s(tstr, sizeof(tstr), "RAM%d", i);
        RAMn_FRAME[i] = new CAN_RAMn_FRAME(static_cast<DeviceGeneric *>(this),
                                tstr, addr + 0x80 + i*72, i);

        // Channels mask registers
        sprintf_s(tstr, sizeof(tstr), "RXIMR%d", i);
        RXIMR[i] = new Reg32Generic(static_cast<DeviceGeneric *>(this),
                                tstr, addr + 0x880 + i*4);
    }
    estate_ = State_Off;
    msgsel_ = 0;
    irqRequest_ = false;

    shcan_ = 0;
    /*shmem_handle_ = utils_memshare_create("canfd1", sizeof(ShmMemType));
    if (shmem_handle_) {
        shcan_ = reinterpret_cast<ShmMemType *>(
                utils_memshare_map(shmem_handle_, sizeof(ShmMemType)));
    }
    if (shcan_ && shcan_->size != sizeof(ShmMemType)) {
        shm_init(shcan_);
    }*/
    busId_ = 0;
}

CANSim::~CANSim() {
    for (int i = 0; i < 32; i++) {
        delete RAMn_FRAME[i];
        delete RXIMR[i];
    }
}

void CANSim::update(double dt) {
    switch (estate_) {
    case State_Idle:
        if (isBusy()) {
            estate_ = State_Rx;
        } else {
            for (int i = 0; i < 32; i++) {
                if (!RAMn_FRAME[i]->TX()) {
                    continue;
                }
                RAMn_FRAME[i]->CLRTX();
                msgsel_ = i;
                estate_ = State_Tx;
                break;
            }
        }
        break;
    case State_Rx:
        msgsel_ = checkRxFrameID();
        // Ignore remote frames (they do not handled by software)
        if (msgsel_ != -1 && RAMn_FRAME[msgsel_]->RX()) {
            IFLAG1.set(IFLAG1.get() | (1ul << msgsel_));
            estate_ = State_Idle; //State_RxReady;
            if ((IFLAG1.get() & IMASK1.get()) != 0) {
                irqRequest_ = true;
            }
        }
        break;
    case State_Tx:
            write_frame();
            IFLAG1.set(IFLAG1.get() | (1ul << msgsel_));
            estate_ = State_Idle;
        break;
    default:;
    }

    if (irqRequest_) {
        irqRequest_ = false;
        sim_request_interrupt(81);
    }
}

void CANSim::write_frame() {
    writeFrameToShm();

    unsigned id = RAMn_FRAME[msgsel_]->ID();
    uint16_t dlc = RAMn_FRAME[msgsel_]->DLC();

#if 0
    printf_info("CAN[%d]: ID: %3x DLC: %1u Data:", msgsel_, id, dlc);
    for (int i = 0; i < dlc; i++) {
        printf_info(" %02x", RAMn_FRAME[msgsel_]->DATA()[i]);
    }
    printf_info("%s", "\n");
#endif
}

void CANSim::writeFrameToShm() {
    if (shcan_ == 0) {
        return;
    }
    /*while (shm_is_full(&shcan_->bus[busId_].dev2pc)) {
        utils_sleep_ms(1);
    }
    shm_set(&shcan_->bus[busId_].dev2pc,
            *RAMn_FRAME[msgsel_]->frame());*/
}

void CANSim::writeFrameToVxl() {
    /*memcpy(vxl_txmsg_.frame.buf8,
           RAMn_FRAME[msgsel_]->frame(),
           sizeof(CanFrameType));
    vxl_send_message(&vxl_txmsg_);*/
}

bool CANSim::isBusy() {
    // Check availablity of shm interface
    if (shcan_ == 0) {
        return false;
    }
    //return !shm_is_empty(&shcan_->bus[busId_].pc2dev);
    return false;
}


uint32_t CANSim::CAN_MCR_TYPE::write_action(uint32_t nxt) {
    CANSim *p = static_cast<CANSim *>(parent_);
    CAN_MCR_type t1;
    t1.val = nxt;
    if (t1.b.INRQ) {
        p->setState(State_Init);
    } else if (t1.b.SLEEP) {
        p->setState(State_Sleep);
    } else {
        p->setState(State_Idle);
    }
    return t1.val;
}

uint32_t CANSim::CAN_MSR_TYPE::read_action(uint32_t prev) {
    CANSim *p = static_cast<CANSim *>(parent_);
    CAN_MSR_type t1;
    t1.val = 0;
    switch (p->getState()) {
    case State_Init:
        t1.b.INAK = 1;
        break;
    case State_Rx:
        t1.b.RXM = 1;
        break;
    case State_Tx:
        t1.b.TXM = 1;
        break;
    default:;
    }
    return t1.val;
}


int CANSim::checkRxFrameID() {
#if 0
    uint32_t gm;     // global mask
    uint32_t lm;     // local mask
    uint32_t ar_id;
    CanFrameType rxframe;
    int ret = -1;
    if (shcan_ == 0) {
        return ret;
    }
    // No get usefull frame
    shm_get(&shcan_->bus[busId_].pc2dev, rxframe);

    gm = RXMGMASK.get();
    for (int i = 0; i < 32; i++) {
        if (!RAMn_FRAME[i]->RX()) {
            // Not configured for Rx
            continue;
        }

        lm = RXIMR[i]->get();
        ar_id = (rxframe.bits.IDEXT << 11) | rxframe.bits.IDSTD;

        if ((ar_id & gm & lm) == RAMn_FRAME[i]->ID()) {
            ret = i;
            memcpy(RAMn_FRAME[i]->DATA(),
                   &rxframe.bits.data,
                   sizeof(rxframe.bits.data));
            //printf_debug("Message pass filter object[%d]\n", i);
        }
    }
    return ret;
#else
    return 1;
#endif
}
