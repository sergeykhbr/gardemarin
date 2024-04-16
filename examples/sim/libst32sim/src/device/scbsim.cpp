#include <s32k148regs.h>
#include "scbsim.h"

SCBSim::SCBSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz) {
}
