#include <s32k148regs.h>
#include <s32k148api.h>
#include "lpitsim.h"

LPITSim::LPITSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    MCR(static_cast<DeviceGeneric *>(this), "MCR", addr + 0x08),
    MSR(static_cast<DeviceGeneric *>(this), "MSR", addr + 0x0C),
    MIER(static_cast<DeviceGeneric *>(this), "MIER", addr + 0x10) {
    char tstr[16];
    for (int i = 0; i < 4; i++) {
        sprintf_s(tstr, sizeof(tstr), "TVAL%d", i);
        TVAL[i] = new Reg32Generic(
            static_cast<DeviceGeneric *>(this), tstr, addr + 0x20 + i*0x10);
        sprintf_s(tstr, sizeof(tstr), "CVAL%d", i);
        CVAL[i] = new Reg32Generic(
            static_cast<DeviceGeneric *>(this), tstr, addr + 0x24 + i*0x10);
        sprintf_s(tstr, sizeof(tstr), "TCTRL%d", i);
        TCTRL[i] = new Reg32Generic(
            static_cast<DeviceGeneric *>(this), tstr, addr + 0x28 + i*0x10);
    }
    freqHz_ = 40000000.0;   // SPLL2 = 40 MHz
}

LPITSim::~LPITSim() {
    for (int i = 0; i < 4; i++) {
        delete TVAL[i];
        delete CVAL[i];
        delete TCTRL[i];
    }
}

void LPITSim::update(double dt) {
    uint64_t cnt;
    lpit_mcr_reg_t mcr;
    mcr.value = MCR.get();

    if (mcr.bits.SW_RST) {
        for (int i = 0; i < 4; i++) {
            CVAL[i]->set(0);
            TVAL[i]->set(0);
            TCTRL[i]->set(0);
        }
        return;
    }
    if (!mcr.bits.M_CEN) {
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (!TCTRL[i]->get() & 0x1) {
            // counter disabled
            continue;
        }
        if (TVAL[i]->get() == 0) {
            continue;
        }
        cnt = CVAL[i]->get();
        cnt += static_cast<uint64_t>(freqHz_ * dt);
        if (cnt >= TVAL[i]->get()) {
            cnt -= TVAL[i]->get();
            if (MIER.get() & (1u << i)) {
                s32k148_request_interrupt(48);
            }
        }
        CVAL[i]->set(static_cast<uint32_t>(cnt));
    }
}

