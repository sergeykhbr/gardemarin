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

#include <prjtypes.h>
#include <fwlist.h>
#include <fwobject.h>
#include <fwfifo.h>
#include <FwAttribute.h>
#include <TimerInterface.h>

class SoilDriver : public FwObject,
                   public TimerListenerInterface {
 public:
    explicit SoilDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() override { return 1000; }
    virtual void callbackTimer(uint64_t tickcnt) override;

 protected:
#pragma pack(1)
    struct QueryDataType {
        uint8_t address;        // 0x01
        uint8_t function;       // 0x03 = query data; 0x06 = modification address
        uint8_t start_addr_h;   // 0x00
        uint8_t start_addr_l;   // 0x00
        uint8_t length_h;       // 0x00
        uint8_t length_l;       // 0x08
        uint8_t crc_l;          // 0x44
        uint8_t crc_h;          // 0x0c

        QueryDataType() : address(0x01), function(0x03), start_addr_h(0),
            start_addr_l(0), length_h(0), length_l(8),
            crc_l(0x44), crc_h(0x0c) {}
    };

    struct DataResponseType {
        uint8_t address;        // 0x01
        uint8_t function;       // 0x03 = query data; 0x06 = modification address
        uint8_t length;         // 0x10
        uint8_t temp[2];        // temperature: 0=high; 1=low. 0xffdd = -3.5 C
        uint8_t moisture[2];    // soil moisture: 0=high; 1=low. 0x0164 = 35.6 %
        uint8_t salinty[2];     // soil salinty: 0x04d2 = 1234 uS/cm
        uint8_t EC[2];          // soil EC: 0x03f0 = 1008 uS/cm
        uint8_t pH[2];          // soil pH: 0x02ae = 6.86
        uint8_t N[2];           // soil nitrogen cotent: 0x0087 = 125 mg/kg
        uint8_t P[2];           // soil phosphrus cotent: 0x008A = 138 mg/kg
        uint8_t K[2];           // soil potassium cotent: 0x008E = 142 mg/kg
        uint8_t crc16_l;        // 0x0a
        uint8_t crc16_h;        // 0x69
    };
#pragma pack()

 protected:
    FwAttribute T_;
    FwAttribute moisture_;
    FwAttribute salnity_;
    FwAttribute EC_;
    FwAttribute pH_;
    FwAttribute N_;
    FwAttribute P_;
    FwAttribute K_;

    FwFifo rxfifo_;
    FwFifo txfifo_;
    QueryDataType queryData_;
    DataResponseType response_;
};
