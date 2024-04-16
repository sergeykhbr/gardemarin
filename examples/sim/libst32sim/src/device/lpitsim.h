#pragma once

#include <devgen.h>
#include <reggen.h>

class LPITSim : public DeviceGeneric {
 public:
    LPITSim(const char *name, uint64_t addr, size_t sz);
    virtual ~LPITSim();

    virtual void update(double dt) override;

 private:
    Reg32Generic MCR;
    Reg32Generic MSR;
    Reg32Generic MIER;
    Reg32Generic *TVAL[4];
    Reg32Generic *CVAL[4];
    Reg32Generic *TCTRL[4];

    double freqHz_;
};