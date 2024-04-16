#pragma once

#include <devgen.h>
#include <reggen.h>

class SCBSim : public DeviceGeneric {
 public:
    SCBSim(const char *name, uint64_t addr, size_t sz);

 private:
};