# API介绍
在libusb0.1中，函数有两种返回值，一种为usb_open函数返回打开的设备句柄handle，第二种为int型，当返回值大于等于0时表示成功，返回值小于0表示失败

报告错误代码在WDK crt_errno.h文件中标准的错误代码和一些MinGW的说明如下：

```c
#define EPERM 1 /* Operation not permitted */

#define ENOENT 2 /* No entry, ENOFILE, no such file ordirectory */

#define ESRCH 3 /* No such process */

#define EINTR 4 /* Interrupted function call */

#define EIO 5 /* Input/output error */

#define ENXIO 6 /* No such device or address */

#define E2BIG 7 /* Arg list too long */

#define ENOEXEC 8 /* Exec format error */

#define EBADF 9 /* Bad file descriptor */

#define ECHILD 10 /* No child processes */

#define EAGAIN 11 /* Resource temporarily unavailable */

#define ENOMEM 12 /* Not enough space */

#define EACCES 13 /* Permission denied */

#define EFAULT 14 /* Bad address */

#define EBUSY 16 /* strerror reports "Resourcedevice" */

#define EEXIST 17 /* File exists */

#define EXDEV 18 /* Improper link (cross-device link?) */

#define ENODEV 19 /* No such device */

#define ENOTDIR 20 /* Not a directory */

#define EISDIR 21 /* Is a directory */

#define EINVAL 22 /* Invalid argument */

#define ENFILE 23 /* Too many open files in system */

#define EMFILE 24 /* Too many open files */

#define ENOTTY 25 /* Inappropriate I/O control operation*/

#define EFBIG 27 /* File too large */

#define ENOSPC 28 /* No space left on device */

#define ESPIPE 29 /* Invalid seek (seek on a pipe?) */

#define EROFS 30 /* Read-only file system */

#define EMLINK 31 /* Too many links */

#define EPIPE 32 /* Broken pipe */

#define EDOM 33 /* Domain error (math functions) */

#define ERANGE 34 /* Result too large (possibly toosmall) */

#define EDEADLK 36 /* Resource deadlock avoided */

#define ENAMETOOLONG 38 /* Filename too long */

#define ENOLCK 39 /* No locks available */

#define ENOSYS 40 /* Function not implemented */

#define ENOTEMPTY 41 /* Directory not empty */
```
## 2.函数
### 2.1 core
void usb_init(void);

初始化libusb，usb_init设置了一些内部结构体。usb_init必须在libusb的其他功能函数之前被调用。

 

int usb_find_busses(void);

寻找系统中的所有的usb总线，返回自上次调用此函数以来发生的更改次数（已删除新总线和总线的总数）。

 

int usb_find_devices(void);

该函数会扫描在每条usb总线上的所有设备，并且必须在usb_find_busses()函数后调用，返回自上次调用此函数依赖法伤的变化次数（所有的新设备和被移除的设备）。

 

struct usb_bus *usb_get_busses(void);

这是为那些支持C调用约定的语言实现的，并且可以使用共享库，但是不要支持C全局变量， 返回全局变量usb_busses的值。

 

void usb_set_debug(int level);

该函数设置调试信息的信息级别，设置为4就可以得到信息的调试信息。

0 LOG_OFF, 1 LOG_ERROR, 2 LOG_WARNING, 3 LOG_INFO, 4LOG_DEBUG

### 2.2 Device operations

该组函数主要是与设备相关的操作，如打开、关闭设备等一些usb标准的操作：设置配置、 alternate settings、clearing halts和重启设备等。同时也会提供操作系统级的操作，如声明和释放接口。

usb_dev_handle *usb_open(struct *usb_device dev);

打开一个usb设备，打开设备成功会返回一个句柄，以后的程序就可以通过该句柄来进行设备的通信。

 

int usb_close(usb_dev_handle *dev);

关闭usb设备

返回0：关闭设备成功

小于0：关闭设备失败

 

int usb_set_configuration(usb_dev_handle *dev, intconfiguration);

根据配置描述符bConfigurationValue来配置使能usb设备。

返回0：设置成功

小于0：设置失败

 

int usb_set_altinterface(usb_dev_handle *dev, intalternate);

设置当前接口的一个可选择的setting

 

int usb_resetep(usb_dev_handle *dev, unsigned int ep);

重置指定地址bEndpointAddress端点所有的状态

 

int usb_clear_halt(usb_dev_handle *dev, unsigned intep);

清除指定地址halt状态端点。

 

int usb_reset(usb_dev_handle *dev);

通过想下行端口发送一个RESET来重启连接该PORT的特定的设备 （会造成重新枚举的动作）

返回0：reset成功

小于0：reset失败

调用usb_reset设备重启后，设备会重新进行枚举成一个新设备并打开一个新句柄。

 

int usb_claim_interface(usb_dev_handle *dev, intinterface);

声明一个有操作系统相关的接口，接口的参数在描述符里的bInterfaceNumber字段里指定。

注意：在进行与该接口相关的操作之前，usb_claim_interface必须被首先调用(like usb_set_altinterface,usb_bulk_write, etc).。

返回值：

EBUSY ：接口是不可用的

ENOMEM：没有足够的内存可用

 

int usb_release_interface(usb_dev_handle *dev, intinterface);

释放先前usb_claim_interface声明的接口

### 2.4 Bulk transfers
该组函数允许应用程序通过批量传输管道向设备发送信息

 

int usb_bulk_write(usb_dev_handle *dev, int ep, char*bytes, int size, int timeout);

向一个批量传输端点向设备写入数据

 

int usb_bulk_read(usb_dev_handle *dev, int ep, char *bytes,int size, int timeout);

向一个批量传输端点向设备读取数据

## 5.Interrupt transfers
该组函数允许应用程序通过中断传输管道向设备发送和接受数据。

 

int usb_interrupt_write(usb_dev_handle *dev, int ep, char*bytes, int size, int timeout);

向一个中断端点写数据。该函数会向指定的端点发起一个中断写请求。

 

int usb_interrupt_read(usb_dev_handle *dev, int ep, char*bytes, int size, int timeout);

向一个中断端点写数据。该函数会向指定的端点发起一个中断读请求。

### 2.6Asynchronous API and Isochronous Transfer
libusb-win32通过异步API支持异步传输。libusb-win32异步传输API也支持其他的传输类型：控制传输、中断传输、批量传输等。

 

int usb_isochronous_setup_async(usb_dev_handle *dev, void **context, unsigned char ep, int pktsize);

为端点ep分配一个异步传输请求。

 

int usb_bulk_setup_async(usb_dev_handle *dev, void**context, unsigned char ep);

 

int usb_bulk_setup_async(usb_dev_handle *dev, void **context,unsigned char ep);

 

int usb_submit_async(void *context, char *bytes, intsize);

 

int usb_reap_async(void *context, int timeout);等待请求完成，返回读写成功的字节数或者失败的返回值。

 

int usb_reap_async_nocancel(void *context, inttimeout);

 

int usb_cancel_async(void *context);取消

 

int usb_free_async(void **context);

