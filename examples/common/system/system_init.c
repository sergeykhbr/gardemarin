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

// Typical power consumption at 25 C:
//   168 MHz 87 mA
//   144 MHz 67 mA   - make sense to reduce frequency lower 150 MHz,
//                     4 Flash wait states instead of 5 at 168 MHz
// Voltage scaling is adjusted to Fhclk frequency:
//   Scale 2 for F_HCLK <= 144 MHz
//   Scale 1 for F_HCLK <= 168 MHz

int system_clock_hz() {
    int ret = 144000000;   // 144MHz, max is 168 MHz (higher power consumption +1 flash wait state)
    return ret;
}

void setup_gpio() {
    GPIO_registers_type *PC = (GPIO_registers_type *)GPIOC_BASE;
    GPIO_registers_type *PE = (GPIO_registers_type *)GPIOE_BASE;
    uint32_t t1;

    // PORTC:
    //    PC13 - User btn (internal pull-up). 0=btn is pressed
    t1 = (0 << (13*2));     // [13] input: User btn (internal pull-up). 0=btn is pressed
    write32(&PC->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&PC->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    write32(&PC->OSPEEDR, 0);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    t1 = (1 << (13*2));   // [13] pull-up
    write32(&PC->PUPDR, t1);
    // [15:0] ODRy: port output data
    write32(&PC->ODR, 0);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    t1 = (1 << 13);
    write32(&PC->LCKR, t1);
    // Alternate function [3:0] per bit
    write32(&PC->AFR[0], 0);
    write32(&PC->AFR[1], 0);

    // PORTE
    //    PE2 - User LED (Low=ON; High=Off)
    t1 = (0x1 << (15*2))    // [15] ouptut: LED DRV3 (0=LED PWR disabled; 1=enabled)
       | (0x1 << (2*2))     // [2] output: User LED (Low=ON; High=Off)
       | (0x1 << (1*2))     // [1] output: LED DRV2 (0=LED PWR disabled; 1=enabled)
       | (0x1 << (0*2));    // [0] output: LED DRV1 (0=LED PWR disabled; 1=enabled)
    write32(&PE->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&PE->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = (0x1 << (15*2))    // [15] medium speed
       | (0x0 << (2*2))     // [2] low speed
       | (0x1 << (1*2))     // [1] medium speed
       | (0x1 << (0*2));    // [0] medium speed
    write32(&PE->OSPEEDR, t1);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    t1 = 0;                 // 
    write32(&PE->PUPDR, t1);
    // [15:0] ODRy: port output data
    t1 = (1 << 2);          // User LED is off
    write32(&PE->ODR, t1);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    t1 = (1 << 15)
       | (1 << 2)
       | (1 << 1)
       | (1 << 1);
    write32(&PE->LCKR, t1);
    // Alternate function [3:0] per bit
    write32(&PE->AFR[0], 0);
    write32(&PE->AFR[1], 0);

}

void system_init(void)
{
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SCB_registers_type *SCB = (SCB_registers_type *)SCB_BASE;
    PWR_registers_type *PWR = (PWR_registers_type *)PWR_BASE;
    FLASH_registers_type *FLASH = (FLASH_registers_type *)FLASH_BASE;
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
       | (0 << 22)    // HSI
       | (0 << 16)    // PLL output = VCO/2
       | (288 << 6)   // to form 144 MHz on PLL output
       | (16 << 0);   // HSI /16 = 16 / 16 = 1MHz
    write32(&RCC->PLLCFGR, t1); // 0x24003010);

    /* Reset HSEBYP bit could be disabled only if HSE is OFF */
    // [18] HSEBYP: HSE clock bypass: 0=not bypassed
    t1 = read32(&RCC->CR);
    t1 &= 0xFFFBFFFF;
    write32(&RCC->CR, t1);

    /* Clock interrupt register: disable all clock status interrupts */
    write32(&RCC->CIR, 0x00000000);

    // Power controller on APB1 inteconnect
    // [28] PWREN: Power interface clock ena: 1=Enabled
    t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 28);            // APB1[28] PWR controller
    write32(&RCC->APB1ENR, t1);
    // [14] VOS: Regulator voltage scaling. Trade-off between performance and power consumption.
    // [9] FPDS: Flash power down
    // [8] DBP: Disable backup domain write protection. Must be set to 1 to write into backup registers
    // [7:5] PLS[2:0]: PVD level detection. 000=2.0V,...,111=2.9V
    // [4] PVDE: Programmable voltage detector enable
    // [3] CSBF: Clear standby flag
    // [2] CWUF: Clear wake-up flag
    // [1] PDDS: Power-down deepsleep
    // [0] LPDS: Low power deepsleep.
    t1 = read32(&PWR->CR);
    t1 &= ~(1 << 14);       // 0=Scale 2 mode at 144 MHz (default is 1 =Scale 1 mode at reset 168 MHz)
    t1 |= (1 << 8);         // Write access to RTC backup registers and backup SRAM
    write32(&PWR->CR, t1);

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


    /* Enable the main PLL */
    // [24] PLLON: Main PLL enable
    // [16] HSEON: HSE clock enable (RTC source)
    t1 = read32(&RCC->CR);
    t1 |= (1 << 24)
        | (1 << 16);
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
    t1 = (1 << 10)     // dcache ea
       | (1 << 9)      // icache ena
       | (0 << 8)      // prefetch disabled
       | (4 << 0);     // 4 wstates at 144 MHz
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

    // Vector table rellocation apply here: SCB->VTOR
    nvic_irq_disable(0xffffffff);
    nvic_irq_clear(0xffffffff);

    // [30] OTGHSULPIEN: USB OTG HSULPI clock enable
    // [29] OTGHSEN: USB OTG HS clock enable
    // [28] ETHMACPTPEN: Ethernet PTP clock enable
    // [27] ETHMACRXEN: Ethernet Rx clock enable
    // [26] ETHMACTXEN: Ethernet Tx clock enable
    // [25] ETHMACEN: Ethernet MAC clock enable
    // [23] DMA2DEN: DMA2D clock ena
    // [22] DMA2EN: DMA2 clock en
    // [21] DMA1EN: DMA1 clock en
    // [20] CCMDATARAMEN: CCM data RAM clock en
    // [18] BKPSRAMEN: Backup SRAM clock en
    // [12] CRCEN: CRC clock en
    // [10] GPIOKEN: IO port K clock en
    // [9] GPIOJEN: IO port J clock en
    // [8] GPIOIEN: IO port I clock en
    // [7] GPIOHEN: IO port H clock en
    // [6] GPIOGEN: IO port G clock en
    // [5] GPIOFEN: IO port F clock en
    // [4] GPIOEEN: IO port E clock en
    // [3] GPIODEN: IO port D clock en
    // [2] GPIOCEN: IO port C clock en
    // [1] GPIOBEN: IO port B clock en
    // [0] GPIOAEN: IO port A clock en
    t1 = read32(&RCC->AHB1ENR);
    t1 |= (1 << 18)    // Backup SRAM
        | (1 << 4)     // PE
        | (1 << 2);    // PC
    write32(&RCC->AHB1ENR, t1);

    setup_gpio();
}
