#pragma once

#include <devgen.h>
#include <reggen.h>

class PCCSim : public DeviceGeneric {
 public:
    PCCSim(const char *name, uint64_t addr, size_t sz);

 private:
};