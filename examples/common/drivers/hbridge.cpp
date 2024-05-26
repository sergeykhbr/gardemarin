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
#include "hbridge.h"

struct HBridgeConfigType {
    gpio_pin_type drvmode;
    struct DcMotorConfigType {
        gpio_pin_type A;
        gpio_pin_type B;
        const char *adcport;
    } dc[GARDEMARIN_DCMOTOR_PER_HBDRIGE];
};

static const HBridgeConfigType CFG_HBRDIGE[GARDEMARIN_HBRDIGE_TOTAL] = {
    // HBRIDGE[0]
    {
         {(GPIO_registers_type *)GPIOD_BASE, 8},       // 55 PD8 DRV0_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOE_BASE, 9},   // 40 PE9 TIM1_CH1
                 {(GPIO_registers_type *)GPIOE_BASE, 11},  // 42 PE11 TIM1_CH2
                 "in0"
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOE_BASE, 13},  // 44 PE13 TIM1_CH3
                 {(GPIO_registers_type *)GPIOE_BASE, 14},  // 45 PE14 TIM1_CH4
                 "in3"
             }
         }
    },
    // HBRDIGE[1]
    {
         {(GPIO_registers_type *)GPIOD_BASE, 9},       // 56 PD9 DRV1_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOD_BASE, 12},  // 59 PD12 TIM4_CH1
                 {(GPIO_registers_type *)GPIOB_BASE, 7},   // 93 PB7 TIM4_CH2
                 "in4"
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOB_BASE, 8},   // 95 PB8 TIM4_CH3
                 {(GPIO_registers_type *)GPIOB_BASE, 9},   // 96 PB9 TIM4_CH4
                 "in5"
             }
         }
    },
    // HBRDIGE[2]
    {
         {(GPIO_registers_type *)GPIOD_BASE, 10},      // 57 PD10 DRV2_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOC_BASE, 6},   // 63 PC6 TIM8_CH1
                 {(GPIO_registers_type *)GPIOC_BASE, 7},   // 64 PC7 TIM8_CH2
                 "in6"
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOC_BASE, 8},   // 65 PC8 TIM8_CH3
                 {(GPIO_registers_type *)GPIOC_BASE, 9},   // 66 PC9 TIM8_CH4
                 "in8"
             }
         }
    },
    // HBRDIGE[3]
    {
         {(GPIO_registers_type *)GPIOD_BASE, 11},      // 58 PD11 DRV3_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOE_BASE, 5},   // 4 PE5 TIM9_CH1
                 {(GPIO_registers_type *)GPIOE_BASE, 6},   // 5 PE6 TIM9_CH2
                 "in9"
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOB_BASE, 14},  // 53 PB14 TIM12_CH1
                 {(GPIO_registers_type *)GPIOB_BASE, 15},  // 54 PB15 TIM12_CH2
                 "in10"
             }
         }
    }
};

HBridgeDriver::HBridgeDriver(const char *name, int idx) : FwObject(name),
    idx_(idx),
    drvmode_(this, "drvmode"),
    dc0_(static_cast<FwObject *>(this), 0),
    dc1_(static_cast<FwObject *>(this), 1),
    i0_(static_cast<FwObject *>(this), "i0", CFG_HBRDIGE[idx_].dc[0].adcport),
    i1_(static_cast<FwObject *>(this), "i1", CFG_HBRDIGE[idx_].dc[1].adcport) {
    // DC motor
    // mode = 0 (4-pins control)
    //    in[1]  in[0]     out[1] out[0]
    //     0       0         z     z 
    //     0       1         0     1    forward rotation
    //     1       0         1     0    backward rotation
    //     1       1         break break 
    gpio_pin_as_output(&CFG_HBRDIGE[idx].drvmode,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_SLOW,
                       GPIO_NO_PUSH_PULL);
    setBinDisabled();   // set 4-pins control

    for (int i = 0; i < GARDEMARIN_DCMOTOR_PER_HBDRIGE; i++) {
        gpio_pin_as_output(&CFG_HBRDIGE[idx].dc[i].A,
                           GPIO_NO_OPEN_DRAIN,
                           GPIO_SLOW,
                           GPIO_NO_PUSH_PULL);

        gpio_pin_as_output(&CFG_HBRDIGE[idx].dc[i].B,
                           GPIO_NO_OPEN_DRAIN,
                           GPIO_SLOW,
                           GPIO_NO_PUSH_PULL);

        // Set output value to LOW
        gpio_pin_clear(&CFG_HBRDIGE[idx].dc[i].A);
        gpio_pin_clear(&CFG_HBRDIGE[idx].dc[i].B);
    }
 
}

void HBridgeDriver::Init() {
    RegisterAttribute(&drvmode_);

    RegisterInterface(static_cast<BinInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));

    dc0_.Init();
    dc1_.Init();
    i0_.Init();
    i1_.Init();
}

void HBridgeDriver::PostInit() {
    i0_.PostInit();
    i1_.PostInit();
}

void HBridgeDriver::setBinEnabled() {
    gpio_pin_set(&CFG_HBRDIGE[idx_].drvmode);
}

void HBridgeDriver::setBinDisabled() {
    gpio_pin_clear(&CFG_HBRDIGE[idx_].drvmode);
}

uint8_t HBridgeDriver::getBinState() {
    return static_cast<uint8_t>(gpio_pin_get(&CFG_HBRDIGE[idx_].drvmode));
}

void HBridgeDriver::callbackTimer(uint64_t tickcnt) {
    // TODO: read ADC current value
}


// DC motor
// mode = 0 (4-pins control)
//    in[1]  in[0]     out[1] out[0]
//     0       0         z     z 
//     0       1         0     1    forward rotation
//     1       0         1     0    backward rotation
//     1       1         break break 
void HBridgeDriver::startDcMotor(int idx, int8_t direction) {
    // Set channel OUT[1:0] to 01 value
    if (direction == 0) {
        gpio_pin_set(&CFG_HBRDIGE[idx_].dc[idx].A);
        gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].B);
    } else {
        gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].A);
        gpio_pin_set(&CFG_HBRDIGE[idx_].dc[idx].B);
    }
}

void HBridgeDriver::stopDcMotor(int idx) {
    gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].A);
    gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].B);
}

void HBridgeDriver::DriveModeAttribute::pre_read() {
    u_.u8 = parent_->getBinState();
}

void HBridgeDriver::DriveModeAttribute::post_write() {
    if (u_.u8) {
        parent_->setBinEnabled();
    } else {
        parent_->setBinDisabled();
    }
}

