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
#include "system_handlers.h"
#include <app_handlers.h>

extern char __stack_base[];

/**
 @brief The vector table.
 @details This relies on the linker script to place at correct location in memory.
*/
__attribute__ ((section(".isr_vector"),used))
irq_handler_type __isr_vectors[98] =
{ //
    (irq_handler_type)(__stack_base),                // The initial stack pointer
    Reset_Handler,                                   /* Reset Handler */
    NMI_Handler,                                     /* NMI Handler*/
    HardFault_Handler,                               /* Hard Fault Handler*/
    MemManage_Handler,                               /* MPU Fault Handler*/
    BusFault_Handler,                                /* Bus Fault Handler*/
    UsageFault_Handler,                              /* Usage Fault Handler*/
    0,                                               /* Reserved*/
    0,                                               /* Reserved*/
    0,                                               /* Reserved*/
    0,                                               /* Reserved*/
    SVC_Handler,                                     /* SVCall Handler*/
    DebugMon_Handler,                                /* Debug Monitor Handler*/
    0,                                               /* Reserved*/
    PendSV_Handler,                                  /* PendSV Handler*/
    SysTick_Handler,                                 /* SysTick Handler*/

    /* Vendor specific interrupts: */
    WWDG_IRQHandler,                   /* 0 Window WatchDog */
    PVD_IRQHandler,                    /* 1 PVD through EXTI Line detection */
    TAMP_STAMP_IRQHandler,             /* 2 Tamper and TimeStamps through the EXTI line */
    RTC_WKUP_IRQHandler,               /* 3 RTC Wakeup through the EXTI line */
    FLASH_IRQHandler,                  /* 4 FLASH */
    RCC_IRQHandler,                    /* 5 RCC */
    EXTI0_IRQHandler,                  /* 6 EXTI Line0 */
    EXTI1_IRQHandler,                  /* 7 EXTI Line1 */
    EXTI2_IRQHandler,                  /* 8 EXTI Line2 */
    EXTI3_IRQHandler,                  /* 9 EXTI Line3 */
    EXTI4_IRQHandler,                  /* 10 EXTI Line4 */
    DMA1_Stream0_IRQHandler,           /* 11 DMA1 Stream 0 */
    DMA1_Stream1_IRQHandler,           /* 12 DMA1 Stream 1 */
    DMA1_Stream2_IRQHandler,           /* 13 DMA1 Stream 2 */
    DMA1_Stream3_IRQHandler,           /* 14 DMA1 Stream 3 */
    DMA1_Stream4_IRQHandler,           /* 15 DMA1 Stream 4 */
    DMA1_Stream5_IRQHandler,           /* 16 DMA1 Stream 5 */
    DMA1_Stream6_IRQHandler,           /* 17 DMA1 Stream 6 */
    ADC_IRQHandler,                    /* 18 ADC1, ADC2 and ADC3s */
    CAN1_TX_IRQHandler,                /* 19 CAN1 TX */
    CAN1_RX0_IRQHandler,               /* 20 CAN1 RX0 */
    CAN1_RX1_IRQHandler,               /* 21 CAN1 RX1 */
    CAN1_SCE_IRQHandler,               /* 22 CAN1 SCE */
    EXTI9_5_IRQHandler,                /* 23 External Line[9:5]s */
    TIM1_BRK_TIM9_IRQHandler,          /* 24 TIM1 Break and TIM9 */
    TIM1_UP_TIM10_IRQHandler,          /* 25 TIM1 Update and TIM10 */
    TIM1_TRG_COM_TIM11_IRQHandler,     /* 26 TIM1 Trigger and Commutation and TIM11 */
    TIM1_CC_IRQHandler,                /* 27 TIM1 Capture Compare */
    TIM2_IRQHandler,                   /* 28 TIM2 */
    TIM3_IRQHandler,                   /* 29 TIM3 */
    TIM4_IRQHandler,                   /* 30 TIM4 */
    I2C1_EV_IRQHandler,                /* 31 I2C1 Event */
    I2C1_ER_IRQHandler,                /* 32 I2C1 Error */
    I2C2_EV_IRQHandler,                /* 33 I2C2 Event */
    I2C2_ER_IRQHandler,                /* 34 I2C2 Error */
    SPI1_IRQHandler,                   /* 35 SPI1 */
    SPI2_IRQHandler,                   /* 36 SPI2 */
    USART1_IRQHandler,                 /* 37 USART1 */
    USART2_IRQHandler,                 /* 38 USART2 */
    USART3_IRQHandler,                 /* 39 USART3 */
    EXTI15_10_IRQHandler,              /* 40 External Lines[15:10] */
    RTC_Alarm_IRQHandler,              /* 41 RTC Alarm (A and B) through EXTI Line */
    OTG_FS_WKUP_IRQHandler,            /* 42 USB OTG FS Wakeup through EXTI line */
    TIM8_BRK_TIM12_IRQHandler,         /* 43 TIM8 Break and TIM12 */
    TIM8_UP_TIM13_IRQHandler,          /* 44 TIM8 Update and TIM13 */
    TIM8_TRG_COM_TIM14_IRQHandler,     /* 45 TIM8 Trigger and Commutation and TIM14 */
    TIM8_CC_IRQHandler,                /* 46 TIM8 Capture Compare */
    DMA1_Stream7_IRQHandler,           /* 47 DMA1 Stream7 */
    FSMC_IRQHandler,                   /* 48 FSMC */
    SDIO_IRQHandler,                   /* 49 SDIO */
    TIM5_IRQHandler,                   /* 50 TIM5 */
    SPI3_IRQHandler,                   /* 51 SPI3 */
    UART4_IRQHandler,                  /* 52 UART4 */
    UART5_IRQHandler,                  /* 53 UART5 */
    TIM6_DAC_IRQHandler,               /* 54 TIM6 and DAC1&2 underrun errors */
    TIM7_IRQHandler,                   /* 55 TIM7 */
    DMA2_Stream0_IRQHandler,           /* 56 DMA2 Stream 0 */
    DMA2_Stream1_IRQHandler,           /* 57 DMA2 Stream 1 */
    DMA2_Stream2_IRQHandler,           /* 58 DMA2 Stream 2 */
    DMA2_Stream3_IRQHandler,           /* 59 DMA2 Stream 3 */
    DMA2_Stream4_IRQHandler,           /* 60 DMA2 Stream 4 */
    ETH_IRQHandler,                    /* 61 Ethernet */
    ETH_WKUP_IRQHandler,               /* 62 Ethernet Wakeup through EXTI line */
    CAN2_TX_IRQHandler,                /* 63 CAN2 TX */
    CAN2_RX0_IRQHandler,               /* 64 CAN2 RX0 */
    CAN2_RX1_IRQHandler,               /* 65 CAN2 RX1 */
    CAN2_SCE_IRQHandler,               /* 66 CAN2 SCE */
    OTG_FS_IRQHandler,                 /* 67 USB OTG FS */
    DMA2_Stream5_IRQHandler,           /* 68 DMA2 Stream 5 */
    DMA2_Stream6_IRQHandler,           /* 69 DMA2 Stream 6 */
    DMA2_Stream7_IRQHandler,           /* 70 DMA2 Stream 7 */
    USART6_IRQHandler,                 /* 71 USART6 */
    I2C3_EV_IRQHandler,                /* 72 I2C3 event */
    I2C3_ER_IRQHandler,                /* 73 I2C3 error */
    OTG_HS_EP1_OUT_IRQHandler,         /* 74 USB OTG HS End Point 1 Out */
    OTG_HS_EP1_IN_IRQHandler,          /* 75 USB OTG HS End Point 1 In */
    OTG_HS_WKUP_IRQHandler,            /* 76 USB OTG HS Wakeup through EXTI */
    OTG_HS_IRQHandler,                 /* 77 USB OTG HS */
    DCMI_IRQHandler,                   /* 78 DCMI */
    0,                                 /* 79 Reserved */
    HASH_RNG_IRQHandler,               /* 80 Hash and RNG */
    FPU_IRQHandler                     /* 81 FPU */
};

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.

void __attribute__ ((section(".after_vectors")))
DefaultISR(void) {
    while (1) {
    }
}

