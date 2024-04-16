#include "reggen.h"

Reg32Generic::Reg32Generic(DeviceGeneric *parent, const char *name, uint64_t off) {
    parent_ = parent;
    memAddr_ = off;
    memSize_ = 4;
    regvalue_ = 0;
    mem_ = reinterpret_cast<uint8_t *>(&regvalue_);
    name_ = std::string(name);
    parent_->addMap(static_cast<ISimMemoryInterface *>(this));
}

Reg32Generic::~Reg32Generic() {
    mem_ = 0;
}

void Reg32Generic::operation(MemoryOperationType *memop){
    uint64_t off = memop->addr - getBaseAddress();
    if (memop->type == Memop_Read) {
        memop->payload.u32[0] = read_action(regvalue_);
    } else {
        regvalue_ = write_action(memop->payload.u32[0]);
    }
}
