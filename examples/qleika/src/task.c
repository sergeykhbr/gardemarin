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
#include <i2c_common.h>
#include <i2c_veml7700.h>
#include <i2c_bmp280.h>
#include <dht22.h>
#include <air_d9.h>
#include <exti_btn.h>
#include <bkp.h>
#include <vprintfmt.h>
#include "task.h"
#include "gpio_cfg.h"

#define TEMPERATURE_INFO_LINE 0
#define PRESSURE_INFO_LINE    1
#define MOISTURE_INFO_LINE    2
#define PM1p0_INFO_LINE       3
#define PM2p5_INFO_LINE       4
#define PM10_INFO_LINE        5
#define LIGHT_INFO_LINE       6
#define WATERING_INFO_LINE    7

#define CLR_BLACK              0x0000
#define CLR_WHITE              0xFFFF
#define CLR_YELLOW             0xFF56
#define CLR_GREEN              0x07E0
#define CLR_VIOLET             0x7813
#define CLR_DARK_GREEN         0x0280
#define CLR_DARK_GREEN_YELLOW  0x2280
#define CLR_DARK_YELLOW        0x4A80
#define CLR_DARK_RED           0x50A1

typedef struct watering_cycle_type {
    int wait;
    int duration;
} watering_cycle_type;

static const watering_cycle_type WATERING_CYCLE[] = {
    {300, 10},     // [0] 5 minutes wait, 10 sec watering
    {600, 10},     // [1]
    {900, 15},     // [2]
    {1800, 20},    // [3]
    {3600, 20},    // [4]
    {2*3600, 25},  // [5]
    {6*3600, 30},  // [6]
    {12*3600, 45}, // [7]
};

void task_init(task_data_type *data) {
    int mode = bkp_get_watering_mode();
    data->watering_wait = WATERING_CYCLE[mode].wait;
    data->watering_duration = WATERING_CYCLE[mode].duration;

    data->state = State_SplashScreen;
    data->sec = 0;
    data->state_changed_sec = 0;
    data->status = STATUS_NO_WATER;
    data->status_changed_sec = 0;
    data->watering_cnt = data->watering_wait - 10;   // wait 10 seconds after water detected
    data->water_low = 0;
    data->watering_ena = 0;
    data->hour = 0;
    data->minute = 0;
    data->t_corr = 0;
    data->light = 0;
    data->raw.lux = 9500;
    data->raw.lux_error = 0;
    data->raw.pressure = 7500;
    data->raw.pressure_error = 0;
    data->raw.water_level = 0;
    data->raw.dht_error = 0;
    data->raw.temperature = 230;
    data->raw.moisture = 343;
    data->raw.air_1p0 = 0;
    data->raw.air_2p5 = 0;
    data->raw.air_10 = 0;
    data->raw.btn_event = 0;
}

void show_int(int val, int line, int col, uint16_t bkg) {
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
    display_outputText24Line(tstr, line, col, 0xffff, bkg); // clear number field
}

void show_int_x10(int val, int line, int col, uint16_t bkg) {
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
    display_outputText24Line(tstr, line, col, 0xffff, bkg); // clear number field
}

void show_watering_mode(int w1, int w2, int line, int col, uint16_t clr, uint16_t bkg) {
    char tstr1[21];
    char tstr[21];
    int sz;
    int szmax = 20 - col;
    sz = snprintf_lib(tstr1, szmax + 1, "%dh:%dm/%d", w1/3600, (w1/60) % 60, w2);
    for (int i = 0; i < szmax - sz; i++) {
         tstr[i] = ' ';
    }
    for (int i = 0; i < sz; i++) {
         tstr[szmax - sz + i] = tstr1[i];
    }
    tstr[szmax] = 0;
    display_outputText24Line(tstr, line, col, clr, bkg); // clear number field
}

void show_time(char h, char m, int line, int col, uint16_t clr, uint16_t bkg) {
    char tstr1[21];
    char tstr[21];
    int sz;
    int szmax = 20 - col;
    sz = snprintf_lib(tstr1, szmax + 1, "%02d:%02d", h, m);
    for (int i = 0; i < szmax - sz; i++) {
         tstr[i] = ' ';
    }
    for (int i = 0; i < sz; i++) {
         tstr[szmax - sz + i] = tstr1[i];
    }
    tstr[szmax] = 0;
    display_outputText24Line(tstr, line, col, clr, bkg); // clear number field
}

// PM2.5 concentration:
// 0 - 12         Good
// 12.1 - 35.4    Moderate
// 35.5 - 55.4    Unhealthy for sensitive people
// 55.5 - 150.4   Unhealthy
// 150.5 - 250.4  Healthy alert
// 250.5 - 500.4  Hazardous
uint16_t air_bkg_color(int val) {
    int v = val;
    if (v <= 12) {
        return CLR_DARK_GREEN;
    } else if (v <= 35) {
        return CLR_DARK_GREEN_YELLOW;
    } else if (v <= 55) {
        return CLR_DARK_YELLOW;
    }
    return CLR_DARK_RED;
}

void udpate_raw_data(raw_meas_type *raw, int sec) {
    // Light measurement
    update_lux();
    while (is_i2c_busy()) {}
    raw->lux = get_lux();
    raw->lux_error = is_i2c_error();

    // temperature on PCB is higher than env. temperature
    bmp280_update_temperature();
    while (is_i2c_busy()) {}
    raw->pressure_error = is_i2c_error();

    bmp280_update_pressure();
    while (is_i2c_busy()) {}
    raw->pressure = get_pressure(raw->temperature);
    raw->pressure_error |= is_i2c_error();

    // water level
    raw->water_level = gpio_pin_get(&CFG_PIN_WATER_LEVEL_DATA);

    // Air quality
    raw->air_1p0 = air_d9_get_pm1p0();
    raw->air_2p5 = air_d9_get_pm2p5();
    raw->air_10 = air_d9_get_pm10();


    // Cannot request dht measurement moreoften than 2 sec. Let it be 4 sec.
    if ((sec & 0x3) == 0) {
        dht_update();
        raw->dht_error =  dht_is_error();
        if (raw->dht_error == 0) {
            raw->temperature = dht_get_temperature();
            raw->moisture = dht_get_moisture();
        }
    }

    // ignore 2 event in a row
    if (is_btn_up_pressed() && (raw->btn_event & BTN_Up) == 0) {
        raw->btn_event |= BTN_Up;
    } else {
        raw->btn_event &= ~BTN_Up;
    }

    if (is_btn_center_pressed() && (raw->btn_event & BTN_Center) == 0) {
        raw->btn_event |= BTN_Center;
    } else {
        raw->btn_event &= ~BTN_Center;
    }

    if (is_btn_down_pressed() && (raw->btn_event & BTN_Down) == 0) {
        raw->btn_event |= BTN_Down;
    } else {
        raw->btn_event &= ~BTN_Down;
    }
}

void set_state(task_data_type *data, estate_type state_next) {
    data->state = state_next;
    data->state_changed_sec = data->sec;
}

int is_time_to_watering(task_data_type *data) {
    int ret = 0;

    data->watering_cnt++;
    if (data->watering_cnt >= data->watering_wait) {
        ret = 1;
        // Finish watering even there's no water detector
        if (!data->raw.water_level) {
            data->water_low = 1;
        }
    } else {
        data->water_low = 0;
        // stop updating counter if there's no water and timeout is less than 10 sec
        if (!data->raw.water_level
           && data->watering_cnt > (data->watering_wait - 10)) {
            data->watering_cnt = data->watering_wait - 10;
        }
    }
    if (data->watering_cnt > (data->watering_wait + data->watering_duration)) {
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

int status_setting_timeout(task_data_type *data) {
    return (data->sec - data->status_changed_sec) >= 7 ? 1: 0;
}

void draw_status_line(raw_meas_type *raw,       // current data
                      task_data_type *data) {  // data on previous epoch
    int t1;
    int status = data->status;
    if (raw->btn_event && (status == STATUS_NO_WATER
        || status == STATUS_WATERING_IN
        || status == STATUS_STOP_IN)) {
        if (raw->btn_event & BTN_Up) {
            status = STATUS_MODE_SELECT;
            data->status_changed_sec = data->sec;
        } else if (raw->btn_event & BTN_Center) {
            status = STATUS_TIME_H_SET;
            data->status_changed_sec = data->sec;
        } else if (raw->btn_event & BTN_Down) {
            status = STATUS_LIGHT_SELECT;
            data->status_changed_sec = data->sec;
        }
    } else if (status_setting_timeout(data)) {
        if (data->watering_ena) {
            status = STATUS_STOP_IN;
        } else if (!raw->water_level) {
            status = STATUS_NO_WATER;
        } else {
            status = STATUS_WATERING_IN;
        }
    }
    

    switch (status) {
    case STATUS_MODE_SELECT:
        if (status != data->status) {
            display_outputText24Line("Period:", WATERING_INFO_LINE, 0, CLR_WHITE, CLR_VIOLET);
            show_watering_mode(data->watering_wait,
                               data->watering_duration,
                               WATERING_INFO_LINE, 7, CLR_WHITE, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Up) {
            data->status_changed_sec = data->sec;

            t1 = (bkp_get_watering_mode() + 1) % 8;
            data->watering_cnt = 0;
            data->watering_wait = WATERING_CYCLE[t1].wait;
            data->watering_duration = WATERING_CYCLE[t1].duration;
            bkp_set_watering_mode(t1);
            show_watering_mode(data->watering_wait,
                               data->watering_duration,
                               WATERING_INFO_LINE, 7, CLR_WHITE, CLR_VIOLET);
        }
       break;
    case STATUS_TIME_H_SET:
        if (status != data->status) {
            display_outputText24Line("Time:", WATERING_INFO_LINE, 0, CLR_WHITE, CLR_VIOLET);
            show_time(data->hour, data->minute, WATERING_INFO_LINE, 5, CLR_WHITE, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Up) {
            data->status_changed_sec = data->sec;
            data->hour = (data->hour + 1) % 24;
            show_time(data->hour, data->minute, WATERING_INFO_LINE, 5, CLR_WHITE, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Down) {
            data->status_changed_sec = data->sec;
            data->hour--;
            if (data->hour < 0) {
                data->hour = 23;
            }
            show_time(data->hour, data->minute, WATERING_INFO_LINE, 5, CLR_WHITE, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Center) {
            status = STATUS_TIME_M_SET;
            data->status_changed_sec = data->sec;
        }
    break;
    case STATUS_TIME_M_SET:
        if (raw->btn_event & BTN_Up) {
            data->status_changed_sec = data->sec;
            data->minute = (data->minute + 1) % 60;
            show_time(data->hour, data->minute, WATERING_INFO_LINE, 5, CLR_WHITE, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Down) {
            data->status_changed_sec = data->sec;
            data->minute--;
            if (data->minute < 0) {
                data->minute = 59;
            }
            show_time(data->hour, data->minute, WATERING_INFO_LINE, 5, CLR_WHITE, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Center) {
            status = STATUS_TIME_T_CORR;
            data->status_changed_sec = data->sec;
            display_outputText24Line("T,corr:", WATERING_INFO_LINE, 0, CLR_WHITE, CLR_VIOLET);
            show_int_x10(data->t_corr, WATERING_INFO_LINE, 7, CLR_DARK_YELLOW);
        }
    break;
    case STATUS_TIME_T_CORR:
        if (raw->btn_event & BTN_Up) {
            data->status_changed_sec = data->sec;
            data->t_corr++;
            show_int_x10(data->t_corr, WATERING_INFO_LINE, 7, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Down) {
            data->status_changed_sec = data->sec;
            data->t_corr--;
            show_int_x10(data->t_corr, WATERING_INFO_LINE, 7, CLR_VIOLET);
        } else if (raw->btn_event & BTN_Center) {
            // end-of-timeout
            data->status_changed_sec = data->sec - 7;
        }
    break;
    case STATUS_LIGHT_SELECT:
        if (status != data->status) {
            display_outputText24Line("Light:", WATERING_INFO_LINE, 0, CLR_WHITE, CLR_VIOLET);
            show_int(0, WATERING_INFO_LINE, 6, CLR_VIOLET);
        }
    break;
    case STATUS_STOP_IN:
        t1 = data->watering_duration - (data->watering_cnt - data->watering_wait);
        if (data->water_low) {
            display_outputText24Line("Stop in:", WATERING_INFO_LINE, 0, 0xffff, CLR_DARK_YELLOW);
            show_int(t1, WATERING_INFO_LINE, 8, CLR_DARK_YELLOW);
        } else {
            display_outputText24Line("Stop in:", WATERING_INFO_LINE, 0, 0xffff, CLR_BLACK);
            show_int(t1, WATERING_INFO_LINE, 8, CLR_DARK_GREEN);
        }
    break;
    case STATUS_WATERING_IN:
        if (status != data->status) {
            display_outputText24Line("Watering in:", WATERING_INFO_LINE, 0, 0xffff, CLR_BLACK);
        }
        t1 = data->watering_wait - data->watering_cnt;
        show_int(t1, WATERING_INFO_LINE, 12, CLR_BLACK);
    break;
    case STATUS_NO_WATER:
        if (status != data->status) {
            display_outputText24Line("No Water            ", WATERING_INFO_LINE, 0, 0xffff, 0xa2a8);
        }
    break;
    default:
        display_outputText24Line("Error state:         ", WATERING_INFO_LINE, 0, 0xffff, 0xa2a8);
    }

    data->status = status;
}


void task_update(task_data_type *data) {
    raw_meas_type raw;
    raw = data->raw;

    switch (data->state) {
    case State_SplashScreen:
        data->state = State_Wait;
        data->state_next = State_SelfTest;
        data->wait_cnt = 4;
        break;
    case State_Wait:
        if (--data->wait_cnt <= 0) {
            set_state(data, data->state_next);
        }
        break;
    case State_SelfTest:
        display_outputText24Line("T:", TEMPERATURE_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int_x10(data->raw.temperature, TEMPERATURE_INFO_LINE, 2, CLR_BLACK);

        display_outputText24Line("Pressure:", PRESSURE_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int_x10(data->raw.pressure, PRESSURE_INFO_LINE, 9, CLR_BLACK);

        display_outputText24Line("Moisture:", MOISTURE_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int_x10(data->raw.moisture, MOISTURE_INFO_LINE, 9, CLR_BLACK);

        display_outputText24Line("PM1.0 ug/m3:", PM1p0_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int(data->raw.air_1p0, PM1p0_INFO_LINE, 12, air_bkg_color(raw.air_1p0));

        display_outputText24Line("PM2.5 ug/m3:", PM2p5_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int(data->raw.air_2p5, PM2p5_INFO_LINE, 12, air_bkg_color(raw.air_2p5));

        display_outputText24Line("PM10  ug/m3:", PM10_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int(data->raw.air_10, PM10_INFO_LINE, 12, air_bkg_color(raw.air_10));

        display_outputText24Line("Light:", LIGHT_INFO_LINE, 0, 0xffff, CLR_BLACK);
        show_int(data->raw.lux, LIGHT_INFO_LINE, 6, CLR_BLACK);

        display_outputText24Line("No Water            ", WATERING_INFO_LINE, 0, 0xffff, 0xa2a8);

        veml7700_configure();
        bmp280_configure();
        set_state(data, State_Idle);
        break;
    case State_Idle:
        udpate_raw_data(&raw, data->sec);

        // pump control
        data->watering_ena = is_time_to_watering(data);
        if (data->watering_ena) {
            pump_enable(data);
        } else {
            pump_disable(data);
        }

        draw_status_line(&raw, data);
    
        if (raw.temperature != data->raw.temperature) {
            show_int_x10(raw.temperature, TEMPERATURE_INFO_LINE, 2, CLR_BLACK);
        } else if (raw.dht_error) {
            show_int_x10(raw.temperature, TEMPERATURE_INFO_LINE, 2, CLR_DARK_RED);
        }

        if (raw.pressure != data->raw.pressure) {
            show_int_x10(raw.pressure, PRESSURE_INFO_LINE, 9, CLR_BLACK);
        } else if (raw.pressure_error) {
            show_int_x10(raw.pressure, PRESSURE_INFO_LINE, 9, CLR_DARK_RED);
        }

        if (raw.moisture != data->raw.moisture) {
            show_int_x10(raw.moisture, MOISTURE_INFO_LINE, 9, CLR_BLACK);
        } else if (raw.dht_error) {
            show_int_x10(raw.moisture, MOISTURE_INFO_LINE, 9, CLR_DARK_RED);
        }
        // PM2.5 concentration:
        // 0 - 12         Good
        // 12.1 - 35.4    Moderate
        // 35.5 - 55.4    Unhealthy for sensitive people
        // 55.5 - 150.4   Unhealthy
        // 150.5 - 250.4  Healthy alert
        // 250.5 - 500.4  Hazardous
        if (raw.air_1p0 != data->raw.air_1p0) {
            show_int(raw.air_1p0, PM1p0_INFO_LINE, 12, air_bkg_color(raw.air_1p0));
        }
        if (raw.air_2p5 != data->raw.air_2p5) {
            show_int(raw.air_2p5, PM2p5_INFO_LINE, 12, air_bkg_color(raw.air_2p5));
        }
        if (raw.air_10 != data->raw.air_10) {
            show_int(raw.air_10, PM10_INFO_LINE, 12, air_bkg_color(raw.air_10));
        }
        if (is_i2c_error()) {
            show_int(get_i2c_err_state(), LIGHT_INFO_LINE, 6, CLR_DARK_RED);
        } else if (raw.lux != data->raw.lux) {
            show_int(raw.lux, LIGHT_INFO_LINE, 6, CLR_BLACK);
        }
   
        data->raw = raw;
        break;
    default:;
    }
}

