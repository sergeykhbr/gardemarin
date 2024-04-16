#include <s32k148regs.h>
#include "wdogsim.h"

WDOGSim::WDOGSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz) {
}
