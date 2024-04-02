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
    volatile uint32_t MCR;                 /*!< CAN master control register,         Address offset: 0x00          */
    volatile uint32_t MSR;                 /*!< CAN master status register,          Address offset: 0x04          */
    volatile uint32_t TSR;                 /*!< CAN transmit status register,        Address offset: 0x08          */
    volatile uint32_t RF0R;                /*!< CAN receive FIFO 0 register,         Address offset: 0x0C          */
    volatile uint32_t RF1R;                /*!< CAN receive FIFO 1 register,         Address offset: 0x10          */
    volatile uint32_t IER;                 /*!< CAN interrupt enable register,       Address offset: 0x14          */
    volatile uint32_t ESR;                 /*!< CAN error status register,           Address offset: 0x18          */
    volatile uint32_t BTR;                 /*!< CAN bit timing register,             Address offset: 0x1C          */
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
