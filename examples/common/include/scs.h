/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <inttypes.h>

/**
  System Control Space memory registers, part of the M4 core (see M4 trm)
 */

typedef struct SysTick_registers_type
{
    volatile uint32_t CSR;       // [RW] SysTick Control and Status Register
    volatile uint32_t RVR;       // [RW] SysTick Reload Value Register
    volatile uint32_t CVR;       // [RW] SysTick Current Value Register
    volatile uint32_t CALIB;     // [RO] SysTick Calibration value Register
} SysTick_registers_type;

/** System Control Block registers */
typedef struct SCB_registers_type
{
    volatile uint32_t CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register 0x410FC241 = M4 */
    volatile uint32_t ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
    volatile uint32_t VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
    volatile uint32_t AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
    volatile uint32_t SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
    volatile uint32_t CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
    volatile uint8_t  SHP[12U];               /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
    volatile uint32_t SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
    volatile uint32_t CFSR;                   /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
    volatile uint32_t HFSR;                   /*!< Offset: 0x02C (R/W)  HardFault Status Register */
    volatile uint32_t DFSR;                   /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
    volatile uint32_t MMFAR;                  /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
    volatile uint32_t BFAR;                   /*!< Offset: 0x038 (R/W)  BusFault Address Register */
    volatile uint32_t AFSR;                   /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
    volatile uint32_t PFR[2U];                /*!< Offset: 0x040 (R/ )  Processor Feature Register */
    volatile uint32_t DFR;                    /*!< Offset: 0x048 (R/ )  Debug Feature Register */
    volatile uint32_t ADR;                    /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
    volatile uint32_t MMFR[4U];               /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
    volatile uint32_t ISAR[5U];               /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
    uint32_t RESERVED0[5U];
    volatile uint32_t CPACR;                  /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
} SCB_registers_type;
