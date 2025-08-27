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

extern void uart_early_init();
// Typical power consumption at 25 C:
//   168 MHz 87 mA
//   144 MHz 67 mA   - make sense to reduce frequency lower 150 MHz,
//                     4 Flash wait states instead of 5 at 168 MHz
// Voltage scaling is adjusted to Fhclk frequency:
//   Scale 2 for F_HCLK <= 144 MHz (APB1 = 36MHz, APB2 = 72MHz)
//   Scale 1 for F_HCLK <= 168 MHz

uint32_t SystemCoreClock = 144000000;

int system_clock_hz() {
    int ret = 144000000;   // 144MHz, max is 168 MHz (higher power consumption +1 flash wait state)
    return ret;
}

void system_delay_ns(int nsec) {
    // 1 lock at 144 mhz = 7nsec
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;
    uint32_t curval = read32(&systick->CVR);        // current value
    uint32_t curval_z = curval;
    uint32_t accval = 0;
    uint32_t us = (uint32_t)nsec / 1000;
    uint32_t ns = (uint32_t)(nsec / 7 ) % 1000;
    uint32_t one_us = system_clock_hz() / 1000000;

    while (us) {
        while (accval < one_us) {
            curval = read32(&systick->CVR);
            if (curval < curval_z) {
                accval += curval_z - curval;
                //uart_printk("1[%d]=%d\r\n", us, accval);
            } else {
                accval += curval_z + (read32(&systick->RVR) - curval); // reload value
                //uart_printk("2[%d]=%d\r\n", us, accval);
            }
            curval_z = curval;
        }
        accval = 0;
        us--;
    }
    while (accval < ns) {
        curval = read32(&systick->CVR);
        if (curval < curval_z) {
            accval += curval_z - curval;
        } else {
            accval += curval_z + (read32(&systick->RVR) - curval); // reload value
        }
        curval_z = curval;
    }
}

void setup_nvic() {
    SCB_registers_type *SCB = (SCB_registers_type *)SCB_BASE;

    // Vector table rellocation apply here: SCB->VTOR
    nvic_irq_disable(-1);
    nvic_irq_clear(-1);

    // [10:8] PRIGROUP
    //     111 0 bits for pre-emption prio, 4 bits for subprpiority (M3 TRM 0 vs 256)
    //     110 1 bits for pre-emption prio, 3 bits for subprpiority (M3 TRM 2 vs 128)
    //     ..
    //     011 4 bits for pre-emption prio, 0 bits for subprpiority (M3 TRM 16 vs 16)
    write32(&SCB->AIRCR, (3 << 8));   // by unclear reason only PRIORITY[7:4] bits are used in FreeRTOS

    //write32(&SCB->VTOR, NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

void setup_eth() {
    // The mode, MII or RMII, is selected using the configuration bit 23, MII_RMII_SEL, in the
    // SYSCFG_PMC register. The application has to set the MII/RMII mode while the Ethernet
    // controller is under reset or before enabling the clocks.
    SYSCFG_registers_type *SYSCFG = (SYSCFG_registers_type *)SYSCFG_BASE;
    uint32_t t1 = read32(&SYSCFG->PMC);
    t1 |= (1 << 23);    // RMII PHY selected
    write32(&SYSCFG->PMC, t1);
}

void system_init(void)
{
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SCB_registers_type *SCB = (SCB_registers_type *)SCB_BASE;
    FLASH_registers_type *FLASH = (FLASH_registers_type *)FLASH_R_BASE;
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
    // [31:30] MCU clock output 2: 00=System clock (SYSCLK=HSI here); 01=PLLI2S; 10=HSE; 11=PLL
    // [29:27] MCO2 prescale: 0=no division
    // [26:24] MCO1 prescale: 0=no division
    // [23] I2S clock
    // [22:21] MCU clock output 1: 00=HSI
    // [20:16] RTC clock: 0 = no clock (should be 1 MHz); HSE/8 = 1 MHz
    // [15:13] APB2 clock = AHB clock not divided (MAX 90 MHz)
    // [12:10] APB1 clock = AHB clock not divided (MAX 45 MHz)
    // [7:4] AHB clock not divided (MAX 168 MHz)
    // [3:2] clock is used: 00=HSI; 01=HSE; 10=PLL
    // [1:0] selected as a system clock: 00=HSI; 01=HSE; 10=PLL (16 MHz)
    write32(&RCC->CFGR, 0x00000000);

    /* Reset HSEON, CSSON and PLLON bits */
    // [24] PLLON: 0=PLL OFF
    // [19] CSSON: 0=Clock Security system OFF
    // [16] HSEON: 0=HSE OFF (need to disable to disable HSE bypass)
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
    t1 = (3 << 24)    // 144 / 3 = 48 MHz
       | (1 << 22)    // HSE
       | (0 << 16)    // PLL output = VCO/2
       | (288 << 6)   // to form 144 MHz on PLL output
       | (8 << 0);   // HSE /8 = 8 / 8 = 1MHz
    write32(&RCC->PLLCFGR, t1); // 0x24003010);

    /* Reset HSEBYP bit could be disabled only if HSE is OFF */
    // [18] HSEBYP: HSE clock bypass: 0=not bypassed
    t1 = read32(&RCC->CR);
    t1 &= 0xFFFBFFFF;
    write32(&RCC->CR, t1);

    /* Clock interrupt register: disable all clock status interrupts */
    write32(&RCC->CIR, 0x00000000);

    // [31:30] MCO2[1:0] MCU clock output 2: 00=System clock (SYSCLK=HSI here); 01=PLLI2S; 10=HSE; 11=PLL
    // [29:27] MCO2PRE: MCO2 prescale: 0=no division
    // [26:24] MCO1PRE: MCO1 prescale: 0=no division
    // [23] I2SSRC: I2S clock
    // [22:21] MCO1: MCU clock output 1: 00=HSI
    // [20:16] RTCPRE: RTC clock: 0 = no clock (should be 1 MHz), HSE/8=1MHz
    // [15:13] PPRE2: APB2 clock = AHB clock not divided (MAX 90 MHz): 0xx=1; 100=2; 101=4,..
    // [12:10] PPRE1: APB1 clock = AHB clock not divided (MAX 45 MHz): 0xx=1; 100=2; 101=4,..
    // [7:4] HPRE: AHB clock not divided (MAX 168 MHz)
    // [3:2] SWS: clock is used: 00=HSI; 01=HSE; 10=PLL
    // [1:0] SW: selected as a system clock: 00=HSI; 01=HSE; 10=PLL
    t1 = (8 << 16)       // RTC clock = HSE/8 = 1 MHz
       | (0x4 << 13)     // APB2 = 144/2 = 72 MHz
       | (0x5 << 10)     // APB1 = 144/4 = 36 MHz
       | (0 << 4);       // AHB = PLL = 144 MHz
    write32(&RCC->CFGR, t1);

    /* Enable the HSE 8 MHz */
    // [16] HSEON: HSE clock enable
    t1 = read32(&RCC->CR);
    t1 |=  (1 << 16);
    write32(&RCC->CR, t1);
    // [17] HSERDY: HSE clock ready flag
    while((read32(&RCC->CR) & (1 << 17)) == 0) {}


    /* Enable the main PLL */
    // [24] PLLON: Main PLL enable
    t1 = read32(&RCC->CR);
    t1 |= (1 << 24);
    write32(&RCC->CR, t1);
    /* Wait till the main PLL is ready */
    // [25] PLLRDY: Main PLL clock ready flag
    while((read32(&RCC->CR) & (1 << 25)) == 0) {}

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    // at Vdd = 2.7 .. 3.6 V Max flash memorya ccess freq with no wait state = 30 MHz (see table 15, page 81)
    // ART accelerator speed-up access. at 144MHz = 4 wait states, at 150-168 Mhz with 5 wait-states
    // [12] DCRST: Data cache reset (only when cache disabled)
    // [11] ICRST: Instruction cache reset (only when cache disabled)
    // [10] DCEN: Data cache enable
    // [9] ICEN: Data cache enable
    // [8] PRFTEN: Prefetch enable
    // [2:0] LATENCY: number of wait-states
    t1 = (1 << 10)     // dcache ena
       | (1 << 9)      // icache ena
       | (0 << 8)      // prefetch disabled
       | (4 << 0);     // 4 wstates for 144 mhz
    write32(&FLASH->ACR, t1);

    /* Select the main PLL as system clock source */
    // [31:30] MCO2[1:0] MCU clock output 2: 00=System clock (SYSCLK=HSI here); 01=PLLI2S; 10=HSE; 11=PLL
    // [29:27] MCO2PRE: MCO2 prescale: 0=no division
    // [26:24] MCO1PRE: MCO1 prescale: 0=no division
    // [23] I2SSRC: I2S clock
    // [22:21] MCO1: MCU clock output 1: 00=HSI
    // [20:16] RTCPRE: RTC clock: 0 = no clock (should be 1 MHz), HSE/8=1MHz
    // [15:13] PPRE2: APB2 clock = AHB clock not divided (MAX 90 MHz): 0xx=1; 100=2; 101=4,..
    // [12:10] PPRE1: APB1 clock = AHB clock not divided (MAX 45 MHz): 0xx=1; 100=2; 101=4,..
    // [7:4] HPRE: AHB clock not divided (MAX 168 MHz)
    // [3:2] SWS: clock is used: 00=HSI; 01=HSE; 10=PLL
    // [1:0] SW: selected as a system clock: 00=HSI; 01=HSE; 10=PLL
    t1 = read32(&RCC->CFGR);
    t1 &= ~(0x3);
    t1 |= 0x2;      // Select PLL
    write32(&RCC->CFGR, t1);

    /* Wait till the main PLL is used as system clock source */
    while (((read32(&RCC->CFGR) >> 2) & 0x3) != 0x2) {}

    t1 = read32(&RCC->AHB1ENR);
    t1 |= (1 << 4)     // PE
        | (1 << 3)     // PD
        | (1 << 2)     // PC
        | (1 << 1)     // PB
        | (1 << 0);    // PA
    write32(&RCC->AHB1ENR, t1);

    setup_nvic();
    uart_early_init();
    setup_eth();
}
