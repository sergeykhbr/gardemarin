/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include <scs.h>
#include <nvic.h>
#include <syscfg.h>
#include <rcc.h>
#include <pwr.h>
#include <flash.h>
#include <gpio.h>
#include <usart.h>
#include <exti.h>
#include <adc.h>
#include <can.h>
#include <dma.h>
#include <tim.h>
#include <rtc.h>
#include <spi.h>


#define FLASH_BASE            ((uint32_t)0x08000000) // FLASH (128 KB) base address in the alias region
#define SRAM_BASE             ((uint32_t)0x20000000) // SRAM(20 KB) base address in the alias region
#define PERIPH_BASE           ((uint32_t)0x40000000) // Peripheral base address in the alias region
#define USB_OTG_FS_BASE       ((uint32_t)0x50000000) // USB
#define FSMC_R_BASE           ((uint32_t)0xA0000000) // FSMC registers base address

#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x00020000)


/*!< APB1 peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800)
#define USB2_FS_BASE          (APB1PERIPH_BASE + 0x5C00)
#define SRAM_512B_BASE        (APB1PERIPH_BASE + 0x6000)  // shared 512B USB/CAN SRAM
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)
#define BKP_BASE              (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)

/*!< APB2 peripherals */
#define AFIO_BASE             (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE            (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE            (APB2PERIPH_BASE + 0x1800)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE             (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define USART1_BASE           (APB2PERIPH_BASE + 0x3800)

#define DMA_BASE              (AHBPERIPH_BASE + 0x0000)
#define RCC_BASE              (AHBPERIPH_BASE + 0x1000)
#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x2000)
#define CRC_BASE              (AHBPERIPH_BASE + 0x3000)


/*!< FSMC Bankx registers base address */
#define FSMC_Bank1_R_BASE     (FSMC_R_BASE + 0x0000)
#define FSMC_Bank1E_R_BASE    (FSMC_R_BASE + 0x0104)
#define FSMC_Bank2_R_BASE     (FSMC_R_BASE + 0x0060)
#define FSMC_Bank3_R_BASE     (FSMC_R_BASE + 0x0080)
#define FSMC_Bank4_R_BASE     (FSMC_R_BASE + 0x00A0)

/* Debug MCU registers base address */
#define DBGMCU_BASE           ((uint32_t )0xE0042000)
