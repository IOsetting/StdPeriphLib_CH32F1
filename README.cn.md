# 关于

StdPeriphLib_CH32F1 是用于 CH32F1 系列MCU的 SPL 风格封装库
* 使用 arm-none-eabi-gcc 工具链
* 兼容 Linux 下的 VSCode+PlatformIO 开发环境

## CH32F103 系列 MCU

* 厂商 [WCH沁恒](http://www.wch.cn/), 
* 产品页 http://www.wch-ic.com/products/categories/66.html?pid=5
* STMicroelectronics STM32F103 系列的克隆/替代型
* 亮点: DAC 和 USB 外设

## 技术参数

* Cortex-M3内核, 72MHz系统主频;
* 单周期乘法和硬件除法;
* 20KB SRAM, 64KB CodeFlash;
* 供电范围: 2.7V-5.5V, GPIO同步供电电压;
* 多种低功耗模式: 睡眠/停止/待机;
* 上电/断电复位(POR/PDR);
* 可编程电压监测器(PVD);
* 7通道DMA控制器;
* 12位模数转换ADC, 1us转换时间;
* 16路TouchKey通道检测;
* 12位数模转换DAC;
* 7个定时器;
* 1个USB2.0 FullSpeed主机/设备接口(全速和低速);
* 1个USB2.0 FullSpeed设备接口(全速和低速);
* 1个CAN控制器(2.0B主动);
* 2个I2C接口(支持SMBus/PMBus);
* 3个USART接口;
* 2个SPI接口(支持Master和Slave模式);
* 51个I/O口, 所有IO口都可以映射到16个外部中断;
* CRC计算单元, 96位芯片唯一ID;
* 串行单线调试(SWD)接口;
* 封装形式：LQFP64M、LQFP48、QFN48

## 型号明细

| Part NO      | Freq  | Flash | SRAM | GPIO | Adv TIM(16bit) | GPTIM(16bit) | GPTIM(32bit) | Basic TIM(16bit) | SysTick(24bit) | WDOG | RTC |  ADC(12bit) Unit/Channel | Touchkey |  DAC(12bit) | OPA | TRNG | SPI | IIS | IIC | U(S)ART | CAN | USB2.0 FS | USB2.0 HS | Ethernet | BLE | SDIO | FSMC | DVP | VDD     | Package  |
|  ----------- | ----- | ----- |----- |----- |     ---------- |   ---------- |   ---------- |       ---------- |          ----- |----- | --- |               ---------- |    ----- |       ----- | --- |----- | --- | --- | --- |   ----- | --- |     ----- |     ----- |    ----- | --- |----- |----- | --- |   ----- |   ------ |    
| CH32F103C6T6 | 72MHz | 32K   | 10K  | 37   | 1              | 2            | -            | -                | 1              | 2    | 1   |  1/10                    | 10       |  1          | -   | -    | 1   | -   | 1   | 2       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | LQFP48   |     
| CH32F103C8U6 | 72MHz | 64K   | 20K  | 37   | 1              | 3            | -            | -                | 1              | 2    | 1   |  1/10                    | 10       |  1          | -   | -    | 2   | -   | 2   | 3       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | QFN48    |
| CH32F103C8T6 | 72MHz | 64K   | 20K  | 37   | 1              | 3            | -            | -                | 1              | 2    | 1   |  1/10                    | 10       |  1          | -   | -    | 2   | -   | 2   | 3       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | LQFP48   |
| CH32F103R8T6 | 72MHz | 64K   | 20K  | 51   | 1              | 3            | -            | -                | 1              | 2    | 1   |  1/16                    | 16       |  1          | -   | -    | 2   | -   | 2   | 3       | 1   | D+H/D     | -         | -        | -   | -    | -    | -   | 3.3/5.0 | LQFP64M  |

# 文件结构

```
├── Debug                # 用于演示目的, 可选
├── Examples             # 外设代码示例
├── StdPeriphDriver      # 封装库代码和头文件, 必须
│   ├── inc
│   └── src
├── System               # 系统配置文件, 必须
├── USB-Config           # USB 配置, 可选
└── USB-Driver           # USB 封装库, 可选
    ├── inc
    └── src
```

根据与 STMicroelectronics STM32F103 SPL 封装库的对比, 两者在对应外设的部分基本上是一样的, 从寄存器地址, 变量名, 函数名, 甚至代码格式都是一样的. 
这意味着如果不需要使用DAC和USB这些外设, 可以直接使用STM32F103 SPL进行开发.

# 快速开始

这个封装库经过调整, 适用于 PlatformIO 开发环境

## 前提

* 已经安装 VSCode 和 PlatformIO 扩展
* 在 PlatformIO 中, 已经安装 ST STM32 platform
* ST-Link 模块用于下载

## 创建项目

1. 在 PlatformIO Home界面, 点击 "New Project"
1. Project Name: 输入项目名
1. Board: 选择 STM32F103C8T6 Generic 或 Bluepill F103C8 Generic
1. Framework: CMSIS
1. Location: 取消勾选, 指定项目路径
1. 点击 "Finish"

这个操作会在前面指定的位置创建一个空项目.

## 导入 StdPeriphLib_CH32F1

将此仓库clone只本地, 重命名为lib并移入项目目录, 项目目录结构应当如下
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

## 运行 GPIO 示例项目

1. 将 lib/Examples/GPIO/GPIO_Toggle 目录下除了 ch32f10x_conf.h 以外的所有文件, 复制到 src 目录下
   * 不复制 ch32f10x_conf.h 文件是因为这个文件在 lib/System/ 下已经存在, 而且是一样的
1. 编译项目

## 下载/烧录到开发板

编辑 platformio.ini, 添加一行 upload_flag 参数, 否则下载工具不能正确识别 CH32F103 (以及其它国产克隆)芯片
```
upload_flags = -c set CPUTAPID 0x2ba01477
```
修改后的配置看起来应该是这样的
```
[env:bluepill_f103c8_ch32]
platform = ststm32
board = bluepill_f103c8
framework = cmsis
upload_flags = -c set CPUTAPID 0x2ba01477
```
然后使用STLink连接开发板, 点击下载就可以了


# 授权

* StdPeriphLib_CH32F1 基于沁恒官网的 [CH32F103EVT_ZIP](http://www.wch.cn/downloads/CH32F103EVT_ZIP.html) 修改.
* 原代码包中未设置版权和授权说明
* StdPeriphLib_CH32F1 不会主张任何原代码的版权
* 此项目下所有新的提交, 使用 Apache-2.0 授权.
