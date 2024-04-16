#include <s32k148regs.h>
#include "portsim.h"

PORTSim::PORTSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz) {
}
