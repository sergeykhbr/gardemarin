# Hydroponic System and Weather station on a single PCB

After more than two years of experiments with different designs and approaches,
I finally arrived at what I consider the most convenient and straightforward solution
for a tabletop hydroponic system with integrated weather monitoring capabilities.

The device combines the following monitoring and measurements systems:


| Sensor / Device | Interface / Protocol | Pin Function & Purpose |
| :--- | :--- | :--- |
| **Air Quality Sensor** (DB9) | USART1 | PA[9],PA[10]: Transmits serial telemetry and data readings (RXD/TXD). |
|                              | GPIO | PA[8]: Toggles pin high/low to enable or disable sensor power. |
|                              | PWM | PA[11]: Controls the speed of the internal intake fan. |
| **DHT22 Humidity**           | Single-Wire | PA[1]: Pulls data over a dedicated bi-directional digital pin. |
| **OLED Display**             | SPI1 | PB[5],PB[3]: Sends screen rendering commands and pixel data (SCK/MOSI). |
|                              | GPIO | PB[4]: Command/Data strob selector. |
|                              | GPIO | PA[15]: Drives the hardware Reset (RST) line. |
| **Pump relais**              | GPIO | PA[12]: Control relais pin. |
| **User Buttons**             | GPIO | PB[0],PB[1],PC[13]: Up, Center and Down buttons. |
| **VEML7700 Light sensor**    | I2C2 | PB[10],PB[11]: 400 kHz I2C interface. Address 0x20. |
| **BMP280 Pressure/Temperature** | I2C2 | PB[10],PB[11]: 400 kHz I2C interface. Address 0xEC. |
| **MOSFET LED**                   | TIM1 | PB[13]: PWM LED driver. |
| **XKC Y25 T12V** Liquide level sensor | GPIO | PB[15]: 5V tolerant buffer. |

The entire device is built around a single custom-designed PCB:

![Top and bottom sides](docs/pics/qleika_top_btm_3d.png)


The board features automatic power source selection, seamlessly switching between
the primary power supply (12/24 V) and the secondary 5 V supply.

It also preserves all configuration settings across power outages and provides a simple,
intuitive user interface consisting of just three buttons and a 240x240 pixel color RGB display.

## Assembeled device based on the PCB:

The following pictures show two different pepper varieties, 'Black Pearl' and 'Aurora',
grown in different locations.

!['Black Pearl' (left) and 'Aurora' (right)](docs/pics/black_pearl_young_aurora_10month.png)

The photo of 'Black Pearl' was taken at an early growth stage, when the plant had just started
producing its first peppers.

The photo of 'Aurora' was taken when the plant was approximately 10 months old and had already
produced an abundance of peppers.

![Inside of device](docs/pics/qleika_inside.png)

## Board description

