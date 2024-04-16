#include <s32k148regs.h>
#include "scgsim.h"

SCGSim::SCGSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    CSR(static_cast<DeviceGeneric *>(this), "CSR", addr + 0x10),
    RCCR(static_cast<DeviceGeneric *>(this), "RCCR", addr + 0x14),
    SOSCCSR(static_cast<DeviceGeneric *>(this), "SOSCCSR", addr + 0x100),
    SPLLCSR(static_cast<DeviceGeneric *>(this), "SPLLCSR", addr + 0x600) {
}

uint32_t SCGSim::SCG_CSR_TYPE::read_action(uint32_t prev) {
    SCGSim *p = static_cast<SCGSim *>(parent_);
    return p->RCCR.get();
}

uint32_t SCGSim::SCG_SOSCCSR_TYPE::read_action(uint32_t prev) {
    scg_sosccsr_reg_t t1;
    t1.value = prev;
    t1.bits.LK = 0;
    t1.bits.SOSCVLD = 1;
    return t1.value;
}

uint32_t SCGSim::SCG_SPLLCSR_TYPE::read_action(uint32_t prev) {
    scg_spllcsr_reg_t t1;
    t1.value = prev;
    t1.bits.LK = 0;
    t1.bits.SPLLVLD = 1;
    return t1.value;
}
