/*
 *  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "prjtypes.h"
#include <stm32f4xx_map.h>

int system_clock_hz() {
    int ret = 168000000;   // default 168 MHz
    return ret;
}

void system_init(void)
{
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SCB_registers_type *SCB = (SCB_registers_type *)SCB_BASE;
    uint32_t t1;

    /* FPU settings */
    t1 = read32(&SCB->CPACR);    // Coprocessor Access Control Register
    /* set bits [23:20] to enable CP10 and CP11 coprocessors */
    t1 |= (3UL << 10*2)
        | (3UL << 11*2);
    write32(&SCB->CPACR, t1);

    /* Reset the RCC clock configuration to the default reset state*/
    /* Set HSION bit */
    t1 = read32(&RCC->CR);
    t1 |=  (1 << 0);            // [0] HSION (16 MHz)
    write32(&RCC->CR, t1);

    /* Reset CFGR register */
    // [31:30MCU clock output 2: 00=System clock (SYSCLK=HSI here); 01=PLLI2S; 10=HSE; 11=PLL
    // [29:27] MCO2 prescale: 0=no division
    // [26:24] MCO1 prescale: 0=no division
    // [23] I2S clock
    // [22:21] MCU clock output 1: 00=HSI
    // [20:16] RTC clock: 0 = no clock (should be 1 MHz)
    // [15:13] APB2 clock = AHB clock not divided (MAX 42 MHz)
    // [12:10] APB1 clock = AHB clock not divided (MAX 84 MHz)
    // [7:4] AHB clock not divided (MAX 168 MHz)
    // [3:2] clock is used: 00=HSI; 01=HSE; 10=PLL
    // [1:0] selected as a system clock: 00=HSI; 01=HSE; 10=PLL (16 MHz)
    write32(&RCC->CFGR, 0x00000000);

    /* Reset HSEON, CSSON and PLLON bits */
    // [24] PLLON: 0=PLL OFF
    // [19] CSSON: 0=Clock Security system OFF
    // [16] HSEON: 0=HSE OFF
    t1 = read32(&RCC->CR);
    t1 &= 0xFEF6FFFF;
    write32(&RCC->CR, t1);

    /* Reset PLLCFGR register */
    // [27:24] PLLQ: Divsion factor ofr USB OTG, SDIO and rnd, must be lower 48 MHz
    // [22] PLLSRC: PLL source: 0=HSI; 1=HSE (8MHz)
    // [17:16] PLLP: Main PLL division factor: 00=2; 01=4... Must be less than 168 MHz
    // [14:6] PLLN: Multiplication factor of the main PLL. (16/PLLM)*PLLN must be in range [50, 432]
    //              16 / 16 * 168 = 336 MHz (VCO input frequency)
    // [5:0] PLLM: Division factor for the main PLL before VCO = 16. In range 1..2 MHz to limit jitter
    t1 = (4 << 24)    // 168 / 4 = 42 MHz
       | (0 << 22)    // HSI
       | (0 << 16)    // PLL output = VCO/2
       | (336 << 6)   // to form 168 MHz on PLL output
       | (16 << 0);   // HSI /16 = 16 / 16 = 1MHz
    write32(&RCC->PLLCFGR, t1); // 0x24003010);

    /* Reset HSEBYP bit could be disabled only if HSE is OFF */
    // [18] HSEBYP: HSE clock bypass: 0=not bypassed
    t1 = read32(&RCC->CR);
    t1 &= 0xFFFBFFFF;
    write32(&RCC->CR, t1);

    /* Clock interrupt register: disable all clock status interrupts */
    write32(&RCC->CIR, 0x00000000);

#if 0
    /* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
    
    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;


    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
   
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_4WS;

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    {
    }
#endif

    // Vector table rellocation apply here: SCB->VTOR
    nvic_irq_disable(0xffffffff);
    nvic_irq_clear(0xffffffff);
}
