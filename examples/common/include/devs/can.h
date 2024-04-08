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

// Master Control Register
typedef struct CAN_MCR_bits_type {
    volatile uint32_t INRQ : 1;        // [0] RW: Initialization request: 1=enter init mode request
    volatile uint32_t SLEEP : 1;       // [1] RW: Sleep mode request: 1=set to sleep mode (bit is set after reset)
    volatile uint32_t TXFP : 1;        // [2] RW: Transmit FIFO priority: 0=prio driven by ID; 1=prio driven by request order
    volatile uint32_t RFLM : 1;        // [3] RW: Receive FIFO loceked mode: 0=Rx FIFO not locked on overrun; 1=overrun msg discarded
    volatile uint32_t NART : 1;        // [4] RW: No automatic retransmission: 1=message is transmitted only once; 0=CAN automatically retransmits the msg until success
    volatile uint32_t AWUM : 1;        // [5] RW: Automatic wake-up mode: 0=controlled by software
    volatile uint32_t ABOM : 1;        // [6] RW: Automatic bus-off management: (32.7.6) 0=controlled by software
    volatile uint32_t TTCM : 1;        // [7] RW: Time triggered communication mode: 0=disabled; 1=enabled
    volatile uint32_t rsrv14_8 : 7;    // [14:8]
    volatile uint32_t RESET : 1;       // [15] RS: bxCAN sofware master reset: 0=normal operation; 1=force master to reset. This bit is automatically reset to 0.
    volatile uint32_t DBF : 1;         // [16] RW: Debug Freez. 0=CAN working during debug; 1=tx/tx frozen
    volatile uint32_t rsrv31_17 : 15;  // [31:17]
} CAN_MCR_bits_type;

typedef union CAN_MCR_type {
    volatile uint32_t val;
    CAN_MCR_bits_type b;
} CAN_MCR_type;


// Master Status Register
typedef struct CAN_MSR_bits_type {
    volatile uint32_t INAK : 1;        // [0] R: Initialization acknowledge: 1=indicates that CAN in initialization mode
    volatile uint32_t SLAK : 1;        // [1] R: Sleep acknowledge: 1=indicates that CAN in Sleep mode
    volatile uint32_t ERRI : 1;        // [2] RC_W1: Error interrupt: Set by hardware when ESR is set
    volatile uint32_t WKUI : 1;        // [3] RC_W1: Wake-up interrupt: Set by hardware when SOF detected and WKUIE=1
    volatile uint32_t SLAKI : 1;       // [4] RC_W1: Sleep acknowledge interrupt: Set by hardware when SLKIE=1
    uint32_t rsrv7_5 : 3;              // [7:5]
    volatile uint32_t TXM : 1;         // [8] R: Transmit mode: The CAN hardware is currently transmitter
    volatile uint32_t RXM : 1;         // [9] R: Receive mode: The CAN hardware is currently receiver
    volatile uint32_t SAMP : 1;        // [10] R: Last sample point: The value of RX on the last sample point
    volatile uint32_t RX : 1;          // [11] R: CAN Rx signal: monitor actual value of the CAN_RX pin
    volatile uint32_t rsrv31_12 : 20;  // [31:12]
} CAN_MSR_bits_type;

typedef union CAN_MSR_type {
    volatile uint32_t val;
    CAN_MSR_bits_type b;
} CAN_MSR_type;


// Transmit Status Register
typedef struct CAN_TSR_bits_type {
    volatile uint32_t RQCP0 : 1;       // [0] RC_W1: Request completed mailbox 0
    volatile uint32_t TXOK0 : 1;       // [1] RC_W1: Tx OK of amilbox 0
    volatile uint32_t ALST0 : 1;       // [2] RC_W1: Arbitration lost of mailbox 0
    volatile uint32_t TERR0 : 1;       // [3] RC_W1: Tx error of mailbox 0
    volatile uint32_t rsrv6_4 : 3;     // [6:4]
    volatile uint32_t ABRQ0 : 1;       // [7] RS: Abort request for mailbox 0. Set by SW, cleared by HW
    volatile uint32_t RQCP1 : 1;       // [8] RC_W1: Request completed mailbox 1
    volatile uint32_t TXOK1 : 1;       // [9] RC_W1: Tx OK of amilbox 1
    volatile uint32_t ALST1 : 1;       // [10] RC_W1: Arbitration lost of mailbox 1
    volatile uint32_t TERR1 : 1;       // [11] RC_W1: Tx error of mailbox 1
    volatile uint32_t rsrv14_12 : 3;   // [14:12]
    volatile uint32_t ABRQ1 : 1;       // [15] RS: Abort request for mailbox 1. Set by SW, cleared by HW
    volatile uint32_t RQCP2 : 1;       // [16] RC_W1: Request completed mailbox 2
    volatile uint32_t TXOK2 : 1;       // [17] RC_W1: Tx OK of amilbox 2
    volatile uint32_t ALST2 : 1;       // [18] RC_W1: Arbitration lost of mailbox 2
    volatile uint32_t TERR2 : 1;       // [19] RC_W1: Tx error of mailbox 2
    volatile uint32_t rsrv22_20 : 3;   // [22:20]
    volatile uint32_t ABRQ2 : 1;       // [23] RS: Abort request for mailbox 2. Set by SW, cleared by HW
    volatile uint32_t CODE : 2;        // [25:24] R: Next free tx mailbox code. When all pending code is equal to the lowest prio mailbox
    volatile uint32_t TME0 : 1;        // [26] R: tx mailbox 0 is empty
    volatile uint32_t TME1 : 1;        // [27] R: tx mailbox 1 is empty
    volatile uint32_t TME2 : 1;        // [28] R: tx mailbox 2 is empty
    volatile uint32_t LOW0 : 1;        // [29] R: Lowest prio flag for mailbox 0. Is set when more than one mailbox are pending for tx and mail0 has the lowset prio
    volatile uint32_t LOW1 : 1;        // [30] R: Lowest prio flag for mailbox 1. Is set when more than one mailbox are pending for tx and mail1 has the lowset prio
    volatile uint32_t LOW2 : 1;        // [31] R: Lowest prio flag for mailbox 2. Is set when more than one mailbox are pending for tx and mail2 has the lowset prio
} CAN_TSR_bits_type;

typedef union CAN_TSR_type {
    volatile uint32_t val;
    CAN_TSR_bits_type b;
} CAN_TSR_type;

// Bit Timing Register
typedef struct CAN_BTR_bits_type {
    volatile uint32_t BRP : 10;        // [9:0] RW: Baud rate presscaler: tq=(BRP[9:0]+1)*Tpclk
    uint32_t rsrv15_10 : 6;            // [15:10]
    volatile uint32_t TS1 : 4;         // [19:16] RW: Time segment 1: Tbs1=tq * (TS1[3:0] + 1)
    volatile uint32_t TS2 : 3;         // [22:20] RW: Time segment 2: Tbs2=tq * (TS2[2:0] + 1)
    uint32_t rsrv23 : 1;               // [23]
    volatile uint32_t SJW : 2;         // [25:24] RW: Resynchronization jump width: maximum number of time quanta to lengthen or shorten a bit
    uint32_t rsrv29_26 : 4;            // [29:26]
    volatile uint32_t LBKM : 1;        // [30] RW: Loop back mode (debug): 1=ena; 0=dis
    volatile uint32_t SILM : 1;        // [31] RW: Silent mode (debug): 1=ena; 0=dis
} CAN_BTR_bits_type;

typedef union CAN_BTR_type {
    volatile uint32_t val;
    CAN_BTR_bits_type b;
} CAN_BTR_type;

// Receive FIFO 0/1 Register
typedef struct CAN_RF_bits_type {
    volatile uint32_t FMP : 2;        // [1:0] R: How many messages are pending in the Rx FIFO. Decremented on release.
    uint32_t rsrv2 : 1;               // [2]
    volatile uint32_t FULL : 1;       // [3] RC_W1: FIFO is full. Cleared by SW
    volatile uint32_t FOVR : 1;       // [4] RC_W1: FIFO overrun. Cleared by SW
    volatile uint32_t RFOM : 1;       // [5] RS: Release FIFO output mailbox. Set by SW, cleared by Hw
    volatile uint32_t rsrv31_6 : 26;  // [31:6]
} CAN_RF_bits_type;

typedef union CAN_RF_type {
    volatile uint32_t val;
    CAN_RF_bits_type b;
} CAN_RF_type;

// Interrupt Enable Register
typedef struct CAN_IER_bits_type {
    volatile uint32_t TMEIE : 1;      // [0] RW: Tx mailbox is empty (when RQCPx is set)
    volatile uint32_t FMPIE0 : 1;     // [1] RW: FIFO0 msg pending irq enable (FMP != 0)
    volatile uint32_t FFIE0 : 1;      // [2] RW: FIFO0 full irq enable
    volatile uint32_t FOVIE0 : 1;     // [3] RW: FIFO0 overrun irq enable
    volatile uint32_t FMPIE1 : 1;     // [4] RW: FIFO1 msg pending irq enable (FMP != 0)
    volatile uint32_t FFIE1 : 1;      // [5] RW: FIFO1 full irq enable
    volatile uint32_t FOVIE1 : 1;     // [6] RW: FIFO1 overrun irq enable
    uint32_t rsrv7 : 1;               // [7]
    volatile uint32_t EWGIE : 1;      // [8] RW: Error warning irq enable
    volatile uint32_t EPVIE : 1;      // [9] RW: Error passive irq enable
    volatile uint32_t BOFIE : 1;      // [10] RW: Bus-off irq enable
    volatile uint32_t LECIE : 1;      // [11] RW: Last error code irq enable
    uint32_t rsrv14_12 : 3;           // [14:12]
    volatile uint32_t ERRIE : 1;      // [15] RW: Error interrupt enable
    volatile uint32_t WKUIE : 1;      // [16] RW: Wake-up irq enable
    volatile uint32_t SLKIE : 1;      // [17] RW: Sleep irq enable
    uint32_t rsrv31_18 : 14;          // [31:18]
} CAN_IER_bits_type;

typedef union CAN_IER_type {
    volatile uint32_t val;
    CAN_IER_bits_type b;
} CAN_IER_type;

// Error status Register
typedef struct CAN_ESR_bits_type {
    volatile uint32_t EWGF : 1;        // [0] R: Error warning flag
    volatile uint32_t EPVF : 1;        // [1] R: Error passive flag
    volatile uint32_t BOFF : 1;        // [2] R: Bus-of flag. Entered on TEC overflow (> 255)
    uint32_t rsrv3 : 1;                // [3]
    volatile uint32_t LEC : 3;         // [6:4] RW: Last error code: 0=no; 1=Stuff; 2=Form; 3=Ack; 4=Recessive bit; 5=Dominant bit; 6=CRC; 7=set by SW
    uint32_t rsrv15_7 : 9;             // [15:7]
    volatile uint32_t TEC : 8;         // [23:16] R: LSB byte of the 9-bit tx error counter
    volatile uint32_t REC : 8;         // [31:24] R: Receive error counter. Reception error counter.
} CAN_ESR_bits_type;

typedef union CAN_ESR_type {
    volatile uint32_t val;
    CAN_ESR_bits_type b;
} CAN_ESR_type;

/** 
  * @brief Controller Area Network TxMailBox 
  */
typedef struct CAN_txmailbox_type
{
    volatile uint32_t TIR;  /*!< CAN TX mailbox identifier register */
    volatile uint32_t TDTR; /*!< CAN mailbox data length control and time stamp register */
    volatile uint32_t TDLR; /*!< CAN mailbox data low register */
    volatile uint32_t TDHR; /*!< CAN mailbox data high register */
} CAN_txmailbox_type;

/** 
  * @brief Controller Area Network FIFOMailBox 
  */
  
typedef struct CAN_fifomailbox_type
{
    volatile uint32_t RIR;  /*!< CAN receive FIFO mailbox identifier register */
    volatile uint32_t RDTR; /*!< CAN receive FIFO mailbox data length control and time stamp register */
    volatile uint32_t RDLR; /*!< CAN receive FIFO mailbox data low register */
    volatile uint32_t RDHR; /*!< CAN receive FIFO mailbox data high register */
} CAN_fifomailbox_type;

/** 
  * @brief Controller Area Network FilterRegister 
  */
  
typedef struct CAN_filter_type
{
    volatile uint32_t FR1; /*!< CAN Filter bank register 1 */
    volatile uint32_t FR2; /*!< CAN Filter bank register 1 */
} CAN_filter_type;

/** 
  * @brief Controller Area Network 
  */
  
typedef struct CAN_registers_type
{
    CAN_MCR_type MCR;                 /*!< CAN master control register,         Address offset: 0x00          */
    CAN_MSR_type MSR;                 /*!< CAN master status register,          Address offset: 0x04          */
    CAN_TSR_type TSR;                 /*!< CAN transmit status register,        Address offset: 0x08          */
    CAN_RF_type RF[2];                /*!< CAN receive FIFO 0/1 register,         Address offset: 0x0C/0x10          */
    CAN_IER_type IER;                 /*!< CAN interrupt enable register,       Address offset: 0x14          */
    CAN_ESR_bits_type ESR;                 /*!< CAN error status register,           Address offset: 0x18          */
    CAN_BTR_type BTR;                 /*!< CAN bit timing register,             Address offset: 0x1C          */
    uint32_t RESERVED0[88];       /*!< Reserved, 0x020 - 0x17F                                            */
    CAN_txmailbox_type sTxMailBox[3];       /*!< CAN Tx MailBox,                      Address offset: 0x180 - 0x1AC */
    CAN_fifomailbox_type sFIFOMailBox[2];     /*!< CAN FIFO MailBox,                    Address offset: 0x1B0 - 0x1CC */
    uint32_t RESERVED1[12];       /*!< Reserved, 0x1D0 - 0x1FF                                            */
    volatile uint32_t FMR;                 /*!< CAN filter master register,          Address offset: 0x200         */
    volatile uint32_t FM1R;                /*!< CAN filter mode register,            Address offset: 0x204         */
    uint32_t RESERVED2;           /*!< Reserved, 0x208                                                    */
    volatile uint32_t FS1R;                /*!< CAN filter scale register,           Address offset: 0x20C         */
    uint32_t RESERVED3;           /*!< Reserved, 0x210                                                    */
    volatile uint32_t FFA1R;               /*!< CAN filter FIFO assignment register, Address offset: 0x214         */
    uint32_t RESERVED4;           /*!< Reserved, 0x218                                                    */
    volatile uint32_t FA1R;                /*!< CAN filter activation register,      Address offset: 0x21C         */
    uint32_t RESERVED5[8];        /*!< Reserved, 0x220-0x23F                                              */ 
    CAN_filter_type sFilterRegister[28]; /*!< CAN Filter Register,                 Address offset: 0x240-0x31C   */
} CAN_registers_type;
