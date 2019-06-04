本文主要是关于SPI flash的相关介绍，并着重对SPI flash的原理及其读写进行了相近的阐述。

# SPI flash
SPI一种通信接口。那么严格的来说SPI Flash是一种使用**SPI通信的Flash**，即，可能指NOR也可能是NAND。但现在大部分情况默认下人们说的SPI Flash指的是SPI NorFlash。早期Norflash的接口是`parallel`的形式，即把数据线和地址线并排与IC的管脚连接。但是后来发现不同容量的Norflash不能硬件上兼容（数据线和地址线的数量不一样），并且封装比较大，占用了较大的pcb板位置，所以后来逐渐被SPI（串行接口）Norflash所取代。同时不同容量的SPI Norflash管脚也兼容封装也更小。，至于现在很多人说起NOR flash直接都以SPI flash来代称。

NorFlash根据数据传输的位数可以分为**并行**（Parallel，即地址线和数据线直接和处理器相连）NorFlash和**串行**（SPI，即通过SPI接口和处理器相连）NorFlash；区别主要就是：

* 1、SPI NorFlash每次传输一bit位的数据，parallel连接的NorFlash每次传输多个bit位的数据（有x8和x16bit两种）； 
* 2、SPI NorFlash比parallel便宜，接口简单点，但速度慢。

NandFlash是地址数据线复用的方式，接口标准统一（x8bit和x16bit），所以不同容量再兼容性上基本没什么问题。但是目前对产品的需求越来越小型化以及成本要求也越来越高，所以SPI NandFlash渐渐成为主流，并且采用SPI NANDFlash方案，主控也可以不需要传统NAND控制器，只需要有SPI接口接口操作访问，从而降低成本。另外SPI NandFlash封装比传统的封装也小很多，故节省了pcb板的空间。

**SPI NorFlash**:节省成本，减小封装，存储数据。

# 使用
说白了对于Flash就是**读写擦**，也就是实现flash的驱动。先简单了解下spi flash的物理连接。

之前介绍SPI的时候说过，SPI接口目前的使用是多种方式（具体指的是物理连线有几种方式），Dual SPI、Qual SPI和标准的SPI接口（这种方式肯定不会出现在连接外设是SPI Flash上，这玩意没必要全双工），对于SPI Flash来说，主要就是Dual和Qual这两种方式。

具体项目具体看了，理论上在CLK一定的情况下， 线数越多访问速度也越快。我们项目采用的Dual SPI方式，即两线。

当前涉及到具体的SPI flash芯片类型了，所以必须也得参考flash的datasheet手册了。我们以W25Q64JVSSIQ为例。


