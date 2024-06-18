# STM32 Gardemarin

The STM32 Gardemarin kit empowers users to craft their own systems with multiple motors and sensors,
all controlled by a single board featuring the ARM Cortex M4F processor.

Please find the brief presentation of the development board [here](docs/Gardemarin_Board.pdf).

![Top Side](docs/pics/topside_preview.png)

## Key Features
 
* __Versatile Motor and Sensor Control:__ Capable of handling up to 8 motors,
4 load cells, 2 temperature sensors, and 4 LED strips simultaneously.
* __Connectivity Options:__ Equipped with CAN and Ethernet interfaces, facilitating seamless 
communication with external devices.
* __Expandability:__ Additional options are available; refer to the presentation for details.

## Development Kit Components

The complete setup comprises:

1. Gardemarin (ARM M4F) board
2. Vision Five 2 (RISC-V) Single Board Computer (SBC)
3. Cyclone FX Debugger

![Top Side](docs/pics/dev_setup.png)

The Vision Five 2 board complements the Gardemarin by providing touchscreen control
functions and interfacing primarily via the CAN bus.

## Examples

* Common code snippets reusable across applications.
* Non-modified FreeRTOS source code for running the system.
* Boot application initializes the system and prints 'Hello World' via UART
* Application 'app_os_simlpe' implements the basic aeroponic system firmware
  running FreeRTOS scheduler and basic peripheries (motors, sensors, pumps etc).
* QtMonitor host application running on RISC-V Debian computer provides addtional
  funcitonality, like:

  - Camera to capture video stream or photo.
  - Logging and statistic server
  - Doxygen generated documentation

## Assembled Autonomous Aeroponic System

![Top Side](docs/pics/assembled_aeroponic.png)

## Monitor application

The qtmonitor application provides a user-friendly interface for interaction with the Gardemrin
board using a GUI and touchscreen. This application is built on Qt6 libraries and is so compatible
with the both Windows and Linux operating systems.

![Top Side](docs/pics/sensors_1.gif)
*qtmonitor sensors data: moisture and weights values while watering, NPK, EC, pH and temperatures*

The GIF image above ilustrates how sensor data changes during the watering cycle. In this example,
tap water without fertilizers is used. Watering cycle (~20 minutes) is split on small sub-cycles:
20 seconds watering 1 minute pause. The scales measurements perfectly show this process.
Oxygen saturation process is implemented at the beginning of each cycle in the water tank with the mixture.
Oxygen saturation process increases pH value upto ~7.0 without mixing with additional components.

Use cmake-gui or the following command to build _qtmonitor_ application:

<b>Linux:</b>

    > cd qtmonitor
    > cmake -S . -B ./build
    > cd build
    > make
    > ./bin/qtmonitor --cfg ../gardemarin.cfg


## CAN database file (DBC)

For applications involving CAN access, each example in the repository is accompanied by
a DBC converter and a specialized software layer facilitating interaction (read/write)
with object attributes. During startup, a list of all registered objects and attributes is
provided. This DBC-formatted information can be saved into a _*.dbc_ file for use with third-party
CAN software.

