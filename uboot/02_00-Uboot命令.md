## 介绍
uboot是bootloader程序。

## uboot常用命令

* help 当前板支持命令
* set bootdelay 10 ：设置“倒数计时”为 10 秒。加 save 命令 保存，reset 重启后可见到“倒数计时”。
* printenv查看环境变量
* setenv添加，修改，删除环境变量
* saveenv保存环境变量，将当前所有变量及其值存入flash中。
* tfpt网络下载文件,配置和使用
	* setenv ethaddr 12:34:56:78:9A:BC
	* setenv ipaddr 192.168.1.1
	* setenv serverip 192.168.1.254
	* tftp 32000000 uimage把server中服务目录下uimage通过TFTP读入到0x32000000处。

1.**内存操作**
* md显示内存区内容，md采用十六进制和ASCII码两种形式来显示存储单元的内容。md还可以加长度标识`.l`,`.w`和`.b`.`md[,b, .w, .l] address`
	* md:表示4字节一组进行显示,如：08088b1f 4ade4a26
	* md.b:表示一个字节一个字节显示，如:1f 8f


```
>md.w 100000 
001000000:2705 1956 5050 432...(8个word=16byte)
00100010：3520 284d ...         (8个word=16byte)
```
* mm 修改内存，地址自动递增。`mm[.b,.w,.l] address`，修改存储器内容的方法。 会显示地址和当前值，然后提示用户输入。如果输入了一个合法十六进制数，这个将会被写入该地址，然后提示下一个地址。如果没有输入任何信息，只是按回车，那么该地址内容保持不变。如果`结束输入`，输入空格，然后回车。

进行交互输入的方式

若接下去不再修改时，就在“？”处“空格+回车”。即退出了 "mm".

2.**Flash操作**
* flinfo 查看Flash扇区信息


* protect flash写保护，打开或关闭扇区写保护
	* `protect off all`:关闭所有扇区的写保护
	* `protect on all`：打开所有扇区的写保护。
	*  `protect off start end`
	*  `protect on start end`
	
* erase擦出flash扇区
	* `erase start end`使用cp命令向Nor型Flash写入数据前必须使用erase命令擦出flash，因为nor flash按字节写入时，无法写入1,必须通过擦除来写入1.
	* `erase 30000 1effff`
* cp 数据拷贝,`cp[.b,.l,.w] saddress daddress len`.cp提供一种内存与内存，内存与falsh之间数据拷贝的方法。

**程序跳转执行**
* go:执行内存中二进制代码，一个简单跳转到指定地址
	* `go addr [arg..]`开始地址addr,参数为arg。
* `bootm`执行内存中二进制代码.启动application传递参数arg。当启动linux kernel,arg参数可以是`initrd`镜像的地址。
* `bdinfo`显示开发板信息
	* `内存地址和大小`，`时钟频率`，`MAC地址`等信息。

**自动启动**
```
>setenv bootcmd tftp 3100000 uimage;bootm 31000000 
>saveenv
```

## uboot工作流程
### 工作模式
大多数BootLoader都包含两种不同的工作模式：`启动模式`和`下载模式`。

#### 启动模式
BootLoader从目标机上的某个固态存储设备上将操作系统自动加载到RAM中运行，整个过程并没有用户的介入。正常工作模式，因此嵌入式产品发布时候，BootLoader显然必须工作这种模式下。

#### 下载模式
这个模式下，目标机上的BootLoader将通过串口或网络等痛新手段从主机(Host)下载文件，然后控制启动流程。

