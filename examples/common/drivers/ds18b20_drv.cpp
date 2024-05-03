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

#include <prjtypes.h>
#include <stm32f4xx_map.h>
#include <fwapi.h>
#include "ds18b20_drv.h"
#include <uart.h>

// 67 TEMP1  PA[8]
// 70 TEMP2  PA[11]
static const gpio_pin_type GPIO_CFG[GARDEMARIN_DS18B20_TOTAL] = {
    {(GPIO_registers_type *)GPIOA_BASE, 8},
    {(GPIO_registers_type *)GPIOA_BASE, 11}
};

IrqHandlerInterface *drivers_ = 0;

extern "C" void startCounter(int usec) {
    TIM_registers_type *TIM3 = (TIM_registers_type *)TIM3_BASE;
    tim_cr1_reg_type cr1;

    write32(&TIM3->ARR, static_cast<uint32_t>(usec));
    write32(&TIM3->CNT, static_cast<uint32_t>(usec));

    cr1.val = 0;
    cr1.bits.CEN = 1;
    cr1.bits.OPM = 1;   // one pulse mode
    cr1.bits.DIR = 1;   // downcount
    write32(&TIM3->CR1.val, cr1.val);
}

// Configure TIM3 as a 15 us interval former
extern "C" void TIM3_irq_handler() {
    TIM_registers_type *TIM3 = (TIM_registers_type *)TIM3_BASE;

    int usec = 0;
    drivers_->handleInterrupt(&usec);

    write16(&TIM3->SR, 0);  // clear all pending bits
    nvic_irq_clear(29);
    if (usec) {
        startCounter(usec);
    }
}

// TIM3 is used as sample interval counter
Ds18b20Driver::Ds18b20Driver(const char *name)
    : FwObject(name),
    SerialMsb0_("SerialMsb0"),
    SerialLsb0_("SerialLsb0"),
    T0_("T0"),
    SerialMsb1_("SerialMsb1"),
    SerialLsb1_("SerialLsb1"),
    T1_("T1"),
    estate_(Idle),
    etxrx_(TxRx_None),
    wdog_(0),
    bitcnt_(0),
    shft8_(0),
    chn_(0),
    requestConversion_(false) {
    SerialLsb0_.make_uint32(0);
    SerialMsb0_.make_uint16(0);
    T0_.make_uint32(0);
    SerialLsb1_.make_uint32(0);
    SerialMsb1_.make_uint16(0);
    T1_.make_uint32(0);

    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM3 = (TIM_registers_type *)TIM3_BASE;
    drivers_ = static_cast<IrqHandlerInterface *>(this);

    for (int i = 0; i < GARDEMARIN_DS18B20_TOTAL; i++) {
        gpio_pin_as_output(&GPIO_CFG[i],
                           GPIO_NO_OPEN_DRAIN,
                           GPIO_FAST,
                           GPIO_NO_PUSH_PULL);
        gpio_pin_set(&GPIO_CFG[i]); // set high
    }

    // adc clock on APB1 = 144/4 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 1);             // APB1[0] TIM3EN
    write32(&RCC->APB1ENR, t1);

    write32(&TIM3->CR1.val, 0);         // stop counter
    write16(&TIM3->PSC, system_clock_hz() / 2 / 1000000 - 1);             // prescaler = 1: CK_CNT = (F_ck_psc/(PSC+1))
    write16(&TIM3->DIER, 1);            // [0] UIE - update interrupt enabled

    // prio: 0 highest; 7 is lowest
    nvic_irq_enable(29, 5);
}

void Ds18b20Driver::Init() {
    RegisterAttribute(&T0_);
    RegisterAttribute(&T1_);
    RegisterAttribute(&SerialLsb0_);
    RegisterAttribute(&SerialMsb0_);
    RegisterAttribute(&SerialLsb1_);
    RegisterAttribute(&SerialMsb1_);

    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));
}

void Ds18b20Driver::callbackTimer(uint64_t tickcnt) {
    if (estate_ == Idle) {
        estate_ = Master_Reset_Pulse;
        chn_ = (chn_ + 1) & 1;
        setInput(&GPIO_CFG[chn_]);
        requestConversion_ = true;
        startCounter(50);
    }
}

void Ds18b20Driver::handleInterrupt(int *argv) {
    // Receive and transmit bit data
    switch (etxrx_) {
    case TxRx_BitInit:
        setOutput(&GPIO_CFG[chn_], 0);
        *argv = 2;
        etxrx_ = TxRx_Setup;
        wdog_ = 0;
        return;
    case TxRx_Setup:
        if (we_) {
            setOutput(&GPIO_CFG[chn_], shft8_ & 1);
            shft8_ >>= 1;
            *argv = 45;
            etxrx_ = TxRx_End;
        } else {
            setInput(&GPIO_CFG[chn_]);
            *argv = 15;
            etxrx_ = TxRx_Hold;
        }
        return;
    case TxRx_Hold:
        rxbuf_.u[bytecnt_] >>= 1;
        rxbuf_.u[bytecnt_] |= static_cast<uint8_t>(gpio_pin_get(&GPIO_CFG[chn_])) << 7;
        if (bitcnt_ == 0) {
            bytecnt_++;
        }
        *argv = 30;
        etxrx_ = TxRx_End;
        return;
    case TxRx_End:
        if (we_ == 0 && gpio_pin_get(&GPIO_CFG[chn_]) == 0 && (++wdog_ < 8)) {
            // Wait a bit longer until line will be pulled to HIGH
            *argv = 15;
        } else {
            setInput(&GPIO_CFG[chn_]); // line will be pull-up to HIGH after transmission
            *argv = 2;
            if (bitcnt_) {
                bitcnt_--;
                etxrx_ = TxRx_BitInit;
            } else {
                etxrx_ = TxRx_None;
            }
        }
        return;
    default:;
    }

    // Command interface:
    switch (estate_) {
    case Master_Reset_Pulse:
        setOutput(&GPIO_CFG[chn_], 0);
        *argv = 500;        // 480 - 960
        estate_ = Wait_Presence_Pulse;
        break;
    case Wait_Presence_Pulse:
        setInput(&GPIO_CFG[chn_]);
        *argv = 2;
        estate_ = Tx_Present_Pulse;
        wdog_ = 0;
        break;
    case Tx_Present_Pulse:
        // Expect DS18B20 TX PRESENCE pulse LOW duration 60-240 ms in a range 480 us (min).
        if (gpio_pin_get(&GPIO_CFG[chn_]) == 0) {
            if (requestConversion_) {
                // We should be sure that FamilyId is 0x28 before using wait cycle
                estate_ = Rom_Command_33h;  // Read ROM 8 bytes
            } else {
                estate_ = Rom_Command_CCh;  // skip ROM command
            }
            *argv = 480;
        } else {
            if (++wdog_ < 12) {
                *argv = 50;
            } else {
                // Error state
                estate_ = Idle;
                requestConversion_ = false;
            }
        }
        break;
    case Rom_Command_33h:
        // Read Rom: 8 bytes
        we_ = 1;
        shft8_ = 0x33;
        bitcnt_ = 7;
        bytecnt_ = 0;
        etxrx_ = TxRx_BitInit;
        estate_ = Rom_response;
        *argv = 2;
        break;
    case Rom_Command_CCh:
        // Read Rom: 8 bytes
        we_ = 1;
        shft8_ = 0xCC;
        bitcnt_ = 7;
        bytecnt_ = 0;
        etxrx_ = TxRx_BitInit;
        estate_ = Func_Command_BEh;         // read scratchpad
        *argv = 2;
        break;
    case Rom_response:
        *argv = 2;
        if (bytecnt_ < sizeof(Rom33h_type)) {
            we_ = 0;
            bitcnt_ = 7;
            etxrx_ = TxRx_BitInit;
        } else {
            if (rxbuf_.rom33h.FamilyCode != 0x28) {
                // Wrong sensor or pull-up resitor is not connected (always 0's)
                *argv = 0;
                estate_ = Idle;
            } else {
                estate_ = Func_Command_44h;     // start temp conversion
                if (chn_ == 0) {
                    SerialLsb0_.write(reinterpret_cast<char *>(
                        &rxbuf_.rom33h.SerialNumber[0]), 4);
                    SerialMsb0_.write( reinterpret_cast<char *>(
                        &rxbuf_.rom33h.SerialNumber[4]), 2);
                } else {
                    SerialLsb1_.write(reinterpret_cast<char *>(
                        &rxbuf_.rom33h.SerialNumber[0]), 4);
                    SerialMsb1_.write( reinterpret_cast<char *>(
                        &rxbuf_.rom33h.SerialNumber[4]), 2);
                }
            }
        }
        break;
    case Func_Command_44h:
        we_ = 1;
        shft8_ = 0x44;
        bitcnt_ = 7;
        bytecnt_ = 0;
        etxrx_ = TxRx_BitInit;
        estate_ = Tx_Wait_Ready;
        *argv = 2;
        break;
    case Func_Command_BEh:
        we_ = 1;
        shft8_ = 0xBE;
        bitcnt_ = 7;
        bytecnt_ = 0;
        etxrx_ = TxRx_BitInit;
        estate_ = Tx_Read_Scratchpad;
        *argv = 2;
        break;
    case Tx_Read_Scratchpad:
        if (bytecnt_ < sizeof(Ds18B20_memory_map)) {
            we_ = 0;
            bitcnt_ = 7;
            etxrx_ = TxRx_BitInit;
            *argv = 2;
        } else {
            uint32_t T = rxbuf_.scratchpad.TemperatureMsb;
            uint32_t frac = rxbuf_.scratchpad.TemperatureLsb & 0xF;
            T = (T << 4) | (rxbuf_.scratchpad.TemperatureLsb >> 4);
            T = 100 * T + (100 * frac) / 15;
            if (chn_ == 0) {
                T0_.make_uint32(T);
            } else {
                T1_.make_uint32(T);
            }
            estate_ = Idle;
        }
        break;
    case Tx_Wait_Ready:
        if (gpio_pin_get(&GPIO_CFG[chn_]) == 0) {
            *argv = 30;
        } else if (requestConversion_) {
            *argv = 30;
            requestConversion_ = false;
            estate_ = Master_Reset_Pulse;
        } else {
            estate_ = Idle;
        }
        break;
    }
}

void Ds18b20Driver::setOutput(const gpio_pin_type *gpio, uint32_t val) {
    uint32_t t1;
    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&gpio->port->MODER);
    t1 &= ~(0x3 << 2*gpio->pinidx);
    t1 |= (0x1 << 2*gpio->pinidx);
    write32(&gpio->port->MODER, t1);

    if (val) {
        gpio_pin_set(gpio);
    } else {
        gpio_pin_clear(gpio);
    }
}

void Ds18b20Driver::setInput(const gpio_pin_type *gpio) {
    uint32_t t1;
    // 00 input; 01 output; 10 alternate; 11 analog
    t1 = read32(&gpio->port->MODER);
    t1 &= ~(0x3 << 2*gpio->pinidx);
    write32(&gpio->port->MODER, t1);
}
