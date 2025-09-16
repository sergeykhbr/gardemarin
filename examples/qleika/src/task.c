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
#include <i2c_veml7700.h>
#include <vprintfmt.h>
#include "task.h"
#include "gpio_cfg.h"

void task_init(task_data_type *data) {
    data->state = State_SplashScreen;
    data->state_changed_sec = 0;
    data->water_sensor = WaterLevel_Off;
}

void show_int(int val, int line, int col) {
    // Length of "Watering in:" = 12. We have 8 symbol for numbers
    char tstr1[21];
    char tstr[21];
    int szmax = 20 - col;
    int sz = snprintf_lib(tstr1, szmax + 1, "%d", val);
    for (int i = 0; i < szmax - sz; i++) {
         tstr[i] = ' ';
    }
    for (int i = 0; i < sz; i++) {
         tstr[szmax - sz + i] = tstr1[i];
    }
    tstr[szmax] = 0;
    display_outputText24Line(tstr, line, col, 0xffff, 0x0000); // clear number field
}

static void set_state_idle(task_data_type *data, int sec) {
    data->state_changed_sec = sec;
    data->state = State_Idle;
    display_outputText24Line("Watering in:        ", 5, 0, 0xffff, 0x0000);
}

static void set_state_no_water(task_data_type *data, int sec) {
    data->state_changed_sec = sec;
    data->state = State_NoWater;
    display_outputText24Line("No Water            ", 5, 0, 0xffff, 0xa2a8);
}

void task_update(task_data_type *data, int sec) {
    switch (data->state) {
    case State_SplashScreen:
        data->state = State_Wait;
        data->state_next = State_SelfTest;
        data->wait_cnt = 4;
        break;
    case State_Wait:
        if (--data->wait_cnt <= 0) {
            data->state = data->state_next;
            //data->lux = veml32_lux();
            data->lux = 9500;
        }
        break;
    case State_SelfTest:
        display_outputText24Line("T:              23.0", 0, 0, 0x07E0, 0x7813);
        display_outputText24Line("Moisture:       34.4", 1, 0, 0xffff, 0x0000);
        display_outputText24Line("Air 2.5:        13.5", 2, 0, 0xffff, 0x0000);
        display_outputText24Line("Air 1.25:        5.5", 3, 0, 0xffff, 0x0000);
        display_outputText24Line("Light:", 4, 0, 0xffff, 0x0000);
        show_int(data->lux, 4, 6);
        display_outputText24Line("                    ", 5, 0, 0xffff, 0x0000);
        // Clear other strings
        display_outputText24Line("                    ", 6, 0, 0xffff, 0x0000);
        display_outputText24Line("                    ", 7, 0, 0xffff, 0x0000);

        if (gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA)) {
            set_state_idle(data, sec);
        } else {
            set_state_no_water(data, sec);
        }
        break;
    case State_NoWater:
        if (gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA)) {
            set_state_idle(data, sec);
        }
        break;
    case State_Watering:
        break;
    case State_Idle:
        if (!gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA)) {
            set_state_no_water(data, sec);
        } else {
            int t1 = sec - data->state_changed_sec;
            show_int(t1, 5, 12);
        }
        break;
    default:;
    }

    if (data->state > State_SelfTest) {
        data->lux = update_lux_raw();
        show_int(data->lux, 4, 6);
    }

}

