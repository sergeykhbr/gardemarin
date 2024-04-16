#include "gpiosim.h"
#include <gpio.h>

GPIOSim::GPIOSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz) {
}
