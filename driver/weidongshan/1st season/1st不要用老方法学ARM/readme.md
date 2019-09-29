**学习目标**：

* 看原理图
* 看芯片手册
* 知道怎么读写寄存器

# 不要用老的方法学习ARM

C51，STM32专用单片机开发板，不要使用keil,MDK等集成度太高的软件。

# 软件框架
## 1.WindowsPC 

`BIOS`启动`Windows内核`挂载C盘、D盘`系统盘/应用盘`启动`应用程序`

## 2.Android手机/工控设备

`bootloader`启动`linux内核`挂载`根文件系统`启动`应用程序`

其中bootloader，去`Flash/SD卡读出内核`。bootloader主要工作,写出单独的程序:

* 初始化时钟
* 初始化内存
* 设置网卡
* 有读取Flash/SD卡的能力
* 显示Logo，操作LCD

Linux内核，也要有操作硬件的能力(驱动程序)