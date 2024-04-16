#pragma once

#include <memopgen.h>

class DeviceGeneric : public ISimMemoryInterface {
 public:
    DeviceGeneric(const char *name, uint64_t addr, size_t sz);

    virtual void init();

    virtual void update(double dt) {}

    virtual const char *deviceName() { return devname_; }

 protected:
    char devname_[256];
};
