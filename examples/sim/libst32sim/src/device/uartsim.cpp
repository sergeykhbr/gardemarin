#include "uartsim.h"
#include <iostream>
#include <simutils.h>

UARTSim::UARTSim(const char *name, uint64_t addr, size_t sz) :
    DeviceGeneric(name, addr, sz),
    DATA(static_cast<DeviceGeneric *>(this), "DATA", addr + 0x1C) {
}

uint32_t UARTSim::UART_DATA_TYPE::write_action(uint32_t nxt) {
    char tstr[2] = {static_cast<char>(nxt), 0};
    printf_info("%s", tstr);
    return nxt;
}
