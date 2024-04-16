#pragma once

#include <inttypes.h>
#include <string>
#include <isimface.h>

typedef enum EMemopAction {
    Memop_Read,
    Memop_Write  
} EMemopAction;

struct MemoryOperationType {
    EMemopAction type;
    size_t size;
    uint64_t addr;
    union PayloadType {
        uint8_t u8[8];
        uint16_t u16[4];
        uint32_t u32[2];
        uint64_t u64;
    } payload;
};

class ISimMemoryInterface : public ISimFace {
 public:
    ISimMemoryInterface();
    virtual ~ISimMemoryInterface();

    virtual void operation(MemoryOperationType *memop);

    virtual uint64_t getBaseAddress() { return memAddr_; }

    virtual uint64_t getSize() { return memSize_; }

    virtual void addMap(ISimMemoryInterface *dev);

 protected:
    uint64_t memAddr_;
    size_t memSize_;
    uint8_t *mem_;
    // The std::list cannot be used beacuse of locking before SuspendThread
    // and then there's no possiblity to use iterator while thread is suspended:
    //std::list<ISimMemoryInterface *>map_;
    ISimMemoryInterface **maplist_;
    int mapmax_;
    int mapcnt_;
};
