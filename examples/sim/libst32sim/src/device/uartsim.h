#pragma once

#include <devgen.h>
#include <reggen.h>

class UARTSim : public DeviceGeneric {
 public:
    UARTSim(const char *name, uint64_t addr, size_t sz);

 private:

    class UART_DATA_TYPE : public Reg32Generic {
     public:
        UART_DATA_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
     protected:
        virtual uint32_t write_action(uint32_t nxt) override;
    };

 private:
    UART_DATA_TYPE DATA;
};