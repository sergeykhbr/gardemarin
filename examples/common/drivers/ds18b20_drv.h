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

#include <gardemarin.h>
#include <prjtypes.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <IrqInterface.h>
#include <TimerInterface.h>
#include <gpio_drv.h>

// Temperature sensor with 1-Write interface
class Ds18b20Driver : public FwObject,
                      public IrqHandlerInterface,
                      public TimerListenerInterface {
 public:
    Ds18b20Driver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // IrqHandlerInterface
    virtual void handleInterrupt(int *argv) override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() override { return 1000; }
    virtual void callbackTimer(uint64_t tickcnt) override;

 protected:
    virtual void setOutput(const gpio_pin_type *gpio, uint32_t val);
    virtual void setInput(const gpio_pin_type *gpio);

 protected:

#pragma pack(1)
    typedef struct Ds18B20_memory_map {
        uint8_t TemperatureLsb;     // (50H = 85 C on reset)
        uint8_t TemperatureMsb;     // (05H = 85 C on reset)
        uint8_t TH_or_UserByte1;
        uint8_t TL_or_UserByte2;
        uint8_t Config;
        uint8_t rsrv5_0xff;
        uint8_t rsrv6;
        uint8_t rsrv7_0x10;
        uint8_t CRC;
    } Ds18B20_memory_map;

    typedef struct Rom33h_type {
        uint8_t FamilyCode;
        uint8_t SerialNumber[6];
        uint8_t CRC;
    } Rom33h_type;

    typedef union rxbuf_type {
        uint8_t u[10];
        Ds18B20_memory_map scratchpad;
        Rom33h_type rom33h;
    } rxbuf_type;
#pragma pack()

    typedef enum EState {
        Idle,
        // initialization sequence
        Master_Reset_Pulse,     // CPU-to-DS18B20 (m2s). Low 480 us minimum
        Wait_Presence_Pulse,
        Tx_Present_Pulse,       // DS18B20-to-CPU (s2m). should be Low 60-240 usec inside of 480 us interval minimum
        // end of init sequence
        Rom_Command_33h,        // Read Rom: only when one slave on the bus (m2s)
        Rom_Command_CCh,        // Skip Rom Command: (m2s)
        Rom_response,           // 8-bytes: (s2m)
        Func_Command_44h,       // initiate temperature conversion (m2s)
        Func_Command_BEh,       // read scratchpad
        Tx_Wait_Ready,          // wait until master Rx "1" temperature conversion finished, then go to init state
        Tx_Read_Scratchpad      // Read up-to 9-bytes: LSB first.
    } EState;

    typedef enum ETxRx {
        TxRx_None,
        TxRx_BitInit,   // 2 us (> 1 us). LOW
        TxRx_Setup,     // 15 us
        TxRx_Hold,      // 30 us
        TxRx_End,       // 2 us (> 1 us). HIGH
    } ETxRx;

 protected:
    FwAttribute SerialMsb0_;     // [47:32]
    FwAttribute SerialLsb0_;     // [31:0]
    FwAttribute T0_;

    FwAttribute SerialMsb1_;     // [47:32]
    FwAttribute SerialLsb1_;     // [31:0]
    FwAttribute T1_;

    EState estate_;
    ETxRx etxrx_;
    int8_t wdog_;
    int8_t bitcnt_;
    int8_t bytecnt_;
    uint8_t shft8_;
    uint8_t we_;
    int8_t chn_;
    bool requestConversion_;
    rxbuf_type rxbuf_;
};
