#pragma once

#include <devgen.h>
#include <reggen.h>

class PORTSim : public DeviceGeneric {
 public:
    PORTSim(const char *name, uint64_t addr, size_t sz);

 private:
};