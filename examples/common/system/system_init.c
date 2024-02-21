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

void system_set_clock() {
}

void system_init(void)
{
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    SCB_registers_type *SCB = (SCB_registers_type *)SCB_BASE;
    uint32_t t1;

    /* FPU settings */
    t1 = read32(&SCB->CPACR);
    /* set CP10 and CP11 Full Access */
    t1 |= (3UL << 10*2)
        | (3UL << 11*2);
    write32(&SCB->CPACR, t1);

    /* Reset the RCC clock configuration to the default reset state*/
    /* Set HSION bit */
    t1 = read32(&RCC->CR);
    write32(&RCC->CR, t1 | 0x00000001);

    /* Reset CFGR register */
    write32(&RCC->CFGR, 0x00000000);

    /* Reset HSEON, CSSON and PLLON bits */
    t1 = read32(&RCC->CR);
    t1 &= 0xFEF6FFFF;
    write32(&RCC->CR, t1);

    /* Reset PLLCFGR register */
    write32(&RCC->PLLCFGR, 0x24003010);

    /* Reset HSEBYP bit */
    t1 = read32(&RCC->CR);
    t1 &= 0xFFFBFFFF;
    write32(&RCC->CR, t1);

    /* Disable all interrupts */
    write32(&RCC->CIR, 0x00000000);

    system_set_clock();

    // Vector table rellocation apply here: SCB->VTOR
    nvic_irq_disable(0xffffffff);
    nvic_irq_clear(0xffffffff);
}
