# STM32 Gardemarin

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
* Simple boot application initializes the system and prints 'Hello World' via UART
* Application 'app_os_simlpe' implements basic tests of system components (e.g., motors, sensors,
  scales) and launches FreeRTOS scheduler with testing tasks.
* Automated Aeroponics System demonstrates extensive network capability and offers
  additional examples such as:

  - FreeRTOS with the Web server using LwIP TCP/IP stack.
  - CAN over Ethernet 
  - And some other useful examples

## CAN database file (DBC)

For applications involving CAN access, each example in the repository is accompanied by
a DBC converter and a specialized software layer facilitating interaction (read/write)
with object attributes. During startup, a list of all registered objects and attributes is
provided. This DBC-formatted information can be saved into a _*.dbc_ file for use with third-party
CAN software.

Example of application startup output:

```
Registered objects list:
  Attributes of class 'kernel':
    ID[0,0] => Version, uint32, 0x20240804
  Attributes of class 'relais0':
    ID[1,0] => State, int16, 0
  Attributes of class 'relais1':
    ID[2,0] => State, int16, 0
  Attributes of class 'can1':
    ID[3,0] => baudrate, uint32, 0x0007a120
    ID[3,1] => mode, int8, 2
    ID[3,2] => test1, uint32, 0xcafef00d
  Attributes of class 'can2':
    ID[4,0] => baudrate, uint32, 0x0007a120
    ID[4,1] => mode, int8, 2
    ID[4,2] => test1, uint32, 0xcafef00d
  Attributes of class 'dbc':

VERSION ""

NS_ :
        BA_
        BA_DEF
        BA_DEF_DEF_
        BA_DEF_DEF_REL_
        BA_DEF_REL_
        BA_DEF_SGTYPE_
        BA_REL_
        BA_SGTYPE_
        BO_TX_BU_
        BU_BO_REL_
        BU_EV_REL_
        BU_SG_REL_
        CAT_
        CAT_DEF_
        CM_
        ENVVAR_DATA_
        EV_DATA_
        FILTER
        NS_DESC_
        SGTYPE_
        SGTYPE_VAL_
        SG_MUL_VAL_
        SIGTYPE_VALTYPE_
        SIG_GROUP_
        SIG_TYPE_REF_
        SIG_VALTYPE_
        VAL_
        VAL_TABLE_
BS_:

BU_: GARDEMARIN ANY


BO_ 268437366 GARDEMARIN_DEBUG: 8 GARDEMARIN

BO_ 268435712 kernel: 1 GARDEMARIN
 SG_ kernel_Version : 0|32@1+ (1,0) [0|0] "" GARDEMARIN

BO_ 268435712 relais0: 1 GARDEMARIN
 SG_ relais0_State : 0|16@1+ (1,0) [0|0] "" GARDEMARIN

BO_ 268436224 relais1: 1 GARDEMARIN
 SG_ relais1_State : 0|16@1+ (1,0) [0|0] "" GARDEMARIN

BO_ 268436224 can1: 2 GARDEMARIN
 SG_ can1_mux M : 0|8d@1+ (1,0) [0|0] "" GARDEMARIN
 SG_ can1_baudrate m0 : 8|32@1+ (1,0) [0|0] "" GARDEMARIN
 SG_ can1_mode m1 : 8|8@1+ (1,0) [0|0] "" GARDEMARIN
 SG_ can1_test1 m2 : 8|32@1+ (1,0) [0|0] "" GARDEMARIN

BO_ 268436736 can2: 2 GARDEMARIN
 SG_ can2_mux M : 0|8d@1+ (1,0) [0|0] "" GARDEMARIN
 SG_ can2_baudrate m0 : 8|32@1+ (1,0) [0|0] "" GARDEMARIN
 SG_ can2_mode m1 : 8|8@1+ (1,0) [0|0] "" GARDEMARIN
 SG_ can2_test1 m2 : 8|32@1+ (1,0) [0|0] "" GARDEMARIN

BO_ 268436736 dbc: 0 GARDEMARIN


CM_ BU_ GARDEMARIN "Prototype of the aeroponic system base on ST32F407* CPU";
CM_ BU_ ANY "Any target like host PC or Hardware";
CM_ BO_ 268437366 "Debug output strings";


Starting FreeRTOS scheduler!
```
