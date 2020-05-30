# 1.USB命令
在USB规范里，对命令一词提供的单词为“Request”，但这里为了更好的理解主机与设备之间的主从关系，将它定义成“命令”。

所有的USB设备都要求对主机发给自己的控制命令作出响应，USB规范定义了11个标准命令，它们分别是：`Clear_Feature`、`Get_Configuration`、`Get_Descriptor`、`Get_Interface`、`Get_Status`、`Set_Address`、`Set_Configuration`、`Set_Descriptor`、`Set_Interface`、`Set_Feature`、`Synch_Frame`。所有USB设备都必须支持这些命令（个别命令除外，如Set_Descriptor、Synch_Frame）。

　不同的命令虽然有不同的数据和使用目的，但所有的USB命令结构是一样的。下表所示为USB命令的结构：

* bmRequsetType:1byte (偏移0字节)
    * D7:传输方向
        * 0:OUT，主机至设备
        * 1:IN,设备到主机
    * D6,5:种类
        * 0:标准
        * 1：类
        * 2：厂商
        * 3:保留
    * D4..0:接收者
        * 0:设备
        * 1:接口
        * 2:端点
        * 3:其他
        * 4..31:保留
* bRequest:1 byte (偏移1字节)
    * 命令类型编码值
* wValue:2 byte (偏移2字节)
    * 根据不同的命令，含义不同
* wIndex:2 byte (偏移4字节)
    * 主要用于索引和偏移
* wLength:2 byte (偏移6字节)
    * 有数据传送阶段，此数据字节数

## 2.USB 11种标准命令

* Clear_Feature
    * bmRequestType
        * 00000000B
        * 00000001B
        * 00000010B
    * bRequst
        * CLEAR_FEATURE
    * wValue
        * 特性选择符
    * wIndex
        * 零
        * 接口号
        * 端口号
    * wLength
        * 零
    * Data
        * 无
* Get_Configuration
    * bmRequestType
        * 10000000B
    * bRequst
        * GET_CONFIGURATION
    * wValue
        * 零
    * wIndex
        * 零
    * wLength
        * -
    * Data
        * 配置值
* Get_Desciptor
    * bmRequestType
        * 10000000B
    * bRequst
        * GET_DESCRIPTOR
    * wValue
        * 描述表种类(高字节,见表5)和索引(低字节)
    * wIndex
        * 零或语言标志
    * wLength
        * 描述表长
    * Data
        * 描述表
