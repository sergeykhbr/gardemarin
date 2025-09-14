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

#include <gpio_drv.h>
#include <spi_display.h>
#include "task.h"
#include "gpio_cfg.h"

void task_init(task_data_type *data) {
    data->state = State_SplashScreen;
    data->water_sensor = WaterLevel_Off;
}

void show_WaterSensor(task_data_type *data, int line) {
    switch(data->water_sensor) {
    case WaterLevel_Off:
        data->water_sensor = WaterLevel_Measure;
        display_outputText24Line("Level sensor     OFF", line, 0, 0xffff, 0xa2a8);
        break;
    case WaterLevel_Measure:
        if (gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA)) {
            data->water_sensor = WaterLevel_Full;
            display_outputText24Line("Watering in:      14", line, 0, 0xffff, 0x0000);
        } else {
            data->water_sensor = WaterLevel_Empty;
            display_outputText24Line("No Water            ", line, 0, 0xffff, 0xa2a8);
        }
        break;
    case WaterLevel_Full:
        if (!gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA)) {
            display_outputText24Line("No Water            ", line, 0, 0xffff, 0xa2a8);
            data->water_sensor = WaterLevel_Empty;
        }
        break;
    case WaterLevel_Empty:
        if (gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA)) {
            display_outputText24Line("Watering in:      14", line, 0, 0xffff, 0x0000);
            data->water_sensor = WaterLevel_Full;
        }
        break;
    default:;
    }
}

void task_update(task_data_type *data, int sec) {

    switch (data->state) {
    case State_SplashScreen:
        data->state = State_Wait;
        data->state_next = State_SelfTest;
        data->wait_cnt = 4;
        break;
    case State_SelfTest:
        display_outputText24Line("T:              23.0", 0, 0, 0x07E0, 0x7813);
        display_outputText24Line("Moisture:       34.4", 1, 0, 0xffff, 0x0000);
        display_outputText24Line("Air 2.5:        13.5", 2, 0, 0xffff, 0x0000);
        display_outputText24Line("Air 1.25:        5.5", 3, 0, 0xffff, 0x0000);
        display_outputText24Line("Light:         10000", 4, 0, 0xffff, 0x0000);
        display_outputText24Line("Level sensor     OFF", 5, 0, 0xffff, 0xa2a8);
        // Clear other strings
        display_outputText24Line("                    ", 6, 0, 0xffff, 0x0000);
        data->state = State_Idle;
        break;
    case State_Wait:
        if (--data->wait_cnt <= 0) {
            data->state = data->state_next;
        }
        break;
    case State_Idle:
        show_WaterSensor(data, 5);
        break;
    default:;
    }


}

