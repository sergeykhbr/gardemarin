#include <s32k148regs.h>
#include "pccsim.h"

PCCSim::PCCSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz) {
}
