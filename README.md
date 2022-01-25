# About

StdPeriphLib_CH32F1 is a SPL style library for CH32F1 series MCU
* For arm-none-eabi-gcc toolchains
* Compatible with VSCode+PlatformIO

## CH32F103 Series MCU

* Products of [WCH沁恒](http://www.wch.cn/), 
* Product page http://www.wch-ic.com/products/categories/66.html?pid=5
* Clones of STMicroelectronics STM32F103 series
* Featured with DAC and USB support

## Technical Specifications

* Cortex-M3 core, 72MHz system main frequency;
* Single cycle multiplication and hardware division;
* 20KB SRAM，64KB CodeFlash (CH32F103C6T6 is 10KB/32KB)
* **Power supply: 2.7V-5.5V**, GPIO follows power supply voltage;
* Low power modes: sleep/stop/standby;
* On/off reset (por/PDR);
* Programmable voltage monitor (PVD);
* 7-channel DMA controller;
* 12-bit ADC, 1us conversion time;
* 16-channel TouchKey detection;
* **12-bit DAC**;
* 7 timers;
* **1 USB2.0 FullSpeed host/device interface with full speed and low speed**;
* **1 USB2.0 FullSpeed device interface with full speed and low speed**;
* 1 CAN controller (2.0B active);
* 2 I2C interfaces (SMBus and PMBus);
* 3 USART interfaces;
* 2 SPI interfaces (Master and Slave modes);
* 1 I/O port, all IO ports can be mapped to 16 external interrupts;
* CRC calculation unit, 96-bit chip unique ID;
* Serial single-wire debugging (SWD) interface;
* Package: LQFP64M, LQFP48, QFN48.

## Types

| Part NO      | Freq  | Flash | SRAM | GPIO | Adv TIM(16bit) | GPTIM(16bit) | GPTIM(32bit) | Basic TIM(16bit) | SysTick(24bit) | WDOG | RTC |  ADC(12bit) Unit/Channel | Touchkey |  DAC(12bit) | OPA | TRNG | SPI | IIS | IIC | U(S)ART | CAN | USB2.0 FS | USB2.0 HS | Ethernet | BLE | SDIO | FSMC | DVP | VDD     | Package  |
|  ----------- | ----- | ----- |----- |----- |     ---------- |   ---------- |   ---------- |       ---------- |          ----- |----- | --- |               ---------- |    ----- |       ----- | --- |----- | --- | --- | --- |   ----- | --- |     ----- |     ----- |    ----- | --- |----- |----- | --- |   ----- |   ------ |    
| CH32F103C6T6 | 72MHz | 32K   | 10K  | 37   | 1              | 2            | -            | -                | 1              | 2    | 1   |  1/10                    | 10       |  1          | -   | -    | 1   | -   | 1   | 2       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | LQFP48   |     
| CH32F103C8U6 | 72MHz | 64K   | 20K  | 37   | 1              | 3            | -            | -                | 1              | 2    | 1   |  1/10                    | 10       |  1          | -   | -    | 2   | -   | 2   | 3       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | QFN48    |
| CH32F103C8T6 | 72MHz | 64K   | 20K  | 37   | 1              | 3            | -            | -                | 1              | 2    | 1   |  1/10                    | 10       |  1          | -   | -    | 2   | -   | 2   | 3       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | LQFP48   |
| CH32F103R8T6 | 72MHz | 64K   | 20K  | 51   | 1              | 3            | -            | -                | 1              | 2    | 1   |  1/16                    | 16       |  1          | -   | -    | 2   | -   | 2   | 3       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | LQFP64M  |

# File Structure

```
├── Debug                # For demo purpose, optional
├── Examples             # Example code for all peripherals
├── StdPeriphDriver      # SPL style library for CH32F1, required
│   ├── inc
│   └── src
├── System               # System configurations, required
├── USB-Config           # USB configurations, optional
└── USB-Driver           # USB library, optional
    ├── inc
    └── src
```

By comparing StdPeriphDriver with STMicroelectronics STM32F103 SPL library, the corresponding parts are almost identical, 
same variable names, same function names, even the code formats are the same. Which means if you don't use the additional
peripherals like DAC and USB, you can pretty much rely on the STM32F103 SPL library.

# Quick Start

This library is adjusted for PlatformIO so the most convenient way is importing it as a PlatformIO library.

## Prerequisites

* VSCode with PlatformIO extension installed
* In PlatformIO, install platform "ST STM32"
* ST-Link module or USB dongle for hex download

## Create STM32 project

1. In PlatformIO Home, Click "New Project"
1. Project Name: input project name
1. Board: Select STM32F103C8T6 Generic or Bluepill F103C8 Generic
1. Framework: CMSIS
1. Location: uncheck and specify the project location
1. Click "Finish"

This will create an empty project in the location you specified.

## Import StdPeriphLib_CH32F1

Clone this repository to local file system and move it to project lib folder, the project file structure should look like this
```
├── include
├── lib
│   ├── Debug
│   ├── Examples
│   ├── StdPeriphDriver
│   ├── System
│   ├── USB-Config
│   └── USB-Driver
├── src
└── test
```

## Run GPIO Example

1. Copy all the files except ch32f10x_conf.h from lib/Examples/GPIO/GPIO_Toggle to folder src
1. There is a ch32f10x_conf.h in lib/System/ alreay, and this demo doesn't need to change it.
1. Build the project

## Download To Board

Edit platformio.ini, add one line otherwise it won't recognize CH32F103 chips
```
upload_flags = -c set CPUTAPID 0x2ba01477
```
Then the configurations should look like
```
[env:bluepill_f103c8_ch32]
platform = ststm32
board = bluepill_f103c8
framework = cmsis
upload_flags = -c set CPUTAPID 0x2ba01477
```
Connect development board to PC through ST-Link, then start download


# License

* StdPeriphLib_CH32F1 is modified basing on [CH32F103EVT_ZIP](http://www.wch.cn/downloads/CH32F103EVT_ZIP.html) from WCH official site. 
* The original code contains no proper license
* StdPeriphLib_CH32F1 makes NO claim of copyright on the original code.
* All new commits are licensed under the Apache-2.0 license.
