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
#include <SensorInterface.h>
#include <IrqInterface.h>
#include <DisplayInterface.h>
#include <gpio_drv.h>
#include <spi.h>

class DisplaySPI : public FwObject,
                   public IrqHandlerInterface,
                   public DisplayInterface {
 public:
    DisplaySPI(const char *name);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // IrqHandlerInterface: SPI tx is empty
    virtual void handleInterrupt(int *argv) override;

    // IDisplayInterface
    virtual void clearScreen();
    virtual void clearLines(int start, int total, uint16_t clr);
    virtual void outputText24Line(char *str, int linepos, int symbpos, uint32_t clr, uint32_t bkgclr);
    virtual void outputText16Line(char *str, int linepos, int symbpos, uint32_t clr, uint32_t bkgclr);

 protected:
    virtual void init_GPIO() = 0;
    virtual void init_SPI() = 0;
    virtual SPI_registers_type *spi_bar() = 0;

 protected:
    virtual void outputTextLine(const uint8_t *arr, uint8_t fontsz,
                                char *str, int linepos,
                                int symbpos,
                                uint32_t clr, uint32_t bkgclr);
    virtual void starCounter(void *dev, int usec);  // resolution 100 ns
    virtual void write_cmd_poll(uint16_t cmd);
    virtual void write_data_poll(uint16_t data);

 protected:

    virtual uint8_t draw_point(uint16_t x, uint16_t y, uint32_t clr);
    virtual uint8_t show_char(const uint8_t *arr, uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint32_t clr, uint32_t bkgclr);
    //virtual uint8_t show_string(uint16_t x, uint16_t y, char *str, uint16_t len, uint32_t clr, uint32_t bkgclr);

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
    uint8_t buffer_[4*1024];

 protected:
    gpio_pin_type DISPLAY_RES;
    gpio_pin_type DISPLAY_SCK;
    gpio_pin_type DISPLAY_SDA;
    gpio_pin_type DISPLAY_DC;
};
