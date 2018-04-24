# PCI总线概述

总线是一种**传输信号的信道**,总线是**连接一个或多个导体的电气连线**。总线由**电气接口**和**编程接口**组成，我们重点关注编程接口。

### PCI概念
PCI是Peripheral component Interconnect(外围设备互联)的简称，是在桌面及更大型的计算机普通使用的外设总线。

### PCI特点
PCI总线具有三个非常显著的优点：
* 在计算机和外设间传输数据时具有更好的性能
* 能够尽量独立于具体的平台
* 可以方便地实现即插即用

<image src="./image/06-01.jpg"/>
从结构上看，PCI总线是一种不依赖于某个具体处理器的局部总线，它是在CPU和原来的总线之间插入的一级总线，具体由一个桥接电路实现对这一层的管理，并实现上下之间的接口以协调数据的传递。

<image src="./image/06-02.jpg"/>
系统的各部分通过PCI总线和PCI-PCI桥连接在一起。CPU和RAM通过PCI总线连接到PCI总线0(即主PCI总线)，而具有PCI接口的显卡直接连接到主PCI总线上。PCI-PCI总线是一种特殊的PCI设备，它负责将PCI总线0和PCI总线1连接在一起。图中连接到PCI 1号总线上的SCSI卡和以太网卡。为了兼容旧的ISA总线标准，PCI总线还可以通过PCI-ISA桥来连接ISA总线，从而支持以前的ISA设备,图中ISA总线上连接着一个多功能I/O控制器，用于控制键盘、鼠标和软驱等。

### PCI设备寻址
每个PCI设备由一个总线号，一个设备号，和一个功能号确定。PCI规范允许一个系统最多拥有256条总线，每条总线最多带32个设备，但每个设备可以是最多8个功能板(如一个音频设备带一个CD-ROM驱动器)。

`/proc/iomem`描述了系统中所有的设备I/O在内存地址空间上的映射。从1G开始第一个设备在`/proc/iomem`中是如何描述的:
```
40000000-400003ff:0000:00:1f.1
```
这是一个PCI设备，40000000-4000003ff是它所映射的内存空间地址，占据了内存地址1024bytes的位置，而00000:00:1f.1则是这个PCI外设的地址，它以冒号和逗号分隔为4个部分，第一个16位表示域，第二个8个表示一个总线，第三个5位表示一个设备号，最后是3位，表示功能号。

因为PCI规范允许单个系统拥有最多256条总线，所以总线编号是8位。每个总线上可支持32个设备，所以设备号是5位，而每个设备上最多可有8种功能，所以功能号是3位。由此，由此可以得出上述的PCI设备的地址是0号总线上的31号设备上的1号功能。

使用lspci命令可以查看系统中的PCI设备，下面是使用lspci命令得到一组输出：
```
00:00.0 Host bridge:Intel Cororation 82845 845
```
<image src="./image/06-03.jpg"/>

### 配置寄存器
每个PCI设备都有一组固定格式的寄存器，即配置寄存器，配置寄存器有Linux内核中的PCI初始化代码与驱动程序共同使用。内核在启动时负责对配置寄存器进行初始化，包括设置中段号以及I/O基址等。

<image src="./image/06-04.jpg"/>
```
00H-01H Vendor ID 制造商标识
02H-03H Device ID 设备标识
04H-95H Command 命令寄存器
06H-07H Status  状态寄存器
08H Revision ID版本识别号寄存器
09H-0bH Class Code分类代码寄存器
0cH Cache Line Size CACHE 行长度寄存器
0dH Latency Timer主设备延迟时间寄存器
0eH Header Type头标类型寄存器
0fH Built-in-testet Regeister自测试寄存器
10H-13H Base Address Register 0基地址寄存器0
14H-17H Base Address Register 1基地址寄存器1
18H-1bH Base Address Register 2基地址寄存器2
1cH-19H Base Address Register 3基地址寄存器3
20H-23H Base Address Register 4基地址寄存器4
24H-27H Base Address Register 5基地址寄存器5
28H-2bH Cardbus CIS Pointer设备总线CIS指针寄存器
2cH-2dH Subsystem Vendor ID 子设备制造商标识
2eH-2fH Subsystem Device ID 子设备标识
30H-33H Expasion ROM Base Address扩展ROM基地址
34H-3bH  保留
3cH Interrupt Line中断线寄存器
3dH Interrupt Pin 中断引脚寄存器
3eH Min_Gnt最小授权寄存器
3fH Max_Lat 最大延迟寄存器
```
# PCI 驱动程序设计
# 终端控制台体系
# 串口驱动程序设计