# Taiko Input

太鼓输入硬件，usb为HID设备，可以连接2个鼓和8通道的数字量输入。

[ENG](./README_EN.md)

<br/>

## [硬件工程](./HW/)
## [USB 单片机](./USB-MCU/)
## [采样单片机](./Sampling-MCU/)
## [上位机](./QT-APP/)
## [文档](./TEXT/)

<br/><br/>

## 工程结构：
```
|—— HW
    |—— taiko-io
    |—— taiko-io-mini
|—— USB-MCU
    |—— TK_usb_CH552
    |—— TK_usb_CH552_mini
|—— Sampling-MCU
    |—— 8chV3_WithCH552
|—— QT-APP
    |—— APP1_Taiko_IO_Setting
|—— TEXT
```

<br/><br/>

## HW 硬件工程

硬件工程位于此目录下。  

taiko-io/内放置的是双鼓的工程，可用于两个鼓的使用环境，工程使用KICAD设计。  
此项目硬件包含8路模拟输入和8路数字量输入，板载配置接口，单usb供电。  

taiko-io-mini/内放置的是单鼓的工程，一般用于一个鼓的使用环境，工程使用LCEDA设计。  
此项目硬件包含4路模拟输入和4个按键，需要测试点配置，单usb供电。  

<br/><br/>

## USB-MCU USB单片机

USB单片机的软件工程位于此目录下。包含两个软件，根据硬件不同，适配的软件工程也不同。  
使用的单片机为 CH552/CH554，是一款USB 8位单片机。  

TK_usb_CH552/软件适配双鼓硬件，只能枚举成为一个USB-HID键盘。  

TK_usb_CH552_mini/软件适配单鼓硬件，可以枚举成USB-HID键盘或n******o switch适用的手柄设备。  

软件使用keil 4编译，编译前根据宏可以关闭/打开相关的软件功能。  

<br/><br/>

## Sampling-MCU 采样单片机

采样单片机的软件工程位于此目录下。软件使用宏区分两款硬件。  
使用的单片机为 STM32G030。  

软件使用keil 5编译，编译前根据宏可以关闭/打开相关的软件功能，切换硬件版本。  

<br/><br/>

## QT-APP 上位机

上位机的软件工程位于此目录下。上位机可以用于配置硬件的相关参数。  

使用的UI框架为QT 5  

<br/><br/>

## TEXT 文档

一些中间文档和输出文档位于此目录下。  

<br/><br/>

# 系统框图
详见[硬件工程](./HW/)  

![系统框图](./HW/img/hardware.png "系统框图")  
<center>系统框图</center>

