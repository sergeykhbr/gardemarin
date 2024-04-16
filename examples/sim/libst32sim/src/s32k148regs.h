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

#ifndef __IO
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif


#define CANFD_ENA
//#define LOOPBACK_MODE_ENA

#ifdef CANFD_ENA
#define MSG_BUF_SIZE  18
#define CANFD_BRS_DISABLE
#else
#define MSG_BUF_SIZE  4		/* Msg Buffer Size. (CAN 2.0AB: 2 hdr +  2 data= 4 words) */
#define CAN2_80MHZ
#endif


// page 1642: CAN register list

typedef struct can_mcr_bits_t {
    volatile uint32_t MAXMB : 7;     // [6:0]
    volatile uint32_t rsv7 : 1;      // [7]
    volatile uint32_t IDAM : 2;      // [9:8]
    volatile uint32_t rsv10 : 1;     // [10]
    volatile uint32_t FDEN : 1;      // [11]
    volatile uint32_t AEN : 1;       // [12]
    volatile uint32_t LPRIOEN : 1;   // [13]
    volatile uint32_t PNET_EN : 1;   // [14]
    volatile uint32_t DMA : 1;       // [15]
    volatile uint32_t IRMQ : 1;      // [16]
    volatile uint32_t SRXDIS : 1;    // [17]
    volatile uint32_t rsv19_18 : 2;  // [19:18]
    volatile uint32_t LPMACK : 1;    // [20]
    volatile uint32_t WRNEN : 1;     // [21]
    volatile uint32_t rsv22 : 1;     // [22]
    volatile uint32_t SUPV : 1;      // [23]
    volatile uint32_t FRZACK : 1;    // [24]
    volatile uint32_t SOFTRST : 1;   // [25]
    volatile uint32_t rsv26 : 1;     // [26]
    volatile uint32_t NOTRDY : 1;    // [27]
    volatile uint32_t HALT : 1;      // [28]
    volatile uint32_t RFEN : 1;      // [29]
    volatile uint32_t FRZ : 1;       // [30]
    volatile uint32_t MDIS : 1;      // [31]
} can_mcr_bits_t;

typedef union can_mcr_reg_t {
    volatile uint32_t value;
    can_mcr_bits_t bits;
} can_mcr_reg_t;

typedef struct can_ctrl1_bits_t {
    volatile uint32_t PROPSEG : 3;   // [2:0] Propagation segment
    volatile uint32_t LOM : 1;       // [3] Listen-only mode: 1=ena
    volatile uint32_t LBUF : 1;      // [4] Lowest buffer transmitted first: 1=ena
    volatile uint32_t TSYN : 1;      // [5] Timer sync: 1=feature enabled
    volatile uint32_t BOFFREC : 1;   // [6] Bus Off Recovery: 1=auto recovery disabled
    volatile uint32_t SMP : 1;       // [7] CAN bit sampling: 0=one sample per bit; 1=3 samples per bit
    volatile uint32_t rsrv9_8 : 2;   // [9:8] 
    volatile uint32_t RWRNMSK : 1;   // [10] Rx Warning Interrupt Mask
    volatile uint32_t TWRNMSK : 1;   // [11] Tx Warning Interrupt Mask
    volatile uint32_t LPB : 1;       // [12] Loop Back mode
    volatile uint32_t CLKSRC : 1;    // [13] CAN engine clk source:0=engine clk; 1=periph clk
    volatile uint32_t ERRMSK : 1;    // [14] Error interrupt mask: 1=enabled
    volatile uint32_t BOFFMSK : 1;   // [15] Bus Off Interript mask: 1=enabled
    volatile uint32_t PSEG2 : 3;     // [18:16] Phase segment 2
    volatile uint32_t PSEG1 : 3;     // [21:19] Phase segment 1
    volatile uint32_t RJW : 2;       // [23:22] Resync Jump width
    volatile uint32_t PRESDIV : 8;   // [31:24] Prescale division factor: sclk=peclk/(presdiv+1)
} can_ctrl1_bits_t;

typedef union can_ctrl1_reg_t {
    volatile uint32_t value;
    can_ctrl1_bits_t bits;
} can_ctrl1_reg_t;

// CAN Bit Timing register (alternative to CTRL1)
typedef struct can_cbt_bits_t {
    volatile uint32_t EPSEG2 : 5;       // [4:0] Extended Phase Segment 2
    volatile uint32_t EPSEG1 : 5;       // [9:5] Extended Phase Segment 1
    volatile uint32_t EPROPSEG : 6;     // [15:10] Extended Propagation Segment
    volatile uint32_t ERJW : 5;         // [20:16] Extended Resync Jump Width
    volatile uint32_t EPRESDIV : 10;    // [30:21] Extended prescale division factor
    volatile uint32_t BTF : 1;          // [31] Bit Timing Format Enable, must be 1 for CANFD
} can_cbt_bits_t;

typedef union can_cbt_reg_t {
    volatile uint32_t value;
    can_cbt_bits_t bits;
} can_cbt_reg_t;

// CAN-FD Bit Timing register
typedef struct can_fdcbt_bits_t {
    volatile uint32_t FPSEG2 : 3;       // [2:0] Fast Phase Segment 2
    volatile uint32_t rsv4_3 : 2;       // [4:3]
    volatile uint32_t FPSEG1 : 3;       // [7:5] Fast Phase Segment 1
    volatile uint32_t rsv9_8 : 2;       // [9:8]
    volatile uint32_t FPROPSEG : 5;     // [15:10] Fast Propagation Segment
    volatile uint32_t rsv15 : 1;        // [15]
    volatile uint32_t FRJW : 3;         // [18:16] Fast Resync Jump Width
    volatile uint32_t rsv19 : 1;        // [19]
    volatile uint32_t FPRESDIV : 10;    // [29:20] Fast prescale division factor
    volatile uint32_t rsv31_30 : 2;     // [31:30]
} can_fdcbt_bits_t;

typedef union can_fdcbt_reg_t {
    volatile uint32_t value;
    can_fdcbt_bits_t bits;
} can_fdcbt_reg_t;


// CAN-FD Control register
typedef struct can_fdctrl_bits_t {
    volatile uint32_t TDCVAL : 6;       // [5:0] Transceiver delay compensation value
    volatile uint32_t rsv7_6 : 2;       // [7:6]
    volatile uint32_t TDCOFF : 5;       // [12:8] Transceiver delay compensation offset
    volatile uint32_t rsv13 : 1;        // [13]
    volatile uint32_t TDCFAIL : 1;      // [14] Transceiver delay compensation fail
    volatile uint32_t TDCEN : 1;        // [15] Transceiver delay compensation enable
    volatile uint32_t MBDSR0 : 2;       // [17:16] Message Buffer Data size for region 0: 0=8B; 1=16B; 2=32B; 3=64B
    volatile uint32_t rsv30_18 : 13;    // [30:18]
    volatile uint32_t FDRATE : 1;       // [31] Bit rate Switch Enable
} can_fdctrl_bits_t;

typedef union can_fdctrl_reg_t {
    volatile uint32_t value;
    can_fdctrl_bits_t bits;
} can_fdctrl_reg_t;


typedef struct can_ctrl2_bits_t {
    volatile uint32_t rsv10_0 : 11;     // [10:0]
    volatile uint32_t EDFLTDI : 1;      // [11] Edge filter disable
    volatile uint32_t ISOCANFDEN : 1;   // [12] ISO CAN FD enable
    volatile uint32_t rsrv13 : 1;       // [13] 
    volatile uint32_t PREXCEN : 1;      // [14] Protocol exception enable
    volatile uint32_t TIMER_SRC : 1;    // [15] Timer source (for CAN0 only): 0=timer clocked by CAN bit clock
    volatile uint32_t EACEN : 1;        // [16] Entire frame arbitration field comparision enable for Rx mailboxes
    volatile uint32_t RRS : 1;          // [17] Remote request storing
    volatile uint32_t MRP : 1;          // [18] Mailboxes Reception Priority
    volatile uint32_t TASD : 5;         // [23:19] Tx Arbitration start delay
    volatile uint32_t RFFN : 4;         // [27:24] Number of Rx FIFO filters
    volatile uint32_t rsv29_28 : 2;     // [29:28]
    volatile uint32_t BOFFDONEMS : 1;   // [30] Bus Off Done Interrupt Mask (0=dis)
    volatile uint32_t ERRMSK_FAST : 1;  // [31] Error interrupt Mask in Data phase for CANFD (0=dis)
} can_ctrl2_bits_t;

typedef union can_ctrl2_reg_t {
    volatile uint32_t value;
    can_ctrl2_bits_t bits;
} can_ctrl2_reg_t;


typedef struct can_frame_bits_t {
    volatile uint32_t timestamp : 16;        // [0][15:0] free running counter
    volatile uint32_t DLC : 4;               // [0][19:16] Length of Data bytes
    volatile uint32_t RTR : 1;               // [0][20] Remote Transmition Request
    volatile uint32_t IDE : 1;               // [0][21] ID Extended Bit
    volatile uint32_t SRR : 1;               // [0][22] Substitute Remote Request
    volatile uint32_t rsv023 : 1;            // [0][23]
    volatile uint32_t CODE : 4;              // [0][27:24] Message buffer code: 0xC = Tx Data Frame (RTR must be 0)
    volatile uint32_t rsv028 : 1;            // [0][28]
    volatile uint32_t ESI : 1;               // [0][29] Error state indicator
    volatile uint32_t BRS : 1;               // [0][30] Bit Rate Switch inside CAN FD frame
    volatile uint32_t EDL : 1;               // [0][31] Extended Data Length: 1=CANFD
    volatile uint32_t IDEXT : 18;            // [1][17:0]
    volatile uint32_t IDSTD : 11;            // [1][28:18] Frame ID
    volatile uint32_t PRIO : 3;              // [1][31:29] Local Prioirty
    char data[64];
} can_frame_bits_t;

typedef union can_frame_t {
    volatile uint32_t buf[18];
    can_frame_bits_t bits;
} can_frame_t;

/** CAN - Size of Registers Arrays */
#define CAN_RAMn_COUNT                           128u
#define CAN_RXIMR_COUNT                          32u
#define CAN_WMB_COUNT                            4u

/** CAN - Register Layout Typedef */
typedef struct {
  __IO uint32_t MCR;                               /**< Module Configuration Register, offset: 0x0 */
  __IO uint32_t CTRL1;                             /**< Control 1 register, offset: 0x4 */
  __IO uint32_t TIMER;                             /**< Free Running Timer, offset: 0x8 */
       uint8_t RESERVED_0[4];
  __IO uint32_t RXMGMASK;                          /**< Rx Mailboxes Global Mask Register, offset: 0x10 */
  __IO uint32_t RX14MASK;                          /**< Rx 14 Mask register, offset: 0x14 */
  __IO uint32_t RX15MASK;                          /**< Rx 15 Mask register, offset: 0x18 */
  __IO uint32_t ECR;                               /**< Error Counter, offset: 0x1C */
  __IO uint32_t ESR1;                              /**< Error and Status 1 register, offset: 0x20 */
       uint8_t RESERVED_1[4];
  __IO uint32_t IMASK1;                            /**< Interrupt Masks 1 register, offset: 0x28 */
       uint8_t RESERVED_2[4];
  __IO uint32_t IFLAG1;                            /**< Interrupt Flags 1 register, offset: 0x30 */
  __IO uint32_t CTRL2;                             /**< Control 2 register, offset: 0x34 */
  __I  uint32_t ESR2;                              /**< Error and Status 2 register, offset: 0x38 */
       uint8_t RESERVED_3[8];
  __I  uint32_t CRCR;                              /**< CRC Register, offset: 0x44 */
  __IO uint32_t RXFGMASK;                          /**< Rx FIFO Global Mask register, offset: 0x48 */
  __I  uint32_t RXFIR;                             /**< Rx FIFO Information Register, offset: 0x4C */
  __IO uint32_t CBT;                               /**< CAN Bit Timing Register, offset: 0x50 */
       uint8_t RESERVED_4[44];
  __IO uint32_t RAMn[CAN_RAMn_COUNT];              /**< Embedded RAM, array offset: 0x80, array step: 0x4 */
       uint8_t RESERVED_5[1536];
  __IO uint32_t RXIMR[CAN_RXIMR_COUNT];            /**< Rx Individual Mask Registers, array offset: 0x880, array step: 0x4 */
       uint8_t RESERVED_6[512];
  __IO uint32_t CTRL1_PN;                          /**< Pretended Networking Control 1 Register, offset: 0xB00 */
  __IO uint32_t CTRL2_PN;                          /**< Pretended Networking Control 2 Register, offset: 0xB04 */
  __IO uint32_t WU_MTC;                            /**< Pretended Networking Wake Up Match Register, offset: 0xB08 */
  __IO uint32_t FLT_ID1;                           /**< Pretended Networking ID Filter 1 Register, offset: 0xB0C */
  __IO uint32_t FLT_DLC;                           /**< Pretended Networking DLC Filter Register, offset: 0xB10 */
  __IO uint32_t PL1_LO;                            /**< Pretended Networking Payload Low Filter 1 Register, offset: 0xB14 */
  __IO uint32_t PL1_HI;                            /**< Pretended Networking Payload High Filter 1 Register, offset: 0xB18 */
  __IO uint32_t FLT_ID2_IDMASK;                    /**< Pretended Networking ID Filter 2 Register / ID Mask Register, offset: 0xB1C */
  __IO uint32_t PL2_PLMASK_LO;                     /**< Pretended Networking Payload Low Filter 2 Register / Payload Low Mask Register, offset: 0xB20 */
  __IO uint32_t PL2_PLMASK_HI;                     /**< Pretended Networking Payload High Filter 2 low order bits / Payload High Mask Register, offset: 0xB24 */
       uint8_t RESERVED_7[24];
  struct {                                         /* offset: 0xB40, array step: 0x10 */
    __I  uint32_t WMBn_CS;                           /**< Wake Up Message Buffer Register for C/S, array offset: 0xB40, array step: 0x10 */
    __I  uint32_t WMBn_ID;                           /**< Wake Up Message Buffer Register for ID, array offset: 0xB44, array step: 0x10 */
    __I  uint32_t WMBn_D03;                          /**< Wake Up Message Buffer Register for Data 0-3, array offset: 0xB48, array step: 0x10 */
    __I  uint32_t WMBn_D47;                          /**< Wake Up Message Buffer Register Data 4-7, array offset: 0xB4C, array step: 0x10 */
  } WMB[CAN_WMB_COUNT];
       uint8_t RESERVED_8[128];
  __IO uint32_t FDCTRL;                            /**< CAN FD Control Register, offset: 0xC00 */
  __IO uint32_t FDCBT;                             /**< CAN FD Bit Timing Register, offset: 0xC04 */
  __I  uint32_t FDCRC;                             /**< CAN FD CRC Register, offset: 0xC08 */
} CAN_Type;



typedef struct lmem_pcccr_bits_t {
    volatile uint32_t ENCACHE : 1;   // [0] Cache Enable
    volatile uint32_t rsv1 : 1;      // [1]
    volatile uint32_t PCCR2 : 1;     // [2] Force all cachable spaces to write through
    volatile uint32_t PCCR3 : 1;     // [3] Force no allocation on cache miss
    volatile uint32_t rsv23_4 : 20;  // [23:4] 
    volatile uint32_t INVW0 : 1;     // [24] Invalidate way 0
    volatile uint32_t PUSHW0 : 1;    // [25] Push way 0 when GO=1
    volatile uint32_t INVW1 : 1;     // [26] Invalidate way 1
    volatile uint32_t PUSHW1 : 1;    // [27] Push way 1 when GO=1
    volatile uint32_t rsv30_28 : 3;  // [30:28]
    volatile uint32_t GO : 1;        // [31] Initiate Cache command
} lmem_pcccr_bits_t;

typedef union lmem_pcccr_reg_t {
    volatile uint32_t value;
    lmem_pcccr_bits_t bits;
} lmem_pcccr_reg_t;


/** LMEM - Register Layout Typedef */
typedef struct {
  __IO uint32_t PCCCR;                             /**< Cache control register, offset: 0x0 */
  __IO uint32_t PCCLCR;                            /**< Cache line control register, offset: 0x4 */
  __IO uint32_t PCCSAR;                            /**< Cache search address register, offset: 0x8 */
  __IO uint32_t PCCCVR;                            /**< Cache read/write value register, offset: 0xC */
       uint8_t RESERVED_0[16];
  __IO uint32_t PCCRMR;                            /**< Cache regions mode register, offset: 0x20 */
} LMEM_Type, *LMEM_MemMapPtr;



// Module Control Register
typedef struct lpit_mcr_bits_t {
    volatile uint32_t M_CEN : 1;     // [0] Module clock enable
    volatile uint32_t SW_RST : 1;    // [1] Software reset
    volatile uint32_t DOZE_EN : 1;   // [2] DOZE mode enable: 0=stop timer channels in DOZE mode
    volatile uint32_t DBG_EN : 1;    // [3] Debug ena: 0=stop timer channels in Debug mode; 1=continue timer
    volatile uint32_t rsv31_4 : 28;  // [31:4]
} lpit_mcr_bits_t;

typedef union lpit_mcr_reg_t {
    volatile uint32_t value;
    lpit_mcr_bits_t bits;
} lpit_mcr_reg_t;

// Module Status Register
typedef struct lpit_msr_bits_t {
    volatile uint32_t TIF0 : 1;      // [0] Channel 0 Timer interrupt Flag
    volatile uint32_t TIF1 : 1;      // [1] Channel 1 Timer interrupt Flag
    volatile uint32_t TIF2 : 1;      // [2] Channel 2 Timer interrupt Flag
    volatile uint32_t TIF3 : 1;      // [3] Channel 3 Timer interrupt Flag
    volatile uint32_t rsv31_4 : 28;  // [31:4]
} lpit_msr_bits_t;

typedef union lpit_msr_reg_t {
    volatile uint32_t value;
    lpit_msr_bits_t bits;
} lpit_msr_reg_t;

/** LPIT - Register Layout Typedef */
typedef struct {
  __I  uint32_t VERID;                             /**< Version ID Register, offset: 0x0 */
  __I  uint32_t PARAM;                             /**< Parameter Register, offset: 0x4 */
  __IO uint32_t MCR;                               /**< Module Control Register, offset: 0x8 */
  __IO uint32_t MSR;                               /**< Module Status Register, offset: 0xC */
  __IO uint32_t MIER;                              /**< Module Interrupt Enable Register, offset: 0x10 */
  __IO uint32_t SETTEN;                            /**< Set Timer Enable Register, offset: 0x14 */
  __IO uint32_t CLRTEN;                            /**< Clear Timer Enable Register, offset: 0x18 */
       uint8_t RESERVED_0[4];
  struct {                                         /* offset: 0x20, array step: 0x10 */
    __IO uint32_t TVAL;                              /**< Timer Value Register, array offset: 0x20, array step: 0x10 */
    __I  uint32_t CVAL;                              /**< Current Timer Value, array offset: 0x24, array step: 0x10 */
    __IO uint32_t TCTRL;                             /**< Timer Control Register, array offset: 0x28, array step: 0x10 */
         uint8_t RESERVED_0[4];
  } TMR[4];
} LPIT_Type;



typedef struct lpuart_stat_bits_t {
    volatile uint32_t rsrv : 14;      // [13:0]
    volatile uint32_t MA2F : 1;       // [14] Match 2 flag
    volatile uint32_t MA1F : 1;       // [15] Match 1 flag
    volatile uint32_t PF : 1;         // [16] PF Parity Error flag
    volatile uint32_t FE : 1;         // [17] Framing Error flag
    volatile uint32_t NF : 1;         // [18] Noise Flag
    volatile uint32_t OR : 1;         // [19] Receiver Overrun Flag
    volatile uint32_t IDLE : 1;       // [20] Idle Line Flag
    volatile uint32_t RDRF : 1;       // [21] Receive Data Register Full Flag
    volatile uint32_t TC : 1;         // [22] Transmission Complete Flag
    volatile uint32_t TDRE : 1;       // [23] Transmit Data Register Empty Flag
    volatile uint32_t RAF : 1;        // [24] Receive Active Flag
    volatile uint32_t LBKDE : 1;      // [25] LIN Break Detection Enable
    volatile uint32_t BRK13 : 1;      // [26] Break Character Generation Length
    volatile uint32_t RWUID : 1;      // [27] Receive Wake Up Idle Detect
    volatile uint32_t RXINV : 1;      // [28] Receive Data Inversion
    volatile uint32_t MSBF : 1;       // [29] MSB First
    volatile uint32_t RXEDGIF : 1;    // [30] RXD Pin Active Edge Interrupt Flag
    volatile uint32_t LBKDIF : 1;     // [31] LIN Break Detect interrupt flag
} lpuart_stat_bits_t;

typedef union lpuart_stat_reg_t {
    lpuart_stat_bits_t bits;
    volatile uint32_t value;
} lpuart_stat_reg_t;

/** LPUART - Register Layout Typedef */
typedef struct {
  __I  uint32_t VERID;                             /**< Version ID Register, offset: 0x0 */
  __I  uint32_t PARAM;                             /**< Parameter Register, offset: 0x4 */
  __IO uint32_t GLOBAL;                            /**< LPUART Global Register, offset: 0x8 */
  __IO uint32_t PINCFG;                            /**< LPUART Pin Configuration Register, offset: 0xC */
  __IO uint32_t BAUD;                              /**< LPUART Baud Rate Register, offset: 0x10 */
  __IO uint32_t STAT;                              /**< LPUART Status Register, offset: 0x14 */
  __IO uint32_t CTRL;                              /**< LPUART Control Register, offset: 0x18 */
  __IO uint32_t DATA;                              /**< LPUART Data Register, offset: 0x1C */
  __IO uint32_t MATCH;                             /**< LPUART Match Address Register, offset: 0x20 */
  __IO uint32_t MODIR;                             /**< LPUART Modem IrDA Register, offset: 0x24 */
  __IO uint32_t FIFO;                              /**< LPUART FIFO Register, offset: 0x28 */
  __IO uint32_t WATER;                             /**< LPUART Watermark Register, offset: 0x2C */
} LPUART_Type;



typedef enum
{
  /* Auxiliary constants */
  NotAvail_IRQn                = -128,             /**< Not available device specific interrupt */

  /* Core interrupts */
  NonMaskableInt_IRQn          = -14,              /**< Non Maskable Interrupt */
  HardFault_IRQn               = -13,              /**< Cortex-M4 SV Hard Fault Interrupt */
  MemoryManagement_IRQn        = -12,              /**< Cortex-M4 Memory Management Interrupt */
  BusFault_IRQn                = -11,              /**< Cortex-M4 Bus Fault Interrupt */
  UsageFault_IRQn              = -10,              /**< Cortex-M4 Usage Fault Interrupt */
  SVCall_IRQn                  = -5,               /**< Cortex-M4 SV Call Interrupt */
  DebugMonitor_IRQn            = -4,               /**< Cortex-M4 Debug Monitor Interrupt */
  PendSV_IRQn                  = -2,               /**< Cortex-M4 Pend SV Interrupt */
  SysTick_IRQn                 = -1,               /**< Cortex-M4 System Tick Interrupt */

  /* Device specific interrupts */
  DMA0_IRQn                    = 0u,               /**< DMA channel 0 transfer complete */
  DMA1_IRQn                    = 1u,               /**< DMA channel 1 transfer complete */
  DMA2_IRQn                    = 2u,               /**< DMA channel 2 transfer complete */
  DMA3_IRQn                    = 3u,               /**< DMA channel 3 transfer complete */
  DMA4_IRQn                    = 4u,               /**< DMA channel 4 transfer complete */
  DMA5_IRQn                    = 5u,               /**< DMA channel 5 transfer complete */
  DMA6_IRQn                    = 6u,               /**< DMA channel 6 transfer complete */
  DMA7_IRQn                    = 7u,               /**< DMA channel 7 transfer complete */
  DMA8_IRQn                    = 8u,               /**< DMA channel 8 transfer complete */
  DMA9_IRQn                    = 9u,               /**< DMA channel 9 transfer complete */
  DMA10_IRQn                   = 10u,              /**< DMA channel 10 transfer complete */
  DMA11_IRQn                   = 11u,              /**< DMA channel 11 transfer complete */
  DMA12_IRQn                   = 12u,              /**< DMA channel 12 transfer complete */
  DMA13_IRQn                   = 13u,              /**< DMA channel 13 transfer complete */
  DMA14_IRQn                   = 14u,              /**< DMA channel 14 transfer complete */
  DMA15_IRQn                   = 15u,              /**< DMA channel 15 transfer complete */
  DMA_Error_IRQn               = 16u,              /**< DMA error interrupt channels 0-15 */
  MCM_IRQn                     = 17u,              /**< FPU sources */
  FTFC_IRQn                    = 18u,              /**< FTFC Command complete */
  Read_Collision_IRQn          = 19u,              /**< FTFC Read collision */
  LVD_LVW_IRQn                 = 20u,              /**< PMC Low voltage detect interrupt */
  FTFC_Fault_IRQn              = 21u,              /**< FTFC Double bit fault detect */
  WDOG_EWM_IRQn                = 22u,              /**< Single interrupt vector for WDOG and EWM */
  RCM_IRQn                     = 23u,              /**< RCM Asynchronous Interrupt */
  LPI2C0_Master_IRQn           = 24u,              /**< LPI2C0 Master Interrupt */
  LPI2C0_Slave_IRQn            = 25u,              /**< LPI2C0 Slave Interrupt */
  LPSPI0_IRQn                  = 26u,              /**< LPSPI0 Interrupt */
  LPSPI1_IRQn                  = 27u,              /**< LPSPI1 Interrupt */
  LPSPI2_IRQn                  = 28u,              /**< LPSPI2 Interrupt */
  LPI2C1_Master_IRQn           = 29u,              /**< LPI2C1 Master Interrupt */
  LPI2C1_Slave_IRQn            = 30u,              /**< LPI2C1 Slave Interrupt */
  LPUART0_RxTx_IRQn            = 31u,              /**< LPUART0 Transmit / Receive Interrupt */
  LPUART1_RxTx_IRQn            = 33u,              /**< LPUART1 Transmit / Receive  Interrupt */
  LPUART2_RxTx_IRQn            = 35u,              /**< LPUART2 Transmit / Receive  Interrupt */
  ADC0_IRQn                    = 39u,              /**< ADC0 interrupt request. */
  ADC1_IRQn                    = 40u,              /**< ADC1 interrupt request. */
  CMP0_IRQn                    = 41u,              /**< CMP0 interrupt request */
  ERM_single_fault_IRQn        = 44u,              /**< ERM single bit error correction */
  ERM_double_fault_IRQn        = 45u,              /**< ERM double bit error non-correctable */
  RTC_IRQn                     = 46u,              /**< RTC alarm interrupt */
  RTC_Seconds_IRQn             = 47u,              /**< RTC seconds interrupt */
  LPIT0_Ch0_IRQn               = 48u,              /**< LPIT0 channel 0 overflow interrupt */
  LPIT0_Ch1_IRQn               = 49u,              /**< LPIT0 channel 1 overflow interrupt */
  LPIT0_Ch2_IRQn               = 50u,              /**< LPIT0 channel 2 overflow interrupt */
  LPIT0_Ch3_IRQn               = 51u,              /**< LPIT0 channel 3 overflow interrupt */
  PDB0_IRQn                    = 52u,              /**< PDB0 interrupt */
  SAI1_Tx_IRQn                 = 55u,              /**< SAI1 Transmit Synchronous interrupt (for interrupt controller) */
  SAI1_Rx_IRQn                 = 56u,              /**< SAI1 Receive Synchronous interrupt (for interrupt controller) */
  SCG_IRQn                     = 57u,              /**< SCG bus interrupt request */
  LPTMR0_IRQn                  = 58u,              /**< LPTIMER interrupt request */
  PORTA_IRQn                   = 59u,              /**< Port A pin detect interrupt */
  PORTB_IRQn                   = 60u,              /**< Port B pin detect interrupt */
  PORTC_IRQn                   = 61u,              /**< Port C pin detect interrupt */
  PORTD_IRQn                   = 62u,              /**< Port D pin detect interrupt */
  PORTE_IRQn                   = 63u,              /**< Port E pin detect interrupt */
  SWI_IRQn                     = 64u,              /**< Software interrupt */
  QSPI_IRQn                    = 65u,              /**< QSPI All interrupts ORed output */
  PDB1_IRQn                    = 68u,              /**< PDB1 interrupt */
  FLEXIO_IRQn                  = 69u,              /**< FlexIO Interrupt */
  SAI0_Tx_IRQn                 = 70u,              /**< SAI0 Transmit Synchronous interrupt (for interrupt controller) */
  SAI0_Rx_IRQn                 = 71u,              /**< SAI0 Receive Synchronous interrupt (for interrupt controller) */
  ENET_TIMER_IRQn              = 72u,              /**< ENET 1588 Timer Interrupt - synchronous */
  ENET_TX_IRQn                 = 73u,              /**< ENET Data transfer done */
  ENET_RX_IRQn                 = 74u,              /**< ENET Receive Buffer Done for Ring/Queue 0 */
  ENET_ERR_IRQn                = 75u,              /**< ENET Payload receive error. */
  ENET_STOP_IRQn               = 76u,              /**< ENET Graceful stop */
  ENET_WAKE_IRQn               = 77u,              /**< ENET Wake from sleep. */
  CAN0_ORed_IRQn               = 78u,              /**< CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
  CAN0_Error_IRQn              = 79u,              /**< CAN0 Interrupt indicating that errors were detected on the CAN bus */
  CAN0_Wake_Up_IRQn            = 80u,              /**< CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode */
  CAN0_ORed_0_15_MB_IRQn       = 81u,              /**< CAN0 OR'ed Message buffer (0-15) */
  CAN0_ORed_16_31_MB_IRQn      = 82u,              /**< CAN0 OR'ed Message buffer (16-31) */
  CAN1_ORed_IRQn               = 85u,              /**< CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
  CAN1_Error_IRQn              = 86u,              /**< CAN1 Interrupt indicating that errors were detected on the CAN bus */
  CAN1_ORed_0_15_MB_IRQn       = 88u,              /**< CAN1 OR'ed Interrupt for Message buffer (0-15) */
  CAN1_ORed_16_31_MB_IRQn      = 89u,              /**< CAN1 OR'ed Interrupt for Message buffer (16-31) */
  CAN2_ORed_IRQn               = 92u,              /**< CAN2 OR'ed [Bus Off OR Transmit Warning OR Receive Warning] */
  CAN2_Error_IRQn              = 93u,              /**< CAN2 Interrupt indicating that errors were detected on the CAN bus */
  CAN2_ORed_0_15_MB_IRQn       = 95u,              /**< CAN2 OR'ed Message buffer (0-15) */
  CAN2_ORed_16_31_MB_IRQn      = 96u,              /**< CAN2 OR'ed Message buffer (16-31) */
  FTM0_Ch0_Ch1_IRQn            = 99u,              /**< FTM0 Channel 0 and 1 interrupt */
  FTM0_Ch2_Ch3_IRQn            = 100u,             /**< FTM0 Channel 2 and 3 interrupt */
  FTM0_Ch4_Ch5_IRQn            = 101u,             /**< FTM0 Channel 4 and 5 interrupt */
  FTM0_Ch6_Ch7_IRQn            = 102u,             /**< FTM0 Channel 6 and 7 interrupt */
  FTM0_Fault_IRQn              = 103u,             /**< FTM0 Fault interrupt */
  FTM0_Ovf_Reload_IRQn         = 104u,             /**< FTM0 Counter overflow and Reload interrupt */
  FTM1_Ch0_Ch1_IRQn            = 105u,             /**< FTM1 Channel 0 and 1 interrupt */
  FTM1_Ch2_Ch3_IRQn            = 106u,             /**< FTM1 Channel 2 and 3 interrupt */
  FTM1_Ch4_Ch5_IRQn            = 107u,             /**< FTM1 Channel 4 and 5 interrupt */
  FTM1_Ch6_Ch7_IRQn            = 108u,             /**< FTM1 Channel 6 and 7 interrupt */
  FTM1_Fault_IRQn              = 109u,             /**< FTM1 Fault interrupt */
  FTM1_Ovf_Reload_IRQn         = 110u,             /**< FTM1 Counter overflow and Reload interrupt */
  FTM2_Ch0_Ch1_IRQn            = 111u,             /**< FTM2 Channel 0 and 1 interrupt */
  FTM2_Ch2_Ch3_IRQn            = 112u,             /**< FTM2 Channel 2 and 3 interrupt */
  FTM2_Ch4_Ch5_IRQn            = 113u,             /**< FTM2 Channel 4 and 5 interrupt */
  FTM2_Ch6_Ch7_IRQn            = 114u,             /**< FTM2 Channel 6 and 7 interrupt */
  FTM2_Fault_IRQn              = 115u,             /**< FTM2 Fault interrupt */
  FTM2_Ovf_Reload_IRQn         = 116u,             /**< FTM2 Counter overflow and Reload interrupt */
  FTM3_Ch0_Ch1_IRQn            = 117u,             /**< FTM3 Channel 0 and 1 interrupt */
  FTM3_Ch2_Ch3_IRQn            = 118u,             /**< FTM3 Channel 2 and 3 interrupt */
  FTM3_Ch4_Ch5_IRQn            = 119u,             /**< FTM3 Channel 4 and 5 interrupt */
  FTM3_Ch6_Ch7_IRQn            = 120u,             /**< FTM3 Channel 6 and 7 interrupt */
  FTM3_Fault_IRQn              = 121u,             /**< FTM3 Fault interrupt */
  FTM3_Ovf_Reload_IRQn         = 122u,             /**< FTM3 Counter overflow and Reload interrupt */
  FTM4_Ch0_Ch1_IRQn            = 123u,             /**< FTM4 Channel 0 and 1 interrupt */
  FTM4_Ch2_Ch3_IRQn            = 124u,             /**< FTM4 Channel 2 and 3 interrupt */
  FTM4_Ch4_Ch5_IRQn            = 125u,             /**< FTM4 Channel 4 and 5 interrupt */
  FTM4_Ch6_Ch7_IRQn            = 126u,             /**< FTM4 Channel 6 and 7 interrupt */
  FTM4_Fault_IRQn              = 127u,             /**< FTM4 Fault interrupt */
  FTM4_Ovf_Reload_IRQn         = 128u,             /**< FTM4 Counter overflow and Reload interrupt */
  FTM5_Ch0_Ch1_IRQn            = 129u,             /**< FTM5 Channel 0 and 1 interrupt */
  FTM5_Ch2_Ch3_IRQn            = 130u,             /**< FTM5 Channel 2 and 3 interrupt */
  FTM5_Ch4_Ch5_IRQn            = 131u,             /**< FTM5 Channel 4 and 5 interrupt */
  FTM5_Ch6_Ch7_IRQn            = 132u,             /**< FTM5 Channel 6 and 7 interrupt */
  FTM5_Fault_IRQn              = 133u,             /**< FTM5 Fault interrupt */
  FTM5_Ovf_Reload_IRQn         = 134u,             /**< FTM5 Counter overflow and Reload interrupt */
  FTM6_Ch0_Ch1_IRQn            = 135u,             /**< FTM6 Channel 0 and 1 interrupt */
  FTM6_Ch2_Ch3_IRQn            = 136u,             /**< FTM6 Channel 2 and 3 interrupt */
  FTM6_Ch4_Ch5_IRQn            = 137u,             /**< FTM6 Channel 4 and 5 interrupt */
  FTM6_Ch6_Ch7_IRQn            = 138u,             /**< FTM6 Channel 6 and 7 interrupt */
  FTM6_Fault_IRQn              = 139u,             /**< FTM6 Fault interrupt */
  FTM6_Ovf_Reload_IRQn         = 140u,             /**< FTM6 Counter overflow and Reload interrupt */
  FTM7_Ch0_Ch1_IRQn            = 141u,             /**< FTM7 Channel 0 and 1 interrupt */
  FTM7_Ch2_Ch3_IRQn            = 142u,             /**< FTM7 Channel 2 and 3 interrupt */
  FTM7_Ch4_Ch5_IRQn            = 143u,             /**< FTM7 Channel 4 and 5 interrupt */
  FTM7_Ch6_Ch7_IRQn            = 144u,             /**< FTM7 Channel 6 and 7 interrupt */
  FTM7_Fault_IRQn              = 145u,             /**< FTM7 Fault interrupt */
  FTM7_Ovf_Reload_IRQn         = 146u              /**< FTM7 Counter overflow and Reload interrupt */
} IRQn_Type;


/** S32_NVIC - Size of Registers Arrays */
#define S32_NVIC_ISER_COUNT                      8u
#define S32_NVIC_ICER_COUNT                      8u
#define S32_NVIC_ISPR_COUNT                      8u
#define S32_NVIC_ICPR_COUNT                      8u
#define S32_NVIC_IABR_COUNT                      8u
#define S32_NVIC_IP_COUNT                        240u

typedef struct {
  __IO uint32_t ISER[S32_NVIC_ISER_COUNT];         /**< Interrupt Set Enable Register n, array offset: 0x0, array step: 0x4 */
       uint8_t RESERVED_0[96];
  __IO uint32_t ICER[S32_NVIC_ICER_COUNT];         /**< Interrupt Clear Enable Register n, array offset: 0x80, array step: 0x4 */
       uint8_t RESERVED_1[96];
  __IO uint32_t ISPR[S32_NVIC_ISPR_COUNT];         /**< Interrupt Set Pending Register n, array offset: 0x100, array step: 0x4 */
       uint8_t RESERVED_2[96];
  __IO uint32_t ICPR[S32_NVIC_ICPR_COUNT];         /**< Interrupt Clear Pending Register n, array offset: 0x180, array step: 0x4 */
       uint8_t RESERVED_3[96];
  __IO uint32_t IABR[S32_NVIC_IABR_COUNT];         /**< Interrupt Active bit Register n, array offset: 0x200, array step: 0x4 */
       uint8_t RESERVED_4[224];
  __IO uint8_t IP[S32_NVIC_IP_COUNT];              /**< Interrupt Priority Register n, array offset: 0x300, array step: 0x1 */
       uint8_t RESERVED_5[2576];
  __O  uint32_t STIR;                              /**< Software Trigger Interrupt Register, offset: 0xE00 */
} S32_NVIC_Type;




/** PCC_Peripheral_Access_Layer PCC Peripheral Access Layer */
#define PCC_FTFC_INDEX                           32
#define PCC_DMAMUX_INDEX                         33
#define PCC_FlexCAN0_INDEX                       36
#define PCC_FlexCAN1_INDEX                       37
#define PCC_FTM3_INDEX                           38
#define PCC_ADC1_INDEX                           39
#define PCC_FlexCAN2_INDEX                       43
#define PCC_LPSPI0_INDEX                         44
#define PCC_LPSPI1_INDEX                         45
#define PCC_LPSPI2_INDEX                         46
#define PCC_PDB1_INDEX                           49
#define PCC_CRC_INDEX                            50
#define PCC_PDB0_INDEX                           54
#define PCC_LPIT_INDEX                           55
#define PCC_FTM0_INDEX                           56
#define PCC_FTM1_INDEX                           57
#define PCC_FTM2_INDEX                           58
#define PCC_ADC0_INDEX                           59
#define PCC_RTC_INDEX                            61
#define PCC_LPTMR0_INDEX                         64
#define PCC_PORTA_INDEX                          73
#define PCC_PORTB_INDEX                          74
#define PCC_PORTC_INDEX                          75
#define PCC_PORTD_INDEX                          76
#define PCC_PORTE_INDEX                          77
#define PCC_SAI0_INDEX                           84
#define PCC_SAI1_INDEX                           85
#define PCC_FlexIO_INDEX                         90
#define PCC_EWM_INDEX                            97
#define PCC_LPI2C0_INDEX                         102
#define PCC_LPI2C1_INDEX                         103
#define PCC_LPUART0_INDEX                        106
#define PCC_LPUART1_INDEX                        107
#define PCC_LPUART2_INDEX                        108
#define PCC_FTM4_INDEX                           110
#define PCC_FTM5_INDEX                           111
#define PCC_FTM6_INDEX                           112
#define PCC_FTM7_INDEX                           113
#define PCC_CMP0_INDEX                           115
#define PCC_QSPI_INDEX                           118
#define PCC_ENET_INDEX                           121

#define PCC_PCCn_COUNT                           122u

typedef struct pcc_bits_t {
    volatile uint32_t zero0 : 3;      // [2:0]
    volatile uint32_t zero1 : 1;      // [3]
    volatile uint32_t zero2 : 20;     // [23:4]
    volatile uint32_t PCS : 3;        // [26:24] Can be writen when CGC=0; 0=Clock is off; 1..7=Clock options
    volatile uint32_t zero3 : 2;      // [28:27]
    volatile uint32_t rsrv : 1;       // [29]
    volatile uint32_t CGC : 1;        // [30] Clock Gate Control: 0=clk disabled; 1=enabled
    volatile uint32_t PR : 1;         // [31] 0=periph is not presence; 1=presence
} pcc_bits_t;

typedef union pcc_reg_t {
    pcc_bits_t bits;
    volatile uint32_t value;
} pcc_reg_t;

typedef union {
  __IO uint32_t PCCn[PCC_PCCn_COUNT];
} PCC_Type;




/** GPIO - Register Layout Typedef */
typedef struct {
  __IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
  __O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
  __O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
  __IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
  __IO uint32_t PIDR;                              /**< Port Input Disable Register, offset: 0x18 */
} GPIO_Type;

typedef struct port_pcr_bits_t {
    volatile uint32_t PS : 1;        // [0] Pull Select: 0=pulldown 1=pullup enabled if PE=1
    volatile uint32_t PE : 1;        // [1] Pull Enable
    volatile uint32_t rsv3_2 : 2;    // [3:2]
    volatile uint32_t PFE : 1;       // [4] Passive Filter Enable
    volatile uint32_t rsv5 : 1;      // [5]
    volatile uint32_t DSE : 1;       // [6] Drive Strength Enable
    volatile uint32_t rsv7 : 1;      // [7]
    volatile uint32_t MUX : 3;       // [10:8] 000=Pin disabled; 001=GPIO
    volatile uint32_t rsv14_11 : 4;  // [14:11]
    volatile uint32_t LK : 1;        // [15] Lock Register
    volatile uint32_t IRQC : 4;      // [19:16] IRQ config: 0=irq flag disabled
    volatile uint32_t rsv23_20 : 4;  // [23:20]
    volatile uint32_t ISF : 1;       // [24] ISF: Interrupt Status Flag
    volatile uint32_t rsv31_25 : 7;  // [31:25]
} port_pcr_bits_t;

typedef union port_pcr_reg_t {
    volatile uint32_t value;
    port_pcr_bits_t bits;
} port_pcr_reg_t;

/** PORT - Register Layout Typedef */
typedef struct {
  __IO uint32_t PCR[32];                           /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  __O  uint32_t GPCLR;                             /**< Global Pin Control Low Register, offset: 0x80 */
  __O  uint32_t GPCHR;                             /**< Global Pin Control High Register, offset: 0x84 */
  __O  uint32_t GICLR;                             /**< Global Interrupt Control Low Register, offset: 0x88 */
  __O  uint32_t GICHR;                             /**< Global Interrupt Control High Register, offset: 0x8C */
       uint8_t RESERVED_0[16];
  __IO uint32_t ISFR;                              /**< Interrupt Status Flag Register, offset: 0xA0 */
       uint8_t RESERVED_1[28];
  __IO uint32_t DFER;                              /**< Digital Filter Enable Register, offset: 0xC0 */
  __IO uint32_t DFCR;                              /**< Digital Filter Clock Register, offset: 0xC4 */
  __IO uint32_t DFWR;                              /**< Digital Filter Width Register, offset: 0xC8 */
} PORT_Type;



// Coprocessor Access Control register
typedef struct scb_cpacr_bits_t {
    volatile uint32_t rsv19_0 : 20;    // [19:0]
    volatile uint32_t CP10 : 2;        // [21:20] Access privileges: 0=denied; 1=priv only; 2=rsrv; 3=full access
    volatile uint32_t CP11 : 2;        // [23:22] Access privileges: 0=denied; 1=priv only; 2=rsrv; 3=full access
    volatile uint32_t rsv31_24 : 8;    // [31:24]
} scb_cpacr_bits_t;

typedef union scb_cpacr_reg_t {
    volatile uint32_t value;            // reset value 0000_0000h
    scb_cpacr_bits_t bits;
} scb_cpacr_reg_t;

// Floating-point Context Control register
typedef struct scb_fpccr_bits_t {
    volatile uint32_t LSPACT : 1;       // [0]
    volatile uint32_t USER : 1;         // [1]
    volatile uint32_t rsrv2 : 1;        // [2]
    volatile uint32_t THREAD : 1;       // [3]
    volatile uint32_t HFRDY : 1;        // [4]
    volatile uint32_t MMRDY : 1;        // [5] 0=MemManage is disabled
    volatile uint32_t BFRDY : 1;        // [6] 0=BusFault is disabled
    volatile uint32_t rsrv8 : 1;        // [7]
    volatile uint32_t MONRDY : 1;       // [8] 0=Debug monitor is disabled
    volatile uint32_t rsv29_9 : 21;     // [29:9]
    volatile uint32_t LSPEN : 1;        // [30] Enable lazy state preservation
    volatile uint32_t ASPEN : 1;        // [31] Enable CONTROL<2> setting on exec of fpu
} scb_fpccr_bits_t;

typedef union scb_fpccr_reg_t {
    volatile uint32_t value;            // reset value C000_0000h
    scb_fpccr_bits_t bits;
} scb_fpccr_reg_t;


/** S32_SCB - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[8];
  __IO uint32_t ACTLR;                             /**< Auxiliary Control Register,, offset: 0x8 */
       uint8_t RESERVED_1[3316];
  __I  uint32_t CPUID;                             /**< CPUID Base Register, offset: 0xD00 */
  __IO uint32_t ICSR;                              /**< Interrupt Control and State Register, offset: 0xD04 */
  __IO uint32_t VTOR;                              /**< Vector Table Offset Register, offset: 0xD08 */
  __IO uint32_t AIRCR;                             /**< Application Interrupt and Reset Control Register, offset: 0xD0C */
  __IO uint32_t SCR;                               /**< System Control Register, offset: 0xD10 */
  __IO uint32_t CCR;                               /**< Configuration and Control Register, offset: 0xD14 */
  __IO uint32_t SHPR1;                             /**< System Handler Priority Register 1, offset: 0xD18 */
  __IO uint32_t SHPR2;                             /**< System Handler Priority Register 2, offset: 0xD1C */
  __IO uint32_t SHPR3;                             /**< System Handler Priority Register 3, offset: 0xD20 */
  __IO uint32_t SHCSR;                             /**< System Handler Control and State Register, offset: 0xD24 */
  __IO uint32_t CFSR;                              /**< Configurable Fault Status Registers, offset: 0xD28 */
  __IO uint32_t HFSR;                              /**< HardFault Status register, offset: 0xD2C */
  __IO uint32_t DFSR;                              /**< Debug Fault Status Register, offset: 0xD30 */
  __IO uint32_t MMFAR;                             /**< MemManage Address Register, offset: 0xD34 */
  __IO uint32_t BFAR;                              /**< BusFault Address Register, offset: 0xD38 */
  __IO uint32_t AFSR;                              /**< Auxiliary Fault Status Register, offset: 0xD3C */
       uint8_t RESERVED_2[72];
  __IO uint32_t CPACR;                             /**< Coprocessor Access Control Register, offset: 0xD88 */
       uint8_t RESERVED_3[424];
  __IO uint32_t FPCCR;                             /**< Floating-point Context Control Register, offset: 0xF34 */
  __IO uint32_t FPCAR;                             /**< Floating-point Context Address Register, offset: 0xF38 */
  __IO uint32_t FPDSCR;                            /**< Floating-point Default Status Control Register, offset: 0xF3C */
} S32_SCB_Type;


// System Oscillator Configuration register
typedef struct scg_sosccfg_bits_t {
    volatile uint32_t rsv1_0 : 2;    // [1:0]
    volatile uint32_t EREFS : 1;     // [2] External ref select: 0=ext; internal OSC
    volatile uint32_t HGO : 1;       // [3] High gain oscillator select: 0=low-gain;1=high-gain
    volatile uint32_t RANGE : 2;     // [5:4] System OSC range select: 0=reserved; 1=Low range; 2=medium; 3=high
    volatile uint32_t rsv31_6 : 26;  // [7:3]
} scg_sosccfg_bits_t;

typedef union scg_sosccfg_reg_t {
    volatile uint32_t value;         // reset value 0000_0010h
    scg_sosccfg_bits_t bits;
} scg_sosccfg_reg_t;

// System OSC Divide register
typedef struct scg_soscdiv_bits_t {
    volatile uint32_t SOSCDIV1 : 3;  // [2:0] System OSC Clock Divide 1: 0=disabled; 1=div by 1; ..
    volatile uint32_t rsv7_3 : 5;    // [7:3]
    volatile uint32_t SOSCDIV2 : 3;  // [10:8] System OSC Clock Divide 2: 0=disabled; 1=div by 1; ..
    volatile uint32_t rsv31_11 : 21; // [31:11]
} scg_soscdiv_bits_t;

typedef union scg_soscdiv_reg_t {
    volatile uint32_t value;         // reset value 0000_0000h
    scg_soscdiv_bits_t bits;
} scg_soscdiv_reg_t;

// System OSC Control Status register
typedef struct scg_sosccsr_bits_t {
    volatile uint32_t SOSCEN : 1;    // [0] System OSC Enable: 1=enabled
    volatile uint32_t rsv15_1 : 15;  // [15:1]
    volatile uint32_t SOSCCM : 1;    // [16] System OSC Clock Monitor: 1=Enabled
    volatile uint32_t SOSCCMRE : 1;  // [17] System OSC Clock Monitor Reset Enable
    volatile uint32_t rsv22_18 : 5;  // [22:18]
    volatile uint32_t LK : 1;        // [23] Lock Register: 1=register cannot be writen
    volatile uint32_t SOSCVLD : 1;   // [24] System OSC Valid
    volatile uint32_t SOSCSEL : 1;   // [25] System OSC selected: 0System OSC is not the system clk source
    volatile uint32_t SOSCERR : 1;   // [26] System OSC Clock Error
    volatile uint32_t rsv31_27 : 5;  // [31:27]
} scg_sosccsr_bits_t;

typedef union scg_sosccsr_reg_t {
    volatile uint32_t value;
    scg_sosccsr_bits_t bits;
} scg_sosccsr_reg_t;

// System PLL Control Status register
typedef struct scg_spllcsr_bits_t {
    volatile uint32_t SPLLEN : 1;       // [0] System PLL enable: 0=disabled
    volatile uint32_t rsv15_1 : 15;     // [15:1]
    volatile uint32_t SPLLCM : 1;       // [16] Clock Monitor: 0=disabled
    volatile uint32_t SPLLCMRE : 1;     // [17] Clock Monitor reset enable: 0=generate irq on error; 1=reset on error
    volatile uint32_t rsv22_18 : 5;     // [22:18]
    volatile uint32_t LK : 1;           // [23] Lock: 0=can be written; 1=cannot
    volatile uint32_t SPLLVLD : 1;      // [24] System PLL valid 
    volatile uint32_t SPLLSEL : 1;      // [25] System PLL selected: 0=not the system clock
    volatile uint32_t SPLLERR : 1;      // [26] System PLL clock error
    volatile uint32_t rsv31_27 : 5;     // [31:27]
} scg_spllcsr_bits_t;

typedef union scg_spllcsr_reg_t {
    volatile uint32_t value;         // reset value 0000_0000h
    scg_spllcsr_bits_t bits;
} scg_spllcsr_reg_t;

// Run Clock Control register/ Clock Status register has the same layout
typedef struct scg_rccr_bits_t {
    volatile uint32_t DIVSLOW : 4;      // [3:0] Slow clock div ratio:0=div by 1; 0x7=div by 8
    volatile uint32_t DIVBUS : 4;       // [7:4] Bus clock div ratio:0=div by 1; 0xf=div by 16
    volatile uint32_t rsv15_8 : 8;      // [15:8]
    volatile uint32_t DIVCORE : 4;      // [19:16] Core clock div ratio:0=div by 1; 0xf=div by 16
    volatile uint32_t rsv23_20 : 4;     // [23:20]
    volatile uint32_t SCS : 4;          // [27:24] Clock source: 3=Fast IRC (FIRC_CLK); 6=SPLL_CLK
    volatile uint32_t rsv31_28 : 4;     // [31:28]
} scg_rccr_bits_t;

typedef union scg_rccr_reg_t {
    volatile uint32_t value;         // reset value 030*_0001h
    scg_rccr_bits_t bits;
} scg_rccr_reg_t;


/** SCG - Register Layout Typedef */
typedef struct {
  __I  uint32_t VERID;                             /**< Version ID Register, offset: 0x0 */
  __I  uint32_t PARAM;                             /**< Parameter Register, offset: 0x4 */
       uint8_t RESERVED_0[8];
  __I  uint32_t CSR;                               /**< Clock Status Register, offset: 0x10 */
  __IO uint32_t RCCR;                              /**< Run Clock Control Register, offset: 0x14 */
  __IO uint32_t VCCR;                              /**< VLPR Clock Control Register, offset: 0x18 */
  __IO uint32_t HCCR;                              /**< HSRUN Clock Control Register, offset: 0x1C */
  __IO uint32_t CLKOUTCNFG;                        /**< SCG CLKOUT Configuration Register, offset: 0x20 */
       uint8_t RESERVED_1[220];
  __IO uint32_t SOSCCSR;                           /**< System OSC Control Status Register, offset: 0x100 */
  __IO uint32_t SOSCDIV;                           /**< System OSC Divide Register, offset: 0x104 */
  __IO uint32_t SOSCCFG;                           /**< System Oscillator Configuration Register, offset: 0x108 */
       uint8_t RESERVED_2[244];
  __IO uint32_t SIRCCSR;                           /**< Slow IRC Control Status Register, offset: 0x200 */
  __IO uint32_t SIRCDIV;                           /**< Slow IRC Divide Register, offset: 0x204 */
  __IO uint32_t SIRCCFG;                           /**< Slow IRC Configuration Register, offset: 0x208 */
       uint8_t RESERVED_3[244];
  __IO uint32_t FIRCCSR;                           /**< Fast IRC Control Status Register, offset: 0x300 */
  __IO uint32_t FIRCDIV;                           /**< Fast IRC Divide Register, offset: 0x304 */
  __IO uint32_t FIRCCFG;                           /**< Fast IRC Configuration Register, offset: 0x308 */
       uint8_t RESERVED_4[756];
  __IO uint32_t SPLLCSR;                           /**< System PLL Control Status Register, offset: 0x600 */
  __IO uint32_t SPLLDIV;                           /**< System PLL Divide Register, offset: 0x604 */
  __IO uint32_t SPLLCFG;                           /**< System PLL Configuration Register, offset: 0x608 */
} SCG_Type;


// Special CNT values:
// All other writes to register CNT are illegal and force a reset
#define WDOG_UNLOCK_SEQUENCE  0xD928C520
#define WDOG_REFRESH_SEQUENCE 0xB480A602

// Watchdog Control and Status register
typedef struct wdog_cs_bits_t {
    volatile uint32_t STOP : 1;         // [0] Stop enable: 0=wdog disabled in stop mode
    volatile uint32_t WAIT : 1;         // [1] Wait enable: 0=wdog disabled in wait mode
    volatile uint32_t DBG : 1;          // [2] Debug enable: 0=wdog disabled in debug mode
    volatile uint32_t TST : 2;          // [4:3] Fast Test mode enable
    volatile uint32_t UPDATE : 1;       // [5] Allow updates: 0=not allowed
    volatile uint32_t INT : 1;          // [6] Interrupt: 0=disabled
    volatile uint32_t EN : 1;           // [7] Enable: 1=enabled
    volatile uint32_t CLK : 2;          // [9:8] Clock: 0=bus; 1=LPO; 2=Internal clock; 3=external ref clock
    volatile uint32_t RCS : 1;          // [10] Reconfiguration success
    volatile uint32_t ULK : 1;          // [11] Unlock status: 1=wdog is unlocked
    volatile uint32_t PRES : 1;         // [12] Prescale: 0=256 prescale disabled
    volatile uint32_t CMD32EN : 1;      // [13] 32-bits refresh/unlock commmand: 1=enabled
    volatile uint32_t FLG : 1;          // [14] Interrupt flag: 0=no interrupt occured
    volatile uint32_t WIN : 1;          // [15] Watchdog window: 0=disabled
    volatile uint32_t rsv31_16 : 16;    // [31:16]
} wdog_cs_bits_t;

typedef union wdog_cs_reg_t {
    volatile uint32_t value;            // reset value 0000_2980h
    wdog_cs_bits_t bits;
} wdog_cs_reg_t;

typedef struct {
    __IO uint32_t CS;                   // Watchdog Control and Status Register, offset: 0x0
    __IO uint32_t CNT;                  // Watchdog Counter Register, offset: 0x4
    __IO uint32_t TOVAL;                // Watchdog Timeout Value Register, offset: 0x8
    __IO uint32_t WIN;                  // Watchdog Window Register, offset: 0xC
} WDOG_Type;





// S32K148EVB board
#define LED_RED		21U
#define LED_GREEN	22U
#define LED_BLUE	23U
#define SW3		12U  // PTC12
#define SW4		13U  // PTC13


#define CAN0_BASE                                (0x40024000u)
#define CAN1_BASE                                (0x40025000u)
#define CAN2_BASE                                (0x4002B000u)
#define LPIT0_BASE                               (0x40037000u)
#define PORTA_BASE                               (0x40049000u)
#define PORTB_BASE                               (0x4004A000u)
#define PORTC_BASE                               (0x4004B000u)
#define PORTD_BASE                               (0x4004C000u)
#define PORTE_BASE                               (0x4004D000u)
#define WDOG_BASE                                (0x40052000u)
#define SCG_BASE                                 (0x40064000u)
#define PCC_BASE                                 (0x40065000u)
#define LPUART0_BASE                             (0x4006A000u)
#define LPUART1_BASE                             (0x4006B000u)
#define LPUART2_BASE                             (0x4006C000u)
#define PTA_BASE                                 (0x400FF000u)
#define PTC_BASE                                 (0x400FF080u)
#define PTD_BASE                                 (0x400FF0C0u)
#define PTE_BASE                                 (0x400FF100u)
#define S32_SCB_BASE                             (0xE000E000u)
#define S32_NVIC_BASE                            (0xE000E100u)
#define LMEM_BASE                                (0xE0082000u)

/* LPIT - Peripheral instance base addresses */
#define LPIT0                                    ((LPIT_Type *)LPIT0_BASE)
/* PORT - Peripheral instance base addresses */
#define PORTA                                    ((PORT_Type *)PORTA_BASE)
#define PORTB                                    ((PORT_Type *)PORTB_BASE)
#define PORTC                                    ((PORT_Type *)PORTC_BASE)
#define PORTD                                    ((PORT_Type *)PORTD_BASE)
#define PORTE                                    ((PORT_Type *)PORTE_BASE)
/* GPIO - Peripheral instance base addresses */
#define PTA                                      ((GPIO_Type *)PTA_BASE)
#define PTB                                      ((GPIO_Type *)PTB_BASE)
#define PTC                                      ((GPIO_Type *)PTC_BASE)
#define PTD                                      ((GPIO_Type *)PTD_BASE)
#define PTE                                      ((GPIO_Type *)PTE_BASE)
/* PCC */
#define PCC                                      ((PCC_Type *)PCC_BASE)
/* NVIC */
#define S32_NVIC                                 ((S32_NVIC_Type *)S32_NVIC_BASE)
/* S32_SCB - Peripheral instance base addresses */
#define S32_SCB                                  ((S32_SCB_Type *)S32_SCB_BASE)
/* WDOG */
#define WDOG                                     ((WDOG_Type *)WDOG_BASE)
/* SCG - Peripheral instance base addresses */
#define SCG                                      ((SCG_Type *)SCG_BASE)
/* Peripheral LPUARTx base address */
#define LPUART0                                  ((LPUART_Type *)LPUART0_BASE)
#define LPUART1                                  ((LPUART_Type *)LPUART1_BASE)
#define LPUART2                                  ((LPUART_Type *)LPUART2_BASE)
/* Peripheral LMEM base pointer */
#define LMEM                                     ((LMEM_Type *)LMEM_BASE)
/* CAN - Peripheral instance base addresses */
#define CAN0                                     ((CAN_Type *)CAN0_BASE)
#define CAN1                                     ((CAN_Type *)CAN1_BASE)
#define CAN2                                     ((CAN_Type *)CAN2_BASE)
