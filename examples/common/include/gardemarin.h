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

#define GARDEMARIN_RELAIS_TOTAL 2

#define GARDEMARIN_HBRDIGE_TOTAL 4
#define GARDEMARIN_DCMOTOR_PER_HBDRIGE 2

#define GARDEMARIN_LED_STRIP_TOTAL 4

#define GARDEMARIN_CAN_BUS_TOTAL 2

#define GARDEMARIN_LOAD_SENSORS_TOTAL 4

/**
  Motor drivers current sensors plus chip temperature, V_INTREF and VBAT
 */
#define GARDEMARIN_ADC_CHANNEL_USED 11

/**
  Temperature sensor with 1-Wire interface
 */
#define GARDEMARIN_DS18B20_TOTAL 2
