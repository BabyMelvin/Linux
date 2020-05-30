基于libusbx-1.0.18-rc1，libusbx现已重新merage到libusb。

# 1. 初始化

使用libusb_init初始化libusb，如果是单设备通信，ctx参数可以传NULL，表示使用默认Context.

# 2. 注册hotplug事件侦听

从1.0.16开始，libusb支持hotplug事件侦听，可以使用接口libusb_hotplug_register_callback注册基于以下几种filter的回调函数：

```c
int API_EXPORTED libusb_hotplug_register_callback(libusb_context *ctx,
      int events, int flags,
      int vendor_id, int product_id, int dev_class,
      libusb_hotplug_callback_fn cb_fn, void *user_data,
      libusb_hotplug_callback_handle *callback_handle)

```

* libusb_hotplug_event: 分LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED和 LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT俩种；
* vendor_id:只有指定的vendor id匹配的设备能够触发回调，或设置LIBUSB_HOTPLUG_MATCH_ANY，使得任意vendor id的设备都能触发回调；
* product_id:只有指定的product id匹配的设备能够触发回调，或设置LIBUSB_HOTPLUG_MATCH_ANY，使得任意product id的设备都能触发回调；
* dev_class:只有指定的device class匹配的设备能够触发回调，或设置LIBUSB_HOTPLUG_MATCH_ANY，使得任意device class的设备都能触发回调。

这几种filter之间可以以“与”的方式组合一起发生作用。

# 3. 获取file descriptors
使用`libusb_get_pollfds`可以取得当前context的fd列表，之后可以将这些fd放到`main events loop`中进行poll操作。可以参考《基于glib的单线程多事件源处理》这篇文章，使用GSource来实现。一旦检测到有事件发生，要使用libusb_handle_events_completed，触发后面提到的事件处理函数。

还需要使用libusb_set_pollfd_notifiers注册俩个回调函数，用来处理设备接入或移除后，fd列表发生的变化。

# 4. 设备接入

设备接入后，会触发libusb_set_pollfd_notifiers注册的回调函数，通知应用程序有新增的fd，这时需要将新增的fd添加到poll的队列中。同时，libusb还会触发`libusb_hotplug_register_callback`注册的回调函数，通知应用程序有设备接入。

在hotplug的回调函数中，需要基于传入的libusb_device，调用`libusb_open`接口，创建一个基于此设备的handle。之后根据需要，使用`libusb_reset_device`重置此设备。对于一些linux标准支持的设备（如HID设备），可能需要使用libusb_detach_kernel_driver，或libusb_set_auto_detach_kernel_driver，将kernel提供的驱动卸载掉，否则会与libusb的**通信产生冲突**。

对于支持多configuration的设备，还需要使用**libusb_set_configuration**，选择要使用的configuration number。之后，调用`libusb_claim_interface`，选择要使用的interface。

# 5. 建立通信

调用libusb_alloc_transfer创建一个**空的transfer数据结构**，使用`libusb_fill_interrupt_transfer`填充这个数据结构，参数包括：

* 通信基于的endpoint number
* 数据到达的buffer及大小
* 数据到达后的回调函数等

之后调用`libusb_submit_transfer`，将这个transfer提交给libusb侦听数据到达事件。

6. 数据到达
数据到达后，会触发回调函数，传入之前的transfer数据结构。首先要判断transfer中的status字段是否是**LIBUSB_TRANSFER_COMPLETED**，否则执行相应的异常处理。对于传输完成的数据，可以到transfer中的buffer字段获取，数据的长度使用actual_length表示。**处理完成后**，要使用**libusb_submit_transfer重新提交这个transfer**。

到此，基于libusb的异步通信建立完毕，libusb异步通信对于同步通信来说有更好的性能，且不需要多线程支持，程序执行的时序可以得到更好的控制。
