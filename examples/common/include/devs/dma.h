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

typedef struct dma_stream_cr_bits_type {
    volatile uint32_t EN : 1;      // [0] rw Stream enable
    volatile uint32_t DMEIE : 1;   // [1] rw Direct moed error interrupt enable
    volatile uint32_t TEIE : 1;    // [2] rwTransfer error interrupt enable
    volatile uint32_t HTIE : 1;    // [3] rw Half transfer complete interrupt enable
    volatile uint32_t TCIE : 1;    // [4] rw Transfer complete interrupt enable
    volatile uint32_t PFCTRL : 1;  // [5] rw Peripheral flow control: 0=dma is the flow controller; 1=periph is the flow controller
    volatile uint32_t DIR : 2;     // [7] Data transfer direction: 00=periph-to-mem; 01=mem-to-periph; 10=mem-to-mem
    volatile uint32_t CIRC : 1;    // [8] Circular mode: 0=disabled
    volatile uint32_t PINC : 1;    // [9] Peripheral increment mode: 0=memory address is fixed; 1=increment after each data transfer
    volatile uint32_t MINC : 1;    // [10] Memory increment mode: 0=memory address is fixed; 1=increment after each data transfer
    volatile uint32_t PSIZE : 2;   // [12:11] Peripheral data size: 00=8bit; 01=16-bit; 10=32-bit; 11=reserved
    volatile uint32_t MSIZE : 2;   // [14:13] Memory data size: 00=8bit; 01=16-bit; 10=32-bit; 11=reserved
    volatile uint32_t PINCOS : 1;  // [15]  Peripheral increment offset size: 0=linked to PSIZE; 1=fixed 4 Bytes
    volatile uint32_t PL : 2;      // [17:16] Prioirty levelL 00=Low; 01=Medium; 10=High; 11=very high
    volatile uint32_t DBM : 1;     // [18] Double buffer mode
    volatile uint32_t CT : 1;      // [19] Current target only in (DBM=1 double buffer mode) ping-pong index
    uint32_t rsrv20 : 1;           // [20]
    volatile uint32_t PBURST : 2;  // [22:21] Peripheral burst trasfer: 00=single; 01=INCR4; 10=INCR8; 11=INCR16
    volatile uint32_t MBURST : 2;  // [24:23] Memory burst trasfer: 00=single; 01=INCR4; 10=INCR8; 11=INCR16
    volatile uint32_t CHSEL : 3;   // [27:25] Channel selection: 000=0, 001=1 ... 111=7
    uint32_t rsrv31_28 : 4;        // [31:28]
} dma_stream_cr_bits_type;

typedef union dma_stream_cr_reg_type {
    dma_stream_cr_bits_type bits;
    volatile uint32_t val;
} dma_stream_cr_reg_type;

typedef struct DMA_stream_regs_type {
    dma_stream_cr_reg_type CR;     // [RW] DMA stream x configuration register
    volatile uint32_t NDTR;   // [RW] DMA stream x number of data register
    volatile uint32_t PAR;    // [RW] DMA stream x peripheral address register
    volatile uint32_t M0AR;   // [RW] DMA stream x memory 0 address register
    volatile uint32_t M1AR;   // [RW] DMA stream x memory 1 address register
    volatile uint32_t FCR;    // [RW] DMA stream x FIFO control register
} DMA_stream_regs_type;

typedef struct DMA_registers_type
{
    volatile uint32_t LISR;   // [RO] DMA low interrupt status register,      Address offset: 0x00
    volatile uint32_t HISR;   // [RO] DMA high interrupt status register,     Address offset: 0x04
    volatile uint32_t LIFCR;  // [WO] DMA low interrupt flag clear register,  Address offset: 0x08
    volatile uint32_t HIFCR;  // [WO] DMA high interrupt flag clear register, Address offset: 0x0C
} DMA_registers_type;

