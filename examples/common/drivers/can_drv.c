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
#include <memanager.h>
#include <can.h>
#include "can_drv.h"

void can_init() {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    can_type *p = (can_type *)fw_get_ram_data(CAN_DRV_NAME);
    uint32_t t1;
    if (p == 0) {
         return;
    }

    //    PD1  CAN1_TX    AF9
    //    PD0  CAN1_RX    AF9
    p->ctrl[0].gpio_cfg_tx.port = (GPIO_registers_type *)GPIOD_BASE;
    p->ctrl[0].gpio_cfg_tx.pinidx = 1;
    gpio_pin_as_alternate(&p->ctrl[0].gpio_cfg_tx, 9);

    p->ctrl[0].gpio_cfg_rx.port = (GPIO_registers_type *)GPIOD_BASE;
    p->ctrl[0].gpio_cfg_rx.pinidx = 0;
    gpio_pin_as_alternate(&p->ctrl[0].gpio_cfg_rx, 9);

    //    PB6  CAN2_RX    AF9
    //    PB5  CAN2_TX    AF9
    p->ctrl[1].gpio_cfg_tx.port = (GPIO_registers_type *)GPIOB_BASE;
    p->ctrl[1].gpio_cfg_tx.pinidx = 5;
    gpio_pin_as_alternate(&p->ctrl[1].gpio_cfg_tx, 9);

    p->ctrl[1].gpio_cfg_rx.port = (GPIO_registers_type *)GPIOB_BASE;
    p->ctrl[1].gpio_cfg_rx.pinidx = 6;
    gpio_pin_as_alternate(&p->ctrl[1].gpio_cfg_rx, 9);


    // Enable clock CAN1 and CAN2
    t1 = read32(&RCC->APB1ENR);
    t1 |= (0x3 << 25);            // APB1[26] CAN2 EN; APB1[25] CAN1 EN
    write32(&RCC->APB1ENR, t1);


}
