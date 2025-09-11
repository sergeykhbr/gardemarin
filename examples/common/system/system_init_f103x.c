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
#include <mcu.h>

extern void uart_early_init();
uint32_t SystemCoreClock = 72000000;

int system_clock_hz() {
    int ret = 72000000;   // 
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

void system_init(void)
{
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SCB_registers_type *SCB = (SCB_registers_type *)SCB_BASE;
    FLASH_registers_type *FLASH = (FLASH_registers_type *)FLASH_R_BASE;
    AFIO_registers_type *AFIO = (AFIO_registers_type *)AFIO_BASE;
    uint32_t t1;


    /* Reset the RCC clock configuration to the default reset state*/
    /* Set HSION bit */
    t1 = read32(&RCC->CR);
    t1 |=  (1 << 0);            // [0] HSION (8 MHz F103)
    write32(&RCC->CR, t1);

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    t1 = (1 << 4)      // [4] PRFTBE prefetch buffer is enable
       | (0 << 3)      // [3] HLFCYA:half cycle access disabled (valid for HCLK < 24MHz)
       | (2 << 0);     // [2:0] LATENCY: 2 wstates for 72 mhz
    write32(&FLASH->ACR, t1);

    /* Reset CFGR register */
    // [27:24] MCO: MCU clock output 1: 0xx=no clock; 100=sys clock; 101=HSI; 110HSE; 111=PLL/2
    // [22] USBPRE: USB prescaler: 0=PLL/1.5; 1=not divided
    // [21:18] PLLMUL: PLL multiplicator factor. PLL output < 72 MHz
    // [17] PLLXTPRE: HSE divider for PLL entry: 0=not divided; 1=HSE/2
    // [16] PLLSRC: PLL entry clk source: 0=HSI/2; 1=HSE
    // [15:14] ADCPRE: ADC prescaler: 00=PCLK2/2; 01=PCLK2/4; 10=PCLK2/6; 11=PCLK2/8
    // [13:11] PPRE2: High speed APB2 clock: 0xx=HCLK not divideed (MAX 72 MHz)
    // [10:8] PPRE1: Low speed APB1 clock: 0xx=HCLK not divided; 100=HCLK/2 (MAX 36 MHz)
    // [7:4] HPRE: AHB prescaler: 0xxx=SYSCLK not divided (MAX 72 MHz)
    // [3:2] SWS: System clock swtich status: 00=HSI; 01=HSE; 10=PLL
    // [1:0] SW: System clock switch:  00=HSI; 01=HSE; 10=PLL used as SYSCLK
    write32(&RCC->CFGR, 0x00000000);

    /* Reset HSEON, CSSON and PLLON bits */
    // [24] PLLON: 0=PLL OFF
    // [19] CSSON: 0=Clock Security system OFF
    // [16] HSEON: 0=HSE OFF (need to disable to disable HSE bypass)
    t1 = read32(&RCC->CR);
    t1 &= 0xFEF6FFFF;
    write32(&RCC->CR, t1);

#if 0
    // No  need PLL2/PLL3 for now
    // HSE 8 MHz -> PREDIV1=1 => PLLMUL=9 =>PLLCLK=72
    //              PREDIV2 => PLL2MUL => PLL2CLK to MCO (MCU clock output)
    //              PREDIV2 => PLL3MUL => PLL3VCO to I2S2/I2S3 interfaces
    t1 = (0 << 0)   // [3:0] PREDIV1: 0=not divided
       | (7 << 4)   // [7:4] PREDIV2: 0=not divided; 7=div by 8 (1 MHz)
       | (6 << 8)   // [11:8] PLL2MUL: 00xx=reserved; 010x=reserved; 0110 =PLL2 x8
       | (6 << 12)  // [15:12] PLL3MUL: 00xx=reserved; 010x=reserved; 0110 =PLL2 x8
       | (0 << 16)  // [16] PREDIV1SRC: 0=HSE is input for prediv1; 1=PLL2 as PREDIV1 clock
       | (0 << 17)  // [17] I2S2SRC: I2S2 clock source: 0=SYSCLK; 1=PLL3VCO
       | (0 << 18); // [18] I2S3SRC: I2S3 clock source: 0=SYSCLK; 1=PLL3VCO
    write32(&RCC->CFGR2, t1);
#endif
 

    /* Reset HSEBYP bit could be disabled only if HSE is OFF */
    // [18] HSEBYP: HSE clock bypass: 0=not bypassed
    t1 = read32(&RCC->CR);
    t1 &= 0xFFFBFFFF;
    write32(&RCC->CR, t1);

    /* Clock interrupt register: disable all clock status interrupts */
    write32(&RCC->CIR, 0x00000000);

    // [27:24] MCO: MCU clock output 1: 0xx=no clock; 100=sys clock; 101=HSI; 110HSE; 111=PLL/2
    // [22] USBPRE: USB prescaler: 0=PLL/1.5; 1=not divided
    // [21:18] PLLMUL: PLL multiplicator factor. PLL output < 72 MHz
    // [17] PLLXTPRE: HSE divider for PLL entry: 0=not divided; 1=HSE/2
    // [16] PLLSRC: PLL entry clk source: 0=HSI/2; 1=HSE
    // [15:14] ADCPRE: ADC prescaler: 00=PCLK2/2; 01=PCLK2/4; 10=PCLK2/6; 11=PCLK2/8
    // [13:11] PPRE2: High speed APB2 clock: 0xx=HCLK not divideed (MAX 72 MHz)
    // [10:8] PPRE1: Low speed APB1 clock: 0xx=HCLK not divided; 100=HCLK/2 (MAX 36 MHz)
    // [7:4] HPRE: AHB prescaler: 0xxx=SYSCLK not divided (MAX 72 MHz)
    // [3:2] SWS: System clock swtich status: 00=HSI; 01=HSE; 10=PLL
    // [1:0] SW: System clock switch:  00=HSI; 01=HSE; 10=PLL used as SYSCLK
    t1 = (7 << 18)       // PLLMUL: 7=x9 = 72 MHz
       | (0 << 17)       // PLLXTPRE: LSB of division factor PREDIV1
       | (1 << 16)       // PLL source: 1= clock from PREDIV1; 0=HSI
       | (0x0 << 14)     // ADC  = 72/2 = 36 MHz
       | (0x0 << 11)     // APB2 = 72/1 = 72 MHz
       | (0x4 << 8)      // APB1 = 72/2 = 36 MHz
       | (0 << 4);       // AHB = PLL = 72 MHz
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

    t1 = read32(&RCC->APB2ENR);
    t1 |= (1 << 6)     // PE
        | (1 << 5)     // PD
        | (1 << 4)     // PC
        | (1 << 3)     // PB
        | (1 << 2)     // PA
        | (1 << 0);    // [0] AFIOEN Alternate function I/O clock enable
    write32(&RCC->APB2ENR, t1);

#if 0
    // Disable JTAG SW only
    // [26:24] SWJCFG: 000 JTAG+DP (reset); 001=JTAG+SW no NJTRST; 010=SW; 100=both Off
    t1 = read32(&AFIO->MAPR);
    t1 &= ~(0x7 << 24);
    t1 |= (0x2 << 24);
    write32(&AFIO->MAPR, t1);
#endif


    setup_nvic();
}
