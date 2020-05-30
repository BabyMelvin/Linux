# 一、libusb介绍

libusb 设计了一系列的外部API 为应用程序所调用，通过这些API应用程序可以操作硬件，从libusb的源代码可以看出，这些API 调用了内核的底层接口(ioctl)，和kernel driver中所用到的函数所实现的功能差不多，只是libusb更加接近USB 规范。使得libusb的使用也比开发内核驱动相对容易的多。相关资料见[](http://www.libusb.org/)。

对于开发人员需要仔细阅读下api及其使用的说明文档：[](http://libusb.sourceforge.net/api-1.0/)，在文档中详细的说明api具体作用，及调用流程。

# Libusb API 函数与功能

了解包含的 API 函数以及结构体相关定义等很简单，直接在安装路径下找到我们编程时需要引用的头文件即可，这里对应的头文件是 usb.h。我们打开 usb.h 可以看到，代码上部分为 usb 协议标准术语的宏定义，中间为相关结构体定义，后半部为我们编程引用的 API 声明，下面根据 API 功能和使用顺序进行分类描述：

核心初始化函数

```c
1. void usb_init(void);
```

函数功能：libusb 初始化，这一步必须首先执行。

```c
2. int usb_find_busses(void);
```

函数功能：查找系统上的 usb 总线，任何 usb 设备依附于 usb 总线和主机通信。成功返回 usb 总线数目，失败返回负值。

```c
3. int usb_find_devices(void);
```

函数功能：查找总线上的 usb 设备，这个函数必须在调用 usb_find_busses() 后使用。成功返回设备数目，失败返回负值。

```c
4. struct usb_bus *usb_get_busses(void);
```

函数功能：返回全局变量 usb_busses，用于遍历总线链表，查找总线 usb 设备。失败返回 NULL。这四步操作是任何程序必须首先执行的。

# 设备操作接口

```c
1. usb_dev_handle *usb_open(struct usb_device *dev);
```

函数功能：打开 usb 设备，引用的 usb_dev_handle 以及 usb_device 结构体是 libusb 最常见的，定义可以参见头文件。函数返回 usb 设备的操作句柄，之后对该 usb 设备的操作都通过该句柄进行；

```c
2. int usb_close(usb_dev_handle *dev);
```

函数功能：关闭 usb 设备，在所有操作完成后或者程序退出前调用，传入参数即上面提到的句柄，成功返回0；

```c
3. int usb_get_string(usb_dev_handle *dev, int index, int langid, char *buf, size_t buflen);
```

函数功能：获取设备的字符串描述，通过 index 和 langid 进行索引。返回Unicode字符串到buf中，成功返回实际写入buf 的字节数，失败返回负值。

```c
4. int usb_get_string_simple(usb_dev_handle *dev, int index, char *buf, size_t buflen); 
```
函数功能：包装了 usb_get_string() 函数，返回第一种语言指定 index 索引的字符串描述，并转换到C风格的 ASCII。成功返回实际写入buf 的字节数，失败返回负值。

```c
5. int usb_get_descriptor_by_endpoint(usb_dev_handle *udev, int ep, unsigned char type, unsigned char index, void *buf, int size);
```

函数功能：从设备获取描述符，以 type 和 index 索引，以ep标志的控制管道。成功返回实际写入buf 的字节数，失败返回负值。


```c
6. int usb_get_descriptor(usb_dev_handle *udev, unsigned char type, unsigned char index, void *buf, int size);

```

函数功能：获取设备缺省控制管道的描述符，通过 type 和 index 索引。成功返回实际写入buf 的字节数，失败返回负值。

```c
7. int usb_set_configuration(usb_dev_handle *dev, int configuration);
```

函数功能：设置当前设备使用的configuration，参数configuration 是设备配置描述符中的 bConfigurationValue。成功返回0，失败返回负值。

```c
8. int usb_claim_interface(usb_dev_handle *dev, int interface);
```

函数功能： 注册设备与操作系统通信的接口，interface 参数是接口描述符的 bInterfaceNumber 字段。这个函数必须被调用，因为只有注册接口，才能做相应的操作。必须在任何接口相关操作(如 usb_set_altinterface() 、 usb_bulk_write() 等)之前调用。成功返回0，失败返回负值。

```c
9. int usb_release_interface(usb_dev_handle *dev, int interface);
```

函数功能：注销在 usb_claim_interface 中注册的通讯接口，释放相关资源，与注册函数成对使用。


```C
10. int usb_set_altinterface(usb_dev_handle *dev, int alternate);
``` 
函数功能：设定当前接口中的活跃（起作用的）设置，alternate 为接口描述符中 bAlternateSetting 字段。成功返回0，失败返回负值。


```c
11. int usb_resetep(usb_dev_handle *dev, unsigned int ep);
```

函数功能：复位指定的端点，参数 ep 为端点描述符中的 bEndpointAddress，此函数已被函数 usb_clear_halt 替代。成功返回0，失败返回负值。


```c
12. int usb_clear_halt(usb_dev_handle *dev, unsigned int ep);
```

函数功能：复位指定的端点，参数 ep 为端点描述符中的 bEndpointAddress。成功返回0，失败返回负值。

```c
13. int usb_reset(usb_dev_handle *dev);
```

函数功能：重置指定设备，通过发送 RESET 指令过去。返回0成功，负数失败。在执行该函数之后，需要重新枚举设备，且之前的句柄无法再工作。因此现在使用 usb_close 函数来替代。


```c
14. int usb_get_driver_np(usb_dev_handle *dev, int interface, char *name, unsigned int namelen);

```
函数功能：获取设备接口对应的驱动程序名称，也即遍历 sysfs 系统中驱动与设备的对应关系，查找当前系统中有无该接口驱动，成功驱动名写入到 name 中且函数返回0，失败返回负值。


```c
15. int usb_detach_kernel_driver_np(usb_dev_handle *dev, int interface);
```

函数功能：从内核中分离该接口驱动程序，也即禁止该驱动对设备起作用。这个步骤不可省略，不然会发现驱动和使用 Libusb 编写的应用程序同时对设备起作用，引起错误。使用时可以先通过 usb_get_driver_np 进行判断，如果有绑定的驱动程序，那么再调用此函数进行分离。成功返回0，失败返回负值。

# 数据通讯接口

```c
1. int usb_bulk_write(usb_dev_handle *dev, int ep, const char *bytes, int size, int timeout);
```

函数功能：批量端点写函数，传入指定的端点号，timeout 为超时时间，以 ms 为单位。成功返回实际写入设备的字节数，失败返回负值。

```c
2. int usb_bulk_read(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout); 
```
函数功能：批量端点读函数，传入指定的端点号，timeout 为超时时间，以 ms 为单位。成功返回实际写入 buf 的字节数，失败返回负值。

```c
3. int usb_interrupt_write(usb_dev_handle *dev, int ep, const char *bytes, int size, int timeout);
```

函数功能：中断端点写函数，传入指定的端点号，timeout 为超时时间，以 ms 为单位。成功返回实际写入设备的字节数，失败返回负值。

```c
4. int usb_interrupt_read(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
```

函数功能：中断端点读函数，传入指定的端点号，timeout 为超时时间，以 ms 为单位。成功返回实际写入 buf 的字节数，失败返回负值。

```c
5. int usb_control_msg(usb_dev_handle *dev, int requesttype, int request, int value, int index, char *bytes, int size, int timeout);
```

函数功能：控制请求读写函数，传入参数与 usb 协议保持一致，timeout 为超时时间，以 ms 为单位。成功返回实际读写的字节数，失败返回负值。

# 辅助函数接口

```c
1. char *usb_strerror(void);
```

函数功能：根据错误返回值 errno，返回错误码码对应的字符串。

```c
2. void usb_set_debug(int level);
```

函数功能：设置 libusb 的调试级别，libusb 支持打印调试信息，level 值为指定的调试级别。

```c
3. struct usb_device *usb_device(usb_dev_handle *dev);
```

函数功能：根据句柄返回对应的 usb_device。
