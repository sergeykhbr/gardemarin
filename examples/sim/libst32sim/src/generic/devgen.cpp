#include "devgen.h"

DeviceGeneric::DeviceGeneric(const char *name, uint64_t addr, size_t sz) {
    strcpy(devname_, name);
    memAddr_ = addr;
    memSize_ = sz;
}

void DeviceGeneric::init() {
    if (memSize_ < 1024 * 1024) {
        mem_ = new uint8_t[memSize_];
    }
}

