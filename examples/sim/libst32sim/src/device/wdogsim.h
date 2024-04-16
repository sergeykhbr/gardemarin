#pragma once

#include <devgen.h>
#include <reggen.h>

class WDOGSim : public DeviceGeneric {
 public:
    WDOGSim(const char *name, uint64_t addr, size_t sz);

 private:
};