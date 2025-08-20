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

#if defined(__cplusplus)
extern "C"
{
#endif

extern void EXTI3_CanSofListener_IRQHandler();
extern void Btn_IRQHandler();
extern void CAN1_FIFO0_irq_handler();
extern void CAN1_FIFO1_irq_handler();
extern void CAN1_SCE_irq_handler();
extern void CAN2_FIFO0_irq_handler();
extern void CAN2_FIFO1_irq_handler();
extern void CAN2_SCE_irq_handler();
extern void USART1_irq_handler();
extern void CanMonitor_TIM5_irq_handler();
extern void CanMonitor_SPI3_irq_handler();

#define WWDG_IRQHandler DefaultISR
#define PVD_IRQHandler DefaultISR
#define TAMP_STAMP_IRQHandler DefaultISR
#define RTC_WKUP_IRQHandler DefaultISR
#define FLASH_IRQHandler DefaultISR
#define RCC_IRQHandler DefaultISR
#define EXTI0_IRQHandler DefaultISR
#define EXTI1_IRQHandler DefaultISR
#define EXTI2_IRQHandler DefaultISR
#define EXTI3_IRQHandler EXTI3_CanSofListener_IRQHandler
#define EXTI4_IRQHandler DefaultISR
#define DMA1_Stream0_IRQHandler DefaultISR
#define DMA1_Stream1_IRQHandler DefaultISR
#define DMA1_Stream2_IRQHandler DefaultISR
#define DMA1_Stream3_IRQHandler DefaultISR
#define DMA1_Stream4_IRQHandler DefaultISR
#define DMA1_Stream5_IRQHandler DefaultISR
#define DMA1_Stream6_IRQHandler DefaultISR
#define ADC_IRQHandler DefaultISR
#define CAN1_TX_IRQHandler DefaultISR
#define CAN1_RX0_IRQHandler CAN1_FIFO0_irq_handler
#define CAN1_RX1_IRQHandler CAN1_FIFO1_irq_handler
#define CAN1_SCE_IRQHandler CAN1_SCE_irq_handler
#define EXTI9_5_IRQHandler DefaultISR
#define TIM1_BRK_TIM9_IRQHandler DefaultISR
#define TIM1_UP_TIM10_IRQHandler DefaultISR
#define TIM1_TRG_COM_TIM11_IRQHandler DefaultISR
#define TIM1_CC_IRQHandler DefaultISR
#define TIM2_IRQHandler DefaultISR
#define TIM3_IRQHandler DefaultISR
#define TIM4_IRQHandler DefaultISR
#define I2C1_EV_IRQHandler DefaultISR
#define I2C1_ER_IRQHandler DefaultISR
#define I2C2_EV_IRQHandler DefaultISR
#define I2C2_ER_IRQHandler DefaultISR
#define SPI1_IRQHandler DefaultISR
#define SPI2_IRQHandler DefaultISR
#define USART1_IRQHandler USART1_irq_handler
#define USART2_IRQHandler DefaultISR
#define USART3_IRQHandler DefaultISR
#define EXTI15_10_IRQHandler Btn_IRQHandler
#define RTC_Alarm_IRQHandler DefaultISR
#define OTG_FS_WKUP_IRQHandler DefaultISR
#define TIM8_BRK_TIM12_IRQHandler DefaultISR
#define TIM8_UP_TIM13_IRQHandler DefaultISR
#define TIM8_TRG_COM_TIM14_IRQHandler DefaultISR
#define TIM8_CC_IRQHandler DefaultISR
#define DMA1_Stream7_IRQHandler DefaultISR
#define FSMC_IRQHandler DefaultISR
#define SDIO_IRQHandler DefaultISR
#define TIM5_IRQHandler CanMonitor_TIM5_irq_handler
#define SPI3_IRQHandler CanMonitor_SPI3_irq_handler
#define UART4_IRQHandler DefaultISR
#define UART5_IRQHandler DefaultISR
#define TIM6_DAC_IRQHandler DefaultISR
#define TIM7_IRQHandler DefaultISR
#define DMA2_Stream0_IRQHandler DefaultISR
#define DMA2_Stream1_IRQHandler DefaultISR
#define DMA2_Stream2_IRQHandler DefaultISR
#define DMA2_Stream3_IRQHandler DefaultISR
#define DMA2_Stream4_IRQHandler DefaultISR
#define ETH_IRQHandler DefaultISR
#define ETH_WKUP_IRQHandler DefaultISR
#define CAN2_TX_IRQHandler DefaultISR
#define CAN2_RX0_IRQHandler CAN2_FIFO0_irq_handler
#define CAN2_RX1_IRQHandler CAN2_FIFO1_irq_handler
#define CAN2_SCE_IRQHandler CAN2_SCE_irq_handler
#define OTG_FS_IRQHandler DefaultISR
#define DMA2_Stream5_IRQHandler DefaultISR
#define DMA2_Stream6_IRQHandler DefaultISR
#define DMA2_Stream7_IRQHandler DefaultISR
#define USART6_IRQHandler DefaultISR
#define I2C3_EV_IRQHandler DefaultISR
#define I2C3_ER_IRQHandler DefaultISR
#define OTG_HS_EP1_OUT_IRQHandler DefaultISR
#define OTG_HS_EP1_IN_IRQHandler DefaultISR
#define OTG_HS_WKUP_IRQHandler DefaultISR
#define OTG_HS_IRQHandler DefaultISR
#define DCMI_IRQHandler DefaultISR
#define HASH_RNG_IRQHandler DefaultISR
#define FPU_IRQHandler DefaultISR

#if defined(__cplusplus)
}  // extern "C"
#endif
