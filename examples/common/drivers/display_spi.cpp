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
#include <fwapi.h>
#include <new>
#include <string.h>
#include <uart.h>
#include <spi.h>
#include "display_spi.h"

#define USE_TIM5



// PD[7] = DISPLAY_RES AF0 output
// PC[12] = SPI3_MOSI  AF6 -> AF0 output
// PC[11] = SPI3_MISO  AF0      AF0 output: command/data control
// PC[10] = SPI3_SCK   AF6 -> AF0 output
static const gpio_pin_type DISPLAY_RES = {(GPIO_registers_type *)GPIOD_BASE, 7};  // 88 PD5_SPI_CS3 (error in name PD7_..)
static const gpio_pin_type DISPLAY_SCK = {(GPIO_registers_type *)GPIOC_BASE, 10}; // 78 SPI3_SCK
static const gpio_pin_type DISPLAY_SDA = {(GPIO_registers_type *)GPIOC_BASE, 12}; // 80 SPI3_MOSI
static const gpio_pin_type DISPLAY_DC = {(GPIO_registers_type *)GPIOC_BASE, 11};  // 79 SPI3_MISO. Data/command

IrqHandlerInterface *drv_display_spi_tx_ = 0;


DisplaySPI::DisplaySPI(const char *name) : FwObject(name) {
    RCC_registers_type *RCC = (RCC_registers_type *)RCC_BASE;
    TIM_registers_type *TIM5 = (TIM_registers_type *)TIM5_BASE;
    SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;

    estate_ = Idle;
    bitCnt_ = 0;
    drv_display_spi_tx_ = static_cast<IrqHandlerInterface *>(this);


    // Setup SPI interface. Use bitband instead of SPI3 controller because 
    // non-standard 25-bits transactions:
    //   PD[7]  = DISPLAY_RES/PD7_SPI_CS3   AF6 -> AF0 output
    //   PC[12] = DISPLAY_SDA/SPI3_MOSI     AF6 -> AF0 output
    //   PC[11] = DISPLAY_DC/SPI3_MISO      AF6 -> AF0 output: command/data control (disable SPI Rx if SPI controller is used)
    //   PC[10] = DISPLAY_SCK/SPI3_SCK      AF6 -> AF0 output
    gpio_pin_as_output(&DISPLAY_RES,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_set(&DISPLAY_RES);   // reset: active LOW

    // SCK AF6 = SPI3
    gpio_pin_as_alternate(&DISPLAY_SCK, 6);

    // Data/command:
    gpio_pin_as_output(&DISPLAY_DC,
                       GPIO_NO_OPEN_DRAIN,
                       GPIO_MEDIUM,
                       GPIO_NO_PUSH_PULL);
    gpio_pin_clear(&DISPLAY_DC);

    // SDA AF6 = SPI3
    gpio_pin_as_alternate(&DISPLAY_SDA, 6);

    // adc clock on APB1 = 144/4 = 36 MHz
    uint32_t t1 = read32(&RCC->APB1ENR);
    t1 |= (1 << 15);            // APB1[15] SPI3EN
    t1 |= (1 << 3);             // APB1[3] TIM5EN
    write32(&RCC->APB1ENR, t1);

    // SPI3 master, transmit-only
    uint16_t t2;
    t2 = (0 << 7)    // [7] TXEIE: Tx buffer empty IRQ
       | (0 << 6)    // [6] RXNEIE: Rx buffer not empty IRQ
       | (0 << 5)    // [5] ERRIE: Error IRQ
       | (0 << 4)    // [4] FRF: Frame format: 0=SPI motorola; 1=SPI TI
       | (0 << 2)    // [2] SSOE: SS output enable: 0=disabled (can work multimaster mode)
       | (0 << 1)    // [1] TXDMAEN: Tx buffer DMA ena
       | (0 << 0);   // [0] RXDMAEN: Rx buffer DMA ena
    write16(&SPI3->CR2, t2);

    t2 = (0 << 15)    // [15] BIDIMODE: 0=2-line unidirectional, 1=1-linne bidirectional
       | (1 << 14)    // [14] BIDIOE: output enable in bidir mode
       | (0 << 13)    // [13] CRCEN: hardware CRC calc ena
       | (0 << 12)    // [12] CRCNEXT: CRC transfer next
       | (0 << 11)    // [11] DFF: Data frame format: 0=8bits, 1=16bits
       | (0 << 10)    // [10] RXONLY: Receive only
       | (1 << 9)     // [9] SSM: Software Slave Management. When 1(enabled) the NSS pin input is replaced with the value from SSI bit
       | (1 << 8)     // [8] SSI: Internal Slave select
       | (0 << 7)     // [7] LSBFIRST: 0=MSB tranmit first, 1=LSB
       | (0 << 6)     // [6] SPE: SPI enable
       | (4 << 3)     // [5:3] BR[2:0]: Baud rate control: 0-Fpclk/2, 1=Fpclk/4, 2=Fpclk/8, ...,4=Fpclk/32, ..,  7=Fpclk/256
       | (1 << 2)     // [2] MSTR: Master selection
       | (1 << 1)     // [1] CPOL: Clock polarity: 0=CK to 0 when idle, 1=when idle
       | (1 << 0);    // [0] CPHA: Clock phase: 0=the first clock transition is the first data capture edge
    write16(&SPI3->CR1, t2);

    t2 |= (1 << 6);     // [6] SPE: SPI enable
    write16(&SPI3->CR1, t2);

    // TIM5 (unused for now)
    write32(&TIM5->CR1.val, 0);         // stop counter
    // time scale 100 nsec
    write16(&TIM5->PSC, system_clock_hz() / 2 / 10000000 - 1);             // prescaler = 1: CK_CNT = (F_ck_psc/(PSC+1))
    write16(&TIM5->DIER, 1);            // [0] UIE - update interrupt enabled

    // prio: 0 highest; 7 is lowest
    //nvic_irq_enable(50, 5); // 50 TIM5
    //nvic_irq_enable(51, 6); // 51 SPI3
}

void DisplaySPI::Init() {
    RegisterInterface(static_cast<IrqHandlerInterface *>(this));
    RegisterInterface(static_cast<TimerListenerInterface *>(this));
}

void init_systick() {
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;
    uint32_t t1;

    t1 = system_clock_hz() / 100;     // 100 Hz, SysTick is 24-bits width
    write32(&systick->RVR, t1);       // reload value
    write32(&systick->CVR, t1);        // current value

    t1 = (1 << 2)      // CLKSOURCE: SysTick uses CPU clock (default 1)
       | (1 << 1)       // TICKINT: enable interrupt
       | 1;            // ENABLE:
    write32(&systick->CSR, t1);
}

void deinit_systick() {
    SysTick_registers_type *systick = (SysTick_registers_type *)SysTick_BASE;
    write32(&systick->CSR, read32(&systick->CSR) & ~0x1);
}

void DisplaySPI::PostInit() {
    SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;
    uint16_t t1;

    init_systick();

    uart_printk("LCD: resetting...\r\n");
    gpio_pin_clear(&DISPLAY_RES);   // reset: active LOW
    system_delay_ns(25000);
    gpio_pin_set(&DISPLAY_RES);   // reset: active LOW
    uart_printk("LCD: reset done\r\n");

    // Sleep out command (DC=0)
    write_cmd_poll(0x11);           // 0x11 sleep-out command
    system_delay_ns(200000);
    uart_printk("LCD: sleep out done\r\n");

    // Idle mode off command (DC=0)
    write_cmd_poll(0x38);           // 0x38 idle mode off command

    // Normal display mode on command (DC=0)
    write_cmd_poll(0x13);           // 0x13 Normal display mode on

    // diplay inversion on
    write_cmd_poll(0x21);           // 0x21 display inversion on

    // write gamma set
    write_cmd_poll(0x26);           // 0x26 gamma set command
    write_data_poll(0x1);           // 0x1 = g2.2 gamma curve

    // 0x36 memory data access control command
    t1 = 0 << 7             // [7] 0=top to bottom; 1=bottom to top
       | 0 << 6             // [6] 0=left to right; 1=right to left
       | 0 << 5             // [5] 0=column normal mode; 1=reverse mode
       | 0 << 4             // [4] 0=lcd refresh top to bottom; 1=bottom to top
       | 0 << 3             // [3] 0=rgb; 1=bgr
       | 0 << 2;            // [2] 0=lcd refresh left to right; 1=right to left
    write_cmd_poll(0x36);
    write_data_poll(t1);

    // 0x3A interface pixel format command
    t1 = 6 << 4             // [7:4] RGB format: 5=65k, 6=262k
       | 5 << 0;            // [3:0] 3=12bit/pixel; 5=16bit/pixel (rgb565); 6=18bit/pixel
    write_cmd_poll(0x3A);
    write_data_poll(t1);

    // 0x51 write display brightness command
    write_cmd_poll(0x51);
    write_data_poll(0xff);

    // 0x53 write CTRL display command
    t1 = 0 << 5             // [5] brightness block
       | 0 << 3             // [3] display dimming
       | 0 << 2;            // [2] backlight
    write_cmd_poll(0x53);
    write_data_poll(t1);

    // 0x55 write content adaptive brightness control and color enhancement command
    t1 = 1 << 7             // [7] color enhancment
       | 3 << 4             // [5:4] level: 0=low enhancement; 1=medium enhancement; 3=high enhancement
       | 1 << 0;            // [1:0] mode: 0=off; 1=user interface mode; 2=still picture; 3=moving image
    write_cmd_poll(0x55);
    write_data_poll(t1);

    // 0x5E write CABC minimum brightness command
    write_cmd_poll(0x5E);
    write_data_poll(0);

    // 0xB0 ram control command
    write_cmd_poll(0xB0);
    t1 = 0 << 4             // [7:4] ram mode: 0=mcu interface; 1=rgb interface
       | 0 << 0;            // [3:0] display mode: 0=mcu interface; 1=rgb iterface; 2=vsync itnerface
    write_data_poll(t1);
    t1 = 0 << 4             // [4] frame type: 0=type 0; 1=type1, 2=type2, 3=type3
       | 0 << 3             // [3] data mode: 0=MSB (big endian); 1=LSB (little endian)
       | 0 << 2             // [2] bus width: 0=18 bits; 1=6 bits
       | 0 << 0;            // [1:0] pixel type:0=type0
    write_data_poll(t1);

    // 0xB1 rgb control command
    write_cmd_poll(0xB1);
    t1 = (0 << 7)           // [7] 0=memory mode; 1=shift reg mode
       | (0 << 5)           // [6:5] 0=memory mode; 1=rgb de; 2=rgb hv
       | (0 << 3)           // [3] vspl: pin low
       | (0 << 2)           // [2] hspl: pin low
       | (0 << 1)           // [1] dpl: pin low
       | (0 << 0);          // [0] epl: pin low
    write_data_poll(t1);
    write_data_poll(0x02);  // [6:0] vbp
    write_data_poll(0x14);  // [4:0] hbp

    // 0xB2 porch control command
    write_cmd_poll(0xB2);
    write_data_poll(0x0C);  // back proch
    write_data_poll(0x0C);  // front proch
    write_data_poll(0);     // porch enable
    t1 = 0x3 << 4           // [7:0] idel back proch
       | 0x3 << 0;          // [3:0] idel front proch
    write_data_poll(t1);
    t1 = 0x3 << 4           // [7:0] partial back proch
       | 0x3 << 0;          // [3:0] partial front proch
    write_data_poll(t1);

    // 0xB3 frame rate control 1 command
    write_cmd_poll(0xB3);
    t1 = (0 << 4)           // [4] separate frame
       | (0 << 0);          // [1:0]  Divided: 0=1; 1=2; 2=4; 3=8
    write_data_poll(t1);
    t1 = (0 << 5)           // [7:5] Inversion idle mode: 0=dot; 7=column inversion
       | (0x0F << 0);       // [4:0] Idle frame rate
    write_data_poll(t1);
    t1 = (0 << 5)           // [7:5] Inversion partial mode: 0=dot; 7=column inversion
       | (0x0F << 0);       // [4:0] Partial frame rate
    write_data_poll(t1);

    // 0xB5 partial mode control command
    write_cmd_poll(0xB5);
    t1 = (0 << 7)           // [7] Non display source output level: 0=V63; 1=0
       | (0 << 4)           // [4] 0=Normal mode; 1=invert scan mode
       | (0 << 0);          // [3:0] 0=every frame; 1=1/3 frame; 2=1/5 frame ...
    write_data_poll(t1);

    // 0xB7 gate control command
    write_cmd_poll(0xB7);
    t1 = (7 << 4)           // [6:4] vghs: 7=14.97V
       | (2 << 0);          // [2:0] vgls: 2=-8.23V
    write_data_poll(t1);

    // 0xB8 gate on timing adjustment command
    write_cmd_poll(0xB8);
    write_data_poll(0x2A);
    write_data_poll(0x2B);
    write_data_poll(0x22);  // [3:0] gate on timing adjustment
    t1 = (0x07 << 4)        // [7:4] gate off timing adjustment rgb
       | (0x5 << 0);        // [3:0] gate off timing adjustment
    write_data_poll(t1);

    // 0xBA digital gamma enable command
    write_cmd_poll(0xBA);
    write_data_poll(1 << 2);// [2] enable

    // 0xBB vcoms setting command
    t1 = (uint16_t)((1.625f - 0.1f) / 0.025f);        // VCOM=1.625f to integer
    write_cmd_poll(0xBB);
    write_data_poll(t1);

    // 0xC0 lcm control command
    write_cmd_poll(0xC0);
    t1 = (0 << 6)           // [6] xmy
       | (1 << 5)           // [5] xbgr
       | (0 << 4)           // [4] xinv
       | (1 << 3)           // [3] xmx
       | (1 << 2)           // [2] xmh
       | (0 << 1)           // [1] xmv
       | (0 << 0);          // [0] xgs
    write_data_poll(t1);

    // 0xC2 vdv and vrh command enable command
    write_cmd_poll(0xC2);
    write_data_poll(1);     // 1=from cmd; 0=from nvm
    write_data_poll(0xFF);

    // 0xC3 vrh set command
    t1 = (uint16_t)((4.8f - 3.55f) / 0.05f);        // convert VRHS=4.8f
    write_cmd_poll(0xC3);
    write_data_poll(t1);

    // 0xC4 vdv setting command
    t1 = (uint16_t)((0.0f + 0.8f) / 0.025f);           // convert VDV=0
    write_cmd_poll(0xC4);
    write_data_poll(t1);

    // 0xC5 vcoms offset set command
    write_cmd_poll(0xC5);
    t1 = (uint16_t)((0.0f + 0.8f) / 0.025f);            // VDV offset =0
    write_data_poll(t1);

    // 0xC6 fr control 2 command
    write_cmd_poll(0xC6);
    t1 = (0 << 5)           // [7:5] 0=dot inversion; 7=column inverion
       | (0x0F << 0);       // [4:0] frame rate: 0x0F=60Hz
    write_data_poll(t1);

    // 0xC7 cabc control command
    write_cmd_poll(0xC7);
    t1 = (0 << 3)           // [3] led_on
       | (0 << 2)           // [2] led_pwm_init
       | (0 << 1)           // [1] led_pwm_fix
       | (0 << 0);          // [0] led_pwm_polarity
    write_data_poll(t1);

    // 0xCC pwm frequency selection command
    write_cmd_poll(0xCC);
    t1 = (0x0 << 3) | (0x2 << 0);   // 9.8 KHz
    write_data_poll(t1);

    // 0xD0 power control 1 command
    write_cmd_poll(0xD0);
    write_data_poll(0xA4);
    t1 = (2 << 6)           // [7:6] avdd: 0=6.4V; 1=6.6V; 2=6.8V
       | (2 << 4)           // [5:4] avcl: 0=-4.4V; 1=-4.6V; 2=-4.8V; 3=-5.0V
       | (1 << 0);          // [1:0] vds: 0=2.19V; 1=2.3V; 2=2.4V; 3=2.51V
    write_data_poll(t1);

    // 0xDF command 2 enable command
    write_cmd_poll(0xDF);
    write_data_poll(0x5A);
    write_data_poll(0x69);
    write_data_poll(0x02);
    write_data_poll(0);     // enable

    // 0xE0 positive voltage gamma control command
    write_cmd_poll(0xE0);
    uint16_t positive_voltage_gamma[14] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F,
                                           0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
    for (int i = 0; i < 14; i++) {
        write_data_poll(positive_voltage_gamma[i]);
    }

    // 0xE1 negative voltage gamma control command
    write_cmd_poll(0xE1);
    uint16_t negative_voltage_gamma[14] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F,
                                           0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
    for (int i = 0; i < 14; i++) {
        write_data_poll(negative_voltage_gamma[i]);
    }

    // 0xE2 digital gamma look-up table for red command
    write_cmd_poll(0xE2);
    for (int i = 0; i < 64; i++) {
        write_data_poll(4*i);
    }

    // 0xE3 digital gamma look-up table for blue command
    write_cmd_poll(0xE3);
    for (int i = 0; i < 64; i++) {
        write_data_poll(4*i);
    }

    // 0xE4 gate control command
    write_cmd_poll(0xE4);
    t1 = (uint16_t)((320 / 8) - 1);         // gate line = 320
    write_data_poll(t1);
    write_data_poll(0);                     // first_scan_line_number
    t1 = 0x10 
       | (0 << 2)           // [2] 0=interlace mode; 1=non-interlace mode
       | (0 << 0);          // [0] direction 0=0..319; 1=319..0
    write_data_poll(t1);

    // 0xE7 spi2 command
    write_cmd_poll(0xE7);
    t1 = (0 << 4)           // [4] SPI2 lane
       | (0 << 0);          // [0] command_table_2
    write_data_poll(t1);

    // 0xE8 power control 2 command
    write_cmd_poll(0xE8);
    t1 = (1 << 4)           // [5:4] sbdiv: 0=div2; 1=div3; 2=div4; 3=div6
       | (3 << 0);          // [1:0] stp15ck_div: 0=div2; 1=div3; 2=div4; 3=div6
    write_data_poll(t1);

    // 0xE9 equalize time control command
    write_cmd_poll(0xE9);
    write_data_poll(0x11);  // source equlize time
    write_data_poll(0x11);  // source pre drive time
    write_data_poll(0x08);  // gate equalize time

    // 0xFA program mode enable command
    write_cmd_poll(0xFA);
    write_data_poll(0x5A);
    write_data_poll(0x69);
    write_data_poll(0xEE);
    write_data_poll(0 << 2);    // [2] enable

    // 0x29 display on command
    write_cmd_poll(0x29);


    // Clear screen
    write_cmd_poll(0x2A);                       // column address set command
    write_data_poll((0x00 >> 8) & 0xFF);        // start address msb
    write_data_poll((0x00 >> 0) & 0xFF);        // start address lsb
    write_data_poll(((COLS - 1) >> 8) & 0xFF);   // end address msb
    write_data_poll(((COLS - 1) >> 0) & 0xFF);   // end address lsb

    write_cmd_poll(0x2B);                       // row address set command
    write_data_poll((0x00 >> 8) & 0xFF);        // start address msb
    write_data_poll((0x00 >> 0) & 0xFF);        // start address lsb
    write_data_poll(((ROWS - 1) >> 8) & 0xFF);   // end address msb
    write_data_poll(((ROWS - 1) >> 0) & 0xFF);   // end address lsb

    write_cmd_poll(0x2C);                       // memory write command

    memset(buffer_, 0x20, sizeof(buffer_));

    size_t m = static_cast<size_t>(ROWS) * COLS * 2 / sizeof(buffer_);     // total times
    size_t n = static_cast<size_t>(ROWS) * COLS * 2 % sizeof(buffer_);     // the last
    for (size_t i = 0; i < m; i++) {
        for (size_t ii = 0; ii < sizeof(buffer_); ii++) {
            write_data_poll(buffer_[ii]);
        }
    }
    if (n != 0) {
        for (size_t ii = 0; ii < n; ii++) {
            write_data_poll(buffer_[ii]);
        }
    }
    deinit_systick();
}

void DisplaySPI::handleInterrupt(int *argv) {
}

void DisplaySPI::write_cmd_poll(uint16_t cmd) {
    SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;
    gpio_pin_clear(&DISPLAY_DC);    // 0=CMD
    write16(&SPI3->DR, cmd);
    // SR[1] TXE: transmit buffer empty: 0=not empty; 1=empty
    while ((read16(&SPI3->SR) & 0x2) == 0) {}
}

void DisplaySPI::write_data_poll(uint16_t data) {
    SPI_registers_type *SPI3 = (SPI_registers_type *)SPI3_BASE;
    gpio_pin_set(&DISPLAY_DC);      // 1=DATA
    write16(&SPI3->DR, data);
    // SR[1] TXE: transmit buffer empty: 0=not empty; 1=empty
    while ((read16(&SPI3->SR) & 0x2) == 0) {}
}
