#include <s32k148regs.h>
#include "nvicsim.h"

NVICSim::NVICSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz) {
    char tstr[16];
    for (int i = 0; i < 8; i++) {
        sprintf_s(tstr, sizeof(tstr), "ISER%d", i);
        ISER[i] = new ISER_TYPE(
            static_cast<DeviceGeneric *>(this), tstr, addr + i*0x04, 32*i);
        sprintf_s(tstr, sizeof(tstr), "ICER%d", i);
        ICER[i] = new ICER_TYPE(
            static_cast<DeviceGeneric *>(this), tstr, addr + 0x80 + i*0x04, 32*i);

        sprintf_s(tstr, sizeof(tstr), "ISPR%d", i);
        ISPR[i] = new ISPR_TYPE(
            static_cast<DeviceGeneric *>(this), tstr, addr + 0x100 + i*0x04, 32*i);
        sprintf_s(tstr, sizeof(tstr), "ICPR%d", i);
        ICPR[i] = new ICPR_TYPE(
            static_cast<DeviceGeneric *>(this), tstr, addr + 0x180 + i*0x04, 32*i);

        ie_[i] = 0;
        ip_[i] = 0;
    }
}

NVICSim::~NVICSim() {
    for (int i = 0; i < 8; i++) {
        delete ISER[i];
        delete ICER[i];
        delete ISPR[i];
        delete ICPR[i];
    }
}

bool NVICSim::requestIrq(int idx) {
    uint32_t mask = 1ul << (idx & 0x1f);
    ip_[idx >> 5] |= mask;
    if (ie_[idx >> 5] & mask) {
        return true;
    }
    return false;
}

int NVICSim::nextPendingIrq() {
    int pcnt = 0;
    int idx;

    for (int i = 0; i < 8; i++) {
        if ((ie_[i] & ip_[i]) == 0) {
            // pending bit should be enabled
            continue;
        }
        for (int n = 0; n < 32; n++) {
            if ((ie_[i] & ip_[i] & (1ul << n)) == 0) {
                continue;
            }
            idx = 32*i + n;
            queue_[pcnt++] = idx;
        }
    }

    if (pcnt == 0) {
        return -1;
    }
    // TODO: priority check and re-ordering queue
    idx = queue_[0];
    ip_[idx >> 5] &= ~1ul << (idx & 0x1f);  // auto clear pending bit
    return idx;
}

uint32_t NVICSim::ISER_TYPE::write_action(uint32_t nxt) {
    NVICSim *p = static_cast<NVICSim *>(parent_);
    for (int i = 0; i < 32; i++) {
        if (nxt & (1ul << i)) {
            p->setIE(startidx_ + i);
        }
    }
    return nxt;
}

uint32_t NVICSim::ICER_TYPE::write_action(uint32_t nxt) {
    NVICSim *p = static_cast<NVICSim *>(parent_);
    for (int i = 0; i < 32; i++) {
        if (nxt & (1ul << i)) {
            p->clearIE(startidx_ + i);
        }
    }
    return nxt;
}

uint32_t NVICSim::ISPR_TYPE::write_action(uint32_t nxt) {
    NVICSim *p = static_cast<NVICSim *>(parent_);
    for (int i = 0; i < 32; i++) {
        if (nxt & (1ul << i)) {
            p->setIP(startidx_ + i);
        }
    }
    return nxt;
}

uint32_t NVICSim::ICPR_TYPE::write_action(uint32_t nxt) {
    NVICSim *p = static_cast<NVICSim *>(parent_);
    for (int i = 0; i < 32; i++) {
        if (nxt & (1ul << i)) {
            p->clearIP(startidx_ + i);
        }
    }
    return nxt;
}
