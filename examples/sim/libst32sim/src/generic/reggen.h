#pragma once

#include <devgen.h>

class Reg32Generic : public ISimMemoryInterface {
 public:
    Reg32Generic(DeviceGeneric *parent, const char *name, uint64_t off);
    virtual ~Reg32Generic();

    virtual void operation(MemoryOperationType *memop) override;

    // Side effect methods:
    virtual uint32_t read_action(uint32_t prev) { return prev; }
    virtual uint32_t write_action(uint32_t newval) { return newval; }

    // No side effect methods
    virtual void set(uint32_t val) { regvalue_ = val; }
    virtual uint32_t get() { return regvalue_; }

 protected:
    std::string name_;
    DeviceGeneric *parent_;
    uint32_t regvalue_;
};
