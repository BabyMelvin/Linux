
* 1.程序具备全部CAN功能，包括CAN标准帧/扩展帧接收与发送、CAN总线错误判断、环回等功能
* 2.适用基于LINUX SOCKET机制实现的CAN接口，可用于嵌入式LINUX的CAN测试
* 3.程序采用标准LINUX命令行参数选项形式，接受用户参数

# 功能介绍

## 介绍
本SOCKET CAN程序具备全部CAN功能，包括CAN标准帧/扩展帧接收与发送、CAN总线错误判断、环回等功能适用基于LINUX SOCKET机制实现的CAN接口，可用于嵌入式LINUX中的CAN测试程序
程序采用标准LINUX命令行参数选项形式，接受用户参数

用法: `./cantool [选项]…`

* `-p`, `–port=CAN接口号`:指定CAN接口号，从1开始， 默认为 1（即CAN1接口）
* `-b`, `–baud=波特率`:指定CAN通讯波特率，单位Kbps，默认为 250 Kbps
可用波特率：5，10，20，40，50，80，100，125，200，250，400，500，666，800，1000
* `-i`, `–frame-id=帧ID`:指定CAN发送帧ID（Hex格式）， 默认为1801F456
* `-d`, `–data=数据`:指定CAN发送帧数据， 默认为：00 01 FF FF FF FF FF FF，字节数据间以空格隔开
* `-f`, `–freq=间隔`:指定CAN帧发送间隔，单位ms， 默认为250ms, 最小值为1ms
* `-t`, `–times=次数`:指定CAN帧发送次数， 默认为0次
* `-s`:指定CAN发送帧为标准帧， 默认为发送扩展帧
* `-I`:帧ID每发送一帧递增， 默认不递增
* `-g`, 发送数据每发送一帧递增， 默认不递增
* `-l`, 发送数据时本地环回， 默认不环回

* `–help`显示此帮助信息并退出

注意，以下CAN帧ID作为系统使用:

```
0x00000001 – TX timeout (by netdevice driver)
0x00000002 – lost arbitration / data[0]
0x00000004 – controller problems / data[1]
0x00000008 – protocol violations / data[2..3]
0x00000010 – transceiver status / data[4]
0x00000020 – received no ACK on transmission
0x00000040 – bus off
0x00000080 – bus error (may flood!)
0x00000100 – controller restarted
```

