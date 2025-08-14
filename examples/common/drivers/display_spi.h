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
#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <TimerInterface.h>
#include <SensorInterface.h>
#include <IrqInterface.h>
#include <gpio_drv.h>


class DisplaySPI : public FwObject,
                   public IrqHandlerInterface,
                   public TimerListenerInterface {
 public:
    DisplaySPI(const char *name);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // IrqHandlerInterface: SPI tx is empty
    virtual void handleInterrupt(int *argv) override;

    // TimerListenerInterface
    virtual uint64_t getTimerInterval() { return 100; }
    virtual void callbackTimer(uint64_t tickcnt) override {}

 protected:
    virtual void starCounter(void *dev, int usec);  // resolution 100 ns
    virtual void write_cmd_poll(uint16_t cmd);
    virtual void write_data_poll(uint16_t data);

 protected:
    static const uint16_t ROWS = 240;
    static const uint16_t COLS = 240;

    virtual uint8_t draw_point(uint16_t x, uint16_t y, uint32_t color);
    virtual uint8_t show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint32_t color);
    virtual uint8_t show_string(uint16_t x, uint16_t y, char *str, uint16_t len, uint32_t color);

    enum EState {
        Idle,
        Conversion,
        WaitReady,
        SCK_HIGH,
        SCK_LOW,
        Reading,
        Sleep
    } estate_;
    int8_t bitCnt_;
    uint8_t buffer_[4096];
};

