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

#include <inttypes.h>

/** 
  * @brief External Interrupt/Event Controller
  */

typedef struct EXTI_registers_type
{
    volatile uint32_t IMR;    /*!< EXTI Interrupt mask register,            Address offset: 0x00 */
    volatile uint32_t EMR;    /*!< EXTI Event mask register,                Address offset: 0x04 */
    volatile uint32_t RTSR;   /*!< EXTI Rising trigger selection register,  Address offset: 0x08 */
    volatile uint32_t FTSR;   /*!< EXTI Falling trigger selection register, Address offset: 0x0C */
    volatile uint32_t SWIER;  /*!< EXTI Software interrupt event register,  Address offset: 0x10 */
    volatile uint32_t PR;     /*!< EXTI Pending register,                   Address offset: 0x14 */
} EXTI_registers_type;

