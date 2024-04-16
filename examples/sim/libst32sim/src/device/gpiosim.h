#pragma once

#include <devgen.h>
#include <reggen.h>

class GPIOSim : public DeviceGeneric {
 public:
    GPIOSim(const char *name, uint64_t addr, size_t sz);

 private:
};