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
    volatile uint32_t TSR;                 /*!< CAN transmit status register,        Address offset: 0x08          */
    volatile uint32_t RF0R;                /*!< CAN receive FIFO 0 register,         Address offset: 0x0C          */
    volatile uint32_t RF1R;                /*!< CAN receive FIFO 1 register,         Address offset: 0x10          */
    volatile uint32_t IER;                 /*!< CAN interrupt enable register,       Address offset: 0x14          */
    volatile uint32_t ESR;                 /*!< CAN error status register,           Address offset: 0x18          */
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
