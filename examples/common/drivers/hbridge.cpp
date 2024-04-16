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
    const char *name;
    gpio_pin_type drvmode;
    struct DcMotorConfigType {
        gpio_pin_type A;
        gpio_pin_type B;
    } dc[GARDEMARIN_DCMOTOR_PER_HBDRIGE];
    struct CurrentConfigType {
        gpio_pin_type pin;
        int adcidx;            // ADC channel index
    } current[GARDEMARIN_DCMOTOR_PER_HBDRIGE];
};

static const HBridgeConfigType CFG_HBRDIGE[GARDEMARIN_HBRDIGE_TOTAL] = {
    // HBRIDGE[0]
    {
        "hbrg0",
         {(GPIO_registers_type *)GPIOD_BASE, 8},       // 55 PD8 DRV0_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOE_BASE, 9},   // 40 PE9 TIM1_CH1
                 {(GPIO_registers_type *)GPIOE_BASE, 11}   // 42 PE11 TIM1_CH2
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOE_BASE, 13},  // 44 PE13 TIM1_CH3
                 {(GPIO_registers_type *)GPIOE_BASE, 14}   // 45 PE14 TIM1_CH4
             }
         },
         {
             // current[0]
             {
                 {(GPIO_registers_type *)GPIOA_BASE, 0},   // 23 PA0 ADC123_IN0
                 0
             },
             // current[1]
             {
                 {(GPIO_registers_type *)GPIOA_BASE, 3},   // 26 PA3 ADC123_IN3
                 3
             }
         }
    },
    // HBRDIGE[1]
    {
        "hbrg1",
         {(GPIO_registers_type *)GPIOD_BASE, 9},       // 56 PD9 DRV1_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOD_BASE, 12},  // 59 PD12 TIM4_CH1
                 {(GPIO_registers_type *)GPIOB_BASE, 7}    // 93 PB7 TIM4_CH2
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOB_BASE, 8},   // 95 PB8 TIM4_CH3
                 {(GPIO_registers_type *)GPIOB_BASE, 9}    // 96 PB9 TIM4_CH4
             }
         },
         {
             // current[0]
             {
                 {(GPIO_registers_type *)GPIOA_BASE, 4},   // 29 PA4 ADC12_IN4
                 4
             },
             // current[1]
             {
                 {(GPIO_registers_type *)GPIOA_BASE, 5},   // 30 PA5 ADC12_IN5
                 5
             }
         }
    },
    // HBRDIGE[2]
    {
        "hbrg2",
         {(GPIO_registers_type *)GPIOD_BASE, 10},      // 57 PD10 DRV2_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOC_BASE, 6},   // 63 PC6 TIM8_CH1
                 {(GPIO_registers_type *)GPIOC_BASE, 7}    // 64 PC7 TIM8_CH2
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOC_BASE, 8},   // 65 PC8 TIM8_CH3
                 {(GPIO_registers_type *)GPIOC_BASE, 9}    // 66 PC9 TIM8_CH4
             }
         },
         {
             // current[0]
             {
                 {(GPIO_registers_type *)GPIOA_BASE, 6},   // 31 PA6 ADC12_IN6
                 6
             },
             // current[1]
             {
                 {(GPIO_registers_type *)GPIOB_BASE, 0},   // 35 PB0 ADC12_IN8
                 8
             }
         }
    },
    // HBRDIGE[3]
    {
        "hbrg3",
         {(GPIO_registers_type *)GPIOD_BASE, 11},      // 58 PD11 DRV3_MODE
         {
             // dc[0]
             {
                 {(GPIO_registers_type *)GPIOE_BASE, 5},   // 4 PE5 TIM9_CH1
                 {(GPIO_registers_type *)GPIOE_BASE, 6}    // 5 PE6 TIM9_CH2
             },
             // dc[1]
             {
                 {(GPIO_registers_type *)GPIOB_BASE, 14},  // 53 PB14 TIM12_CH1
                 {(GPIO_registers_type *)GPIOB_BASE, 15}   // 54 PB15 TIM12_CH2
             }
         },
         {
             // current[0]
             {
                 {(GPIO_registers_type *)GPIOB_BASE, 1},   // 36 PB1 ADC12_IN9
                 9
             },
             // current[1]
             {
                 {(GPIO_registers_type *)GPIOC_BASE, 0},   // 15 PC0 ADC123_IN10
                 10
             }
         }
    }
};

HBridgeDriver::HBridgeDriver(int idx) : FwObject(CFG_HBRDIGE[idx].name),
    drvmode_("drvmode", "Drive mode pin: 0=4-pins control; 1=2-pins control"),
    dc0_(static_cast<FwObject *>(this), 0),
    dc1_(static_cast<FwObject *>(this), 1),
    i0_(static_cast<FwObject *>(this), 0),
    i1_(static_cast<FwObject *>(this), 1) {
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
        gpio_pin_as_analog(&CFG_HBRDIGE[idx].current[i].pin);

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

void HBridgeDriver::setBinEnabled() {
    drvmode_.make_uint8(1);
    gpio_pin_set(&CFG_HBRDIGE[idx_].drvmode);
}

void HBridgeDriver::setBinDisabled() {
    drvmode_.make_uint8(0);
    gpio_pin_clear(&CFG_HBRDIGE[idx_].drvmode);
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
void HBridgeDriver::startDcMotor(int idx) {
    // Set channel OUT[1:0] to 01 value
    gpio_pin_set(&CFG_HBRDIGE[idx_].dc[idx].A);
    gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].B);
}

void HBridgeDriver::stopDcMotor(int idx) {
    gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].A);
    gpio_pin_clear(&CFG_HBRDIGE[idx_].dc[idx].B);
}

