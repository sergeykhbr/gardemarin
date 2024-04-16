#pragma once

#include <devgen.h>
#include <reggen.h>

class SCGSim : public DeviceGeneric {
 friend class SCG_CSR_TYPE;
 public:
    SCGSim(const char *name, uint64_t addr, size_t sz);

 private:
    class SCG_CSR_TYPE : public Reg32Generic {
     public:
        SCG_CSR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
     protected:
        virtual uint32_t read_action(uint32_t prev) override;
    };

    class SCG_SOSCCSR_TYPE : public Reg32Generic {
     public:
        SCG_SOSCCSR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
     protected:
        virtual uint32_t read_action(uint32_t prev) override;
    };

    class SCG_SPLLCSR_TYPE : public Reg32Generic {
     public:
        SCG_SPLLCSR_TYPE(DeviceGeneric *parent, const char *name, uint64_t off)
            : Reg32Generic(parent, name, off) {
        }
     protected:
        virtual uint32_t read_action(uint32_t prev) override;
    };

 private:
    SCG_CSR_TYPE CSR;
    Reg32Generic RCCR;
    SCG_SOSCCSR_TYPE SOSCCSR;
    SCG_SPLLCSR_TYPE SPLLCSR;
};