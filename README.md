# About

StdPeriphLib_CH32F1 is a SPL style library for CH32F1 series MCU.

## CH32F1 Series MCU

* Products of [WCH沁恒](http://www.wch.cn/), product page http://www.wch-ic.com/products/categories/66.html?pid=5
* Clones of STM32F103 series of STMicroelectronics
* This series contains 4 types
  * CH32F103C6T6
  * CH32F103C8U6
  * CH32F103C8T6
  * CH32F103R8T6

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


# License

* StdPeriphLib_CH32F1 is modified basing on [CH32F103EVT_ZIP](http://www.wch.cn/downloads/CH32F103EVT_ZIP.html) from WCH official site. 
* The original code contains no proper license
* StdPeriphLib_CH32F1 makes NO claim of copyright on the original code.
* All new commits are licensed under the Apache-2.0 license.
