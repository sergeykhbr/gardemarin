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

uint32_t SystemCoreClock = 144000000;

int system_clock_hz() {
    int ret = 144000000;   // 144MHz, max is 168 MHz (higher power consumption +1 flash wait state)
    return ret;
}

void setup_gpio() {
    GPIO_registers_type *P = (GPIO_registers_type *)GPIOA_BASE;
    uint32_t t1;

    // PORTA:
    //    PA15 SPI3_NSS = AF6 (JTDI default after reset)
    //    PA14 JTCK/SWCLK = AF0 (default after reset)
    //    PA13 JTMS/SWDIO = AF0 (default after reset)
    //    PA12 none
    //    PA11 Temperature2 input (external pull-up)
    //    PA10 USART1_RX = AF7
    //    PA9  USART1_TX = AF7
    //    PA8  Temperature1 input (external pull-up)
    //    PA7  none
    //    PA6  MOTOR_I_SENSOR4 = Analog ADC_IN6
    //    PA5  MOTOR_I_SENSOR3 = Analog ADC_IN5
    //    PA4  MOTOR_I_SENSOR2 = Analog ADC_IN4
    //    PA3  MOTOR_I_SENSOR1 = Analog ADC_IN3
    //    PA2  ETH_MDIO = AF11
    //    PA1  ETH_REF_CLK = AF11
    //    PA0  MOTOR_I_SENSOR0 = Analog ADC_IN0
    t1 = (2 << (15*2))     // [15] alternate:  SPI3_NSS
       | (2 << (14*2))     // [14] alternate:  SWCLK
       | (2 << (13*2))     // [13] alternate: SWDIO
       | (2 << (10*2))     // [10] alternate: USART1_RX
       | (2 << (9*2))      // [9] alternate: USART1_TX
       | (3 << (6*2))      // [6] analog ADC_IN6
       | (3 << (5*2))      // [5] analog ADC_IN5
       | (3 << (4*2))      // [4] analog ADC_IN4
       | (3 << (3*2))      // [3] analog ADC_IN3
       | (2 << (2*2))      // [2] alternate: ETH_MDIO
       | (2 << (1*2))      // [1] alternate: ETH_REF_CLK
       | (3 << (0*2));     // [0] analog ADC_IN0
    write32(&P->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&P->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = (1 << 11)         // [11] Temp2 medium
       | (1 << 8);         // [8] Temp1 medium
    write32(&P->OSPEEDR, t1);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    write32(&P->PUPDR, 0);
    // [15:0] ODRy: port output data
    write32(&P->ODR, 0);
    // Alternate function [3:0] per bit
    t1 = (11 << 2*4)           // [2]  AF11 ETH_MDIO
       | (11 << 1*4);          // [1]  AF11 ETH_REF_CLK
    write32(&P->AFR[0], t1);
    t1 = (6 << (15 - 8)*4)     // [15] AF6 SPI3_NSS
       | (0 << (14 - 8)*4)     // [14] AF0 SWCLK
       | (0 << (13 - 8)*4)     // [13] AF0 SWDIO
       | (7 << (10 - 8)*4)     // [10] AF7 USART1_RX
       | (7 << (9 - 8)*4);     // [9]  AF7 USART1_TX
    write32(&P->AFR[1], t1);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    t1 = (1 << 15)
       | (1 << 14)
       | (1 << 13)
       | (1 << 11)
       | (1 << 10)
       | (1 << 9)
       | (1 << 8)
       | (1 << 6)
       | (1 << 5)
       | (1 << 4)
       | (1 << 3)
       | (1 << 2)
       | (1 << 1)
       | (1 << 0);
    write32(&P->LCKR, t1);

    // PORTB
    //    PB13 ETH_D1     AF11
    //    PB12 ETH_D0     AF11
    //    PB11 ETH_TX_EN  AF11
    //    PB6  CAN2_RX    AF9
    //    PB5  CAN2_TX    AF9
    //    PB4  NJTRST unused
    //    PB3  JTDO   unused
    //    PB1  MOTOR_I_SENSOR6 = Analog ADC_IN9
    //    PB0  MOTOR_I_SENSOR5 = Analog ADC_IN8
    P = (GPIO_registers_type *)GPIOB_BASE;
    t1 = (2 << (13*2))     // [13] alternate: 
       | (2 << (12*2))     // [12] alternate: 
       | (2 << (11*2))     // [11] alternate: 
       | (2 << (6*2))     // [6] alternate: 
       | (2 << (5*2))     // [5] alternate: 
       | (3 << (1*2))     // [1] analog:  ADC_IN9
       | (3 << (0*2));    // [0] analog:  ADC_IN8
    write32(&P->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&P->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    write32(&P->OSPEEDR, 0);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    write32(&P->PUPDR, 0);
    // [15:0] ODRy: port output data
    write32(&P->ODR, 0);
    // Alternate function [3:0] per bit
    t1 = (9 << 6*4)     // [6] AF9 CAN2_RX
       | (9 << 5*4);    // [5] AF9 CAN2_TX
    write32(&P->AFR[0], t1);
    t1 = (11 << (13 - 8)*4)     // [13] AF11 ETH_TX_D1
       | (11 << (12 - 8)*4)     // [12] AF11 ETH_TX_D0
       | (11 << (11 - 8)*4);    // [11] AF11 ETH_TX_EN
    write32(&P->AFR[1], t1);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    t1 = (1 << 13)
       | (1 << 12)
       | (1 << 11)
       | (1 << 6)
       | (1 << 5)
       | (1 << 1)
       | (1 << 0);
    write32(&P->LCKR, t1);

    // PORTC:
    //    PC13 - User btn (internal pull-up). 0=btn is pressed
    //    PC12 SPI3_MOSI  AF6
    //    PC11 SPI3_MISO  AF6
    //    PC10 SPI3_SCK   AF6
    //    PC5  ETH_RX_D1  AF11
    //    PC4  ETH_RX_D0  AF11
    //    PC1  ETH_MDC    AF11
    //    PC0  MOTOR_I_SENSOR7 = Analog ADC_IN10
    P = (GPIO_registers_type *)GPIOC_BASE;
    t1 = (0 << (13*2))     // [13] input: User btn (internal pull-up). 0=btn is pressed
       | (2 << (12*2))     // [12] alternate: SPI3_MOSI
       | (2 << (11*2))     // [11] alternate: SPI3_MISO
       | (2 << (10*2))     // [10] alternate: SPI3_SCK
       | (2 << (5*2))      // [5] alternate: ETH_RX_D1
       | (2 << (4*2))      // [4] alternate: ETH_RX_D0
       | (2 << (1*2))      // [1] alternate: ETH_MDC
       | (3 << (0*2));     // [0] analog: ADC_IN10
    write32(&P->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&P->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    write32(&P->OSPEEDR, 0);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    t1 = (1 << (13*2));   // [13] pull-up: User Btn
    write32(&P->PUPDR, t1);
    // [15:0] ODRy: port output data
    write32(&P->ODR, 0);
    // Alternate function [3:0] per bit
    t1 = (11 << 5*4)           // [5] AF11 ETH_RX_D1
       | (11 << 4*4)           // [4] AF11 ETH_RX_D0
       | (11 << 1*4);          // [1] AF11 ETH_MDC
    write32(&P->AFR[0], t1);
    t1 = (6 << (12 - 8)*4)     // [12] AF6 SPI3_MOSI
       | (6 << (11 - 8)*4)     // [11] AF6 SPI3_MISO
       | (6 << (10 - 8)*4);    // [10] AF6 SPI3_SCK
    write32(&P->AFR[1], t1);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    t1 = (1 << 13)
       | (1 << 12)
       | (1 << 11)
       | (1 << 10)
       | (1 << 5)
       | (1 << 4)
       | (1 << 1)
       | (1 << 0);
    write32(&P->LCKR, t1);

    // PORTD:
    //    PD14 RELAY1 output
    //    PD13 RELAY0 output
    //    PD11 DRV3_MODE output
    //    PD10 DRV2_MODE output
    //    PD9  DRV1_MODE output
    //    PD8  DRV0_MODE output
    //    PD7  SPI_CS3 output
    //    PD6  USART2_TX  AF7
    //    PD5  USART2_TX  AF7
    //    PD4  SPI_CS2 output
    //    PD3  SPI_CS1 output
    //    PD2  SPI_CS0 output
    //    PD1  CAN1_TX    AF9
    //    PD0  CAN1_RX    AF9
    P = (GPIO_registers_type *)GPIOD_BASE;
    t1 = (1 << (14*2))     // [14] output: RELAY1
       | (1 << (13*2))     // [13] output: RELAY0
       | (1 << (11*2))     // [11] output: DRV3_MODE
       | (1 << (10*2))     // [10] output: DRV2_MODE
       | (1 << (9*2))      // [9]  output: DRV1_MODE
       | (1 << (8*2))      // [8]  output: DRV0_MODE
       | (1 << (7*2))      // [7]  output: SPI_CS3
       | (2 << (6*2))      // [6] alternate: USART2_TX
       | (2 << (5*2))      // [5] alternate: USART2_RX
       | (1 << (4*2))      // [4]  output: SPI_CS2
       | (1 << (3*2))      // [3]  output: SPI_CS1
       | (1 << (2*2))      // [2]  output: SPI_CS0
       | (2 << (1*2))      // [1] alternate: CAN1_TX
       | (2 << (0*2));     // [0] alternate: CAN1_RX
    write32(&P->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&P->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = (2 << 7*2)        // SPI_CS3 high
       | (2 << 4*2)        // SPI_CS2 high
       | (2 << 3*2)        // SPI_CS1 high
       | (2 << 2*2);       // SPI_CS0 high
    write32(&P->OSPEEDR, t1);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    t1 = 0;
    write32(&P->PUPDR, t1);
    // [15:0] ODRy: port output data
    t1 = (0 << 14)       // RELAY1
       | (0 << 13)       // RELAY0
       | (1 << 11)       // DRV3_MODE
       | (1 << 10)       // DRV2_MODE
       | (1 << 9)        // DRV1_MODE
       | (1 << 8)        // DRV0_MODE
       | (1 << 7)        // SPI_CS3 high
       | (1 << 4)        // SPI_CS2 high
       | (1 << 3)        // SPI_CS1 high
       | (1 << 2);       // SPI_CS0 high
    write32(&P->ODR, t1);
    // Alternate function [3:0] per bit
    t1 = (7 << 6*4)            // [6] AF7 USART2_RX
       | (7 << 5*4)            // [5] AF7 USART2_TX
       | (9 << 1*4)            // [1] AF9 CAN1_TX
       | (9 << 0*4);           // [0] AF9 CAN1_RX
    write32(&P->AFR[0], t1);
    t1 = 0;
    write32(&P->AFR[1], t1);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    write32(&P->LCKR, 0x00006FFF);

    // PORTE
    //    PE15 LED DRV3 output
    //    PE2 - User LED (Low=ON; High=Off)
    //    PE1  LED DRV2 output
    //    PE0  LED DRV1 output
    P = (GPIO_registers_type *)GPIOE_BASE;
    t1 = (0x1 << (15*2))    // [15] ouptut: LED DRV3 (0=LED PWR disabled; 1=enabled)
       | (0x1 << (2*2))     // [2] output: User LED (Low=ON; High=Off)
       | (0x1 << (1*2))     // [1] output: LED DRV2 (0=LED PWR disabled; 1=enabled)
       | (0x1 << (0*2));    // [0] output: LED DRV1 (0=LED PWR disabled; 1=enabled)
    write32(&P->MODER, t1);
    // [15:0] OTy: 0=push-pull output (reset state); 1=open drain output
    write32(&P->OTYPER, 0);
    // [31:0] OSPEEDRy[1:0]: 00=LowSpeed; 01=Medium; 10=High; 11=VeryHigh
    t1 = (0x1 << (15*2))    // [15] medium speed
       | (0x0 << (2*2))     // [2] low speed
       | (0x1 << (1*2))     // [1] medium speed
       | (0x1 << (0*2));    // [0] medium speed
    write32(&P->OSPEEDR, t1);
    // [15:0] PUPDRy[1:0]: 00=no pull-up/down; 01=Pull-up; 10=pull-down
    t1 = 0;                 // 
    write32(&P->PUPDR, t1);
    // [15:0] ODRy: port output data
    t1 = (1 << 2);          // User LED is off
    write32(&P->ODR, t1);
    // Alternate function [3:0] per bit
    write32(&P->AFR[0], 0);
    write32(&P->AFR[1], 0);
    // [16] LCKK: Lock key (whole register)
    // [15:0] LCKy: Lock bit
    t1 = (1 << 15)
       | (1 << 2)
       | (1 << 1)
       | (1 << 0);
    write32(&P->LCKR, t1);
}

void setup_uart() {
    USART_registers_type *UART1 = (USART_registers_type *)USART1_BASE;
    uint16_t fraction;
    uint16_t mantissa;
    uint16_t t1;

    // UART1 on APB2 = 72 MHz
    // 72000000/(16*115200) = 39.0625
    fraction = 1;     // 0.0625 * 16
    mantissa = 39;
    write16(&UART1->BRR, (mantissa << 4) | fraction);

    // [15] OVER8: Oversampling: 0=16; 1=8
    // [13] UE: USART enable
    // [12] M: word length: 0=8 data, 1=9 data
    // [11] WAKE
    // [10] PCE: Parity control en
    // [9] PS: Parity selection
    // [8] PEIE: PE irq ena
    // [7] TXEIE: TXE irq ena
    // [6] TCIE: Transmission complete irq en
    // [5] RXNEIE: RXNE irq ena
    // [4] EDLEIE: IDLE irq ena
    // [3] TE: transmitter ena
    // [2] RE: receiver ena
    // [1] RWU: Receiver wake-up
    // [0] SBRK: send break
    t1 = (1 << 13)
       | (1 << 3)
       | (1 << 2);
    write16(&UART1->CR1, t1);
    write16(&UART1->CR2, 0);
    write16(&UART1->CR3, 0);
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
    nvic_irq_disable(-1);
    nvic_irq_clear(-1);

    // [30] OTGHSULPIEN: USB OTG HSULPI clock enable
    // [29] OTGHSEN: USB OTG HS clock enable
    // [28] ETHMACPTPEN: Ethernet PTP clock enable. !Clock must be enabled after RMII selection
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
        | (1 << 3)     // PD
        | (1 << 2)     // PC
        | (1 << 1)     // PB
        | (1 << 0);    // PA
    write32(&RCC->AHB1ENR, t1);

    // [26] LTDCEN: LTDC clock en
    // [22] SAI1EN: SAI1EN clock en
    // [21] SAI6EN: SAI6EN clock en
    // [20] SAI5EN: SAI5EN clock en
    // [18] TIM11EN:
    // [17] TIM10EN:
    // [16] TIM9EN:
    // [14] SYSCFGEN:
    // [13] SPI4EN:
    // [12] SPI1EN:
    // [11] SDIOEN:
    // [10] ADC3EN:
    // [9] ADC2EN:
    // [8] ADC1EN:
    // [5] USART6EN:
    // [4] USART1EN:
    // [1] TIM8EN:
    // [0] TIM1EN:
    t1 = (1 << 4);            // APB2[4] USART1
    write32(&RCC->APB2ENR, t1);

    setup_gpio();
    setup_uart();
    setup_eth();
}
