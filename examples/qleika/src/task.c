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
#include <dht22.h>
#include <vprintfmt.h>
#include "task.h"
#include "gpio_cfg.h"

#define TEMPERATURE_INFO_LINE 0
#define PRESSURE_INFO_LINE    1
#define MOISTURE_INFO_LINE    2
#define LIGHT_INFO_LINE       5
#define WATERING_INFO_LINE    6

#define WATERING_WAIT_SEC 1800
#define WATERING_SEC 20

void task_init(task_data_type *data) {
    data->state = State_SplashScreen;
    data->state_changed_sec = 0;
    data->watering_cnt = WATERING_WAIT_SEC - 10;   // wait 10 seconds after water detected
    data->raw.lux = 9500;
    data->raw.water_level = 0;
    data->raw.temperature = 230;
    data->raw.moisture = 343;
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

void show_int_x10(int val, int line, int col) {
    // Length of "Watering in:" = 12. We have 8 symbol for numbers
    char tstr1[21];
    char tstr[21];
    int szmax = 20 - col;
    int sz = snprintf_lib(tstr1, szmax + 1, "%d.%d", val/10, val%10);
    for (int i = 0; i < szmax - sz; i++) {
         tstr[i] = ' ';
    }
    for (int i = 0; i < sz; i++) {
         tstr[szmax - sz + i] = tstr1[i];
    }
    tstr[szmax] = 0;
    display_outputText24Line(tstr, line, col, 0xffff, 0x0000); // clear number field
}

void udpate_raw_data(raw_meas_type *raw) {
    // Light measurement
    if (is_lux_error()) {
        reset_lux();
    } else {
        update_lux();
        while (is_lux_busy()) {}
        raw->lux = get_lux();
    }

    // water level
    raw->water_level = gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA);

    dht_update();
    raw->temperature = dht_get_temperature();
    raw->moisture = dht_get_moisture();
}

void set_state(task_data_type *data, int sec, estate_type state_next) {
    data->state = state_next;
    data->state_changed_sec = sec;
}

int is_time_to_watering(task_data_type *data) {
    int ret = 0;
    if (!data->raw.water_level) {
        data->watering_cnt = WATERING_WAIT_SEC - 10;   // wait 10 seconds after water detected
        return ret;
    }

    data->watering_cnt++;
    if (data->watering_cnt >= WATERING_WAIT_SEC) {
        ret = 1;
    }
    if (data->watering_cnt > (WATERING_WAIT_SEC + WATERING_SEC)) {
        data->watering_cnt = 0;
    }
    return ret;
}

void pump_enable() {
    gpio_pin_set(&CFG_PIN_RELAIS_PUMP);
}

void pump_disable() {
    gpio_pin_clear(&CFG_PIN_RELAIS_PUMP);
}


void task_update(task_data_type *data, int sec) {
    raw_meas_type raw;

    switch (data->state) {
    case State_SplashScreen:
        data->state = State_Wait;
        data->state_next = State_SelfTest;
        data->wait_cnt = 4;
        break;
    case State_Wait:
        if (--data->wait_cnt <= 0) {
            set_state(data, sec, data->state_next);
        }
        break;
    case State_SelfTest:
        display_outputText24Line("T:              23.0", TEMPERATURE_INFO_LINE, 0, 0x07E0, 0x7813);
        show_int_x10(data->raw.temperature, TEMPERATURE_INFO_LINE, 2);
        display_outputText24Line("Pressure:        956", PRESSURE_INFO_LINE, 0, 0xffff, 0x0000);
        display_outputText24Line("Moisture:       34.4", MOISTURE_INFO_LINE, 0, 0xffff, 0x0000);
        show_int_x10(data->raw.moisture, MOISTURE_INFO_LINE, 9);
        display_outputText24Line("Air 2.5:        13.5", 3, 0, 0xffff, 0x0000);
        display_outputText24Line("Air 1.25:        5.5", 4, 0, 0xffff, 0x0000);
        display_outputText24Line("Light:", LIGHT_INFO_LINE, 0, 0xffff, 0x0000);
        show_int(data->raw.lux, LIGHT_INFO_LINE, 6);
        display_outputText24Line("No Water            ", WATERING_INFO_LINE, 0, 0xffff, 0xa2a8);
        // Clear other strings
        display_outputText24Line("                    ", 7, 0, 0xffff, 0x0000);

        veml7700_configure();
        set_state(data, sec, State_Idle);
        break;
    case State_Idle:
        udpate_raw_data(&raw);
    
        // Show watering line (6) on display
        if (raw.water_level != data->raw.water_level) {
            // State is changed:
            if (raw.water_level) {
                display_outputText24Line("Watering in:        ", WATERING_INFO_LINE, 0, 0xffff, 0x0000);
            } else {
                display_outputText24Line("No Water            ", WATERING_INFO_LINE, 0, 0xffff, 0xa2a8);
            }
        } else {
            if (raw.water_level) {
                // show time to net watering
                int t1 = WATERING_WAIT_SEC - data->watering_cnt;
                if (t1 < 0) {
                    t1 = 0;   // now is watering
                }
                show_int(t1, WATERING_INFO_LINE, 12);
            } else {
                // do nothing
            }
        }
        // pump control
        if (is_time_to_watering(data)) {
            pump_enable(data);
        } else {
            pump_disable(data);
        }
    
        if (raw.temperature != data->raw.temperature) {
            show_int_x10(raw.lux, TEMPERATURE_INFO_LINE, 2);
        }
        if (raw.moisture != data->raw.moisture) {
            show_int_x10(raw.moisture, MOISTURE_INFO_LINE, 9);
        }
        if (raw.lux != data->raw.lux) {
            show_int(raw.lux, LIGHT_INFO_LINE, 6);
        }
   
        data->raw = raw;
        break;
    default:;
    }


}

