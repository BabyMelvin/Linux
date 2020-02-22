# 字符设备驱动 异步通知
为了使设备支持异步通知机制，驱动程序中涉及以下3 项工作：

1. 支持F_SETOWN 命令，能在这个控制命令处理中设置`filp->f_owner`为对应进程ID。
不过此项工作已由内核完成，设备驱动无须处理。
2. 支持F_SETFL 命令的处理，每当FASYNC 标志改变时，驱动程序中的`fasync()`函数将得以
执行。驱动中应该实现`fasync()`函数。
3. 在设备资源可获得时，调用`kill_fasync()`函数激发相应的信号

## 1.应用程序
`fcntl(fd, F_SETOWN, getpid()); // 告诉内核，发给谁`应用程序会调用“fcntl()”这个函数，把进程的PID 号告诉给驱动程序。应用程序还要通过“F_GETFL”读出“flags”，在flags 上置上“FASYNC”位。

```c
Oflags = fcntl(fd, F_GETFL);
fcntl(fd, F_SETFL, Oflags, | FASYNC); // 改变fasync标记，最终会调用到驱动程序的
fasync_helper:初始化/释放fasync_struct
```

### 1.1应用程序主动的去查询或read

1.查询方式：很占资源。

2.中断机制：虽然有休眠，但在没有按键按下时`read()`会一直等待，永远不会返回。

3.poll 机制：指定超时时间。
以上都是“应用程序”主动去读或查询。

### 1.2异步通知
有按键按下了，驱动程序来提醒（触发）“应用程序”去读键值。用“signal”进程之间发信号：`kill -9 pid`

* kill 是信号发送者，
* pid 具体进程是信号接收者。
* 信号值是“9”

“信号”与“中断”差不多。注册中断处理函数时是用“request_irq(中断号，处理函数)”。
信号也是有一个“信号”和“处理函数”。参数是“信号的值”，和要挂接的“信号处理函数”。

```c
#include <stdio.h>
#include <signal.h>

void my_signal_fun(int signum)
{
	static int cnt = 0;
	printf("signal = %d, %d time\n", signnum , ++cnt);
}

int main(int argc, char **argv)
{
	signal(SIGUSR1, my_signal_fun);

	while(1) {
		sleep(1000); //这个主函数实现是休眠
	}
	
	return 0;
}
```
在后台执行：`#./signal &` 可见一直在“休眠”状态。这时发送一个信号给它。用应用程序“kill”给它发一个信号。发“USR1”信号`#kill -USR1 2862`给进程。首先要知道进程号是多少。上面是“2862”

程序"signal"一收到这个“Kill”信号时，就会调用里面的“信号处理函数”“my_signal_fun”。
`－USR1` 与 `－10` 是一样的。
1，先注册“信号处理函数”。
2，发送信号。
	* 谁来发信号。
	* 发给谁。
	* 怎么发。

## 2.异步通知功能的驱动函数的应用程序
目标：按下按键时，驱动程序通知应用程序。（以前是应用程序主动读取按键值）

1，应用程序中要注册“信号处理函数”。因为通知它做什么事情，这要一个函数来做。
2，谁发：是驱动程序发送信号。
3，发给谁：信号发送给应用程序。应用程序要告诉驱动程序它自已的PID，
4，如何发：驱动程序中调用某个函数（`kill_fasync()`）

找一个字符驱动查看 kill_fasync 函数的用法。接上面4 个步骤查看用法，下面找到一个：

`rtc.c`

1，首先看这个字符设备中定义的一个“fasync_struct”结构的变量：rtc_async_queue。
2，看这个变量的定义，初始化和使用，整个rtc.c 中有三处出现“rtc_async_queue”

* 定义

`static struct fasync_struct *rtc_async_queue;`

* 初始化

```c
static int rtc_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &rtc_async_queue);
}
```

* 使用

`kill_fasync(&rtc_async_queue, SIGIO, POLL_IN)`

3，要清楚使用方法就要清楚“rtc_fasync（）”在哪里定义：

```c
static const struct file_operations rtc_fops = {
	.owner = THIS_MOUDLE,
	.llseek = no_llseek,
	.read = rtc_read,
#ifdef RTC_POLL
	.poll = rtc_poll,
#endif
	.ioctl = rtc_ioctl,
	.open = rtc_open,
	.release = rtc_release,
	.fasync = rtc_fasync,
};
```

在这个“rtc.c”中可以找到这个函数的定义，它是一个“file_operatons”结构中的成员。
以上弄清楚“kill_fasync”函数的用法之后，可以依照：
在“中断服务程序”中发：即当有按键按下时就发送一个信号给应用程序。
首先声明一个“fasync_struct”结构变量。

```c
static struct fasync_struct *button_async;

static irqreturn_t buttons_irq(int irq, void *dev_id)
{
	struct pin_desc *pindesc = (struct pin_desc *)dev_id;
	unsigned int pinval;

	pinval = s3c2410_gpio_getpin(pindesc->pin);
	if(pinval)
		key_val = 0x80 | pindesc->key_val;
	else
		key_val = pindesc->key_val;

	ev_press = 1; //表示中断发生了
	wake_up_interruptible(&button_waitq);
		
	kill_fasync(&button_async, SIGIO, POLL_IN);
	return IRQ_RETVAL(IRQ_HANDLED);
}
```

有按键按下了就发一个信号给应用程序。其中要定义一个结构，取名为“button_async”,以上便可以发送信号出去了。

发给谁肯定是这个结构中定义的。这个结构“button_async”要做些初始化。
有如下例子，这里的结构体是“rtc_async_queue”，看看这个“fasync_helper”函数是否在初
始化这个结构体。

```c
static int rtc_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &rtc_async_queue);
}
```
查询到函数“rtc_fasync”的定义：

```c
static struct file_operaitons second_drv_fops = {
	.owner = THIS_MODULE,
	.open = fifth_drv_open,
	.read = fifth_drv_read,
	.release = fifth_drv_close,
	.poll = fifth_drv_poll,
	.fasync = fifth_drv_fasync,
};
```
然后加上这个初始化“button_async”结构的函数“fasync_help”：

```c
static int fifth_drv_fasync(int fd, struct file *filp, int on)
{
	printk("driver: fifth_drv_fasync\n");
	return fasync_helper(fd, filp, on &button_async);
}
```
这个函数“fifth_drv_fasync”什么情况下调用：

信号是驱动程序发的，在中断服务程序“buttons_irq”通过“kill_fasync (&button_async, SIGIO,
POLL_IN);” 来发。发给谁是包含在“ button_async ” 这个结构体中。这个结构体在
“fifth_drv_fasync”中由“fasync_helper”来初始化。这个函数“fifth_drv_fasync”什么时候被
调用，是应用程序调用“.fasync”

这么一个“.fasync”系统调用时，就会调用到：fifth_drv_fasync.所以显然是应用程序要调用“.fasync”来设置那个“发给谁”。

为了使折别支持异步通知机制，驱动程序中涉及以下3项工作：

1.支持SETOWN命令，能在这个控制命令处理中设置`filp->f_owner`为对应进程ID。不过此项工作已经由内核完成，设备驱动无需处理。

2.支持F_SETFL命令的处理，每当FASYNC标志改变时，驱动程序中的`fasync()`函数将得以执行。驱动中应该实现`fasync();`函数

3.在设备资源获取时，调用`kill_fasync()`函数激发相应的信号

应用程序:

```c
fcntl(fd, F_SETOWN, getpid()); // 告诉内核，发给谁
Oflags = fcntl(fd, F_GETFL);
fcntl(fd, F_SETFL, Oflags | FASYNC); //改变fasync标记，最终调用到驱动fasync > fasync_helper 初始化/释放fasync_struct
```

为了使设备支持异步通知机制，驱动程序中涉及以下3 项工作：

1. 支持F_SETOWN 命令，能在这个控制命令处理中设置filp->f_owner 为对应进程ID。
不过此项工作已由内核完成，设备驱动无须处理。
2. 支持F_SETFL 命令的处理，每当FASYNC 标志改变时，驱动程序中的fasync()函数将得以
执行。驱动中应该实现fasync()函数。
3. 在设备资源可获得时，调用kill_fasync()函数激发相应的信号

## 3.应用程序
如上面“`fcntl(fd, F_SETFL, Oflags | FASYNC);`”接口被调用时，下在函数中的“`fasync_helper()`”就会被调用：

```c
static int fifth_drv_fasync(int fd, struct file *filp, int on)
{
	printk("driver: fifth_drv_fasync\n");
	return fasync_helper(fd, filp, on, &button_async);
}
```

应用程序会调用“F_SETOWN”即“fcntl(fd, F_SETOWN, pid)”这样一个函数，来进程的pid 告
诉驱动程序。然后应用程序还要通过“F_GETFL”读出标志位，即flags，先读出来再在这个flags 上面修
改，加上一个“FASYNC”。异步通知的flags.每当FAYNC 标志“flags”改变时，驱动程序里
面的

fasync_helper”，这是内核帮做的一个函数。是辅助作用。将结构体
变量“button_async”初始化，我们定义了但没去初始化它，这个内核辅助函数就去初始化它。
这个结构体“button_async”初始化后，中断服务程序“buttons_irq”就能调用：

```c
kill_fasync(&button_async, SIGIO, POLL_IN);
```

当应用程序调用`这个接口“fcntl(fd, SETFL, oflags | FASYNC)`”时，函数“`fasync_helper`”
就会被调用。

驱动程序对“F_SETOWN”的处理是由内核完成的。

```c
int fd;

void my_signal_fun(int signum)
{
	unsigned char key_val;
	read(fd, &key_val, 1); //读了按键值
	printf("key_val:0x %x\n", key_val);
}

int main(int argc, char **argv)
{
	unsigned char key_val;
	int ret;
	int Oflags;

	//内核中发出信号是SIGIO，表示IO有数据让你读和谐
	signal(SIGIO, my_signal_fun);
	fd = open("/dev/buttons", O_RDWR);
	if(fd < 0)
		printf("can't open!");

	fcntl(fd, F_SETOWN, getpid());
	Oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, Oflags | FASYNC);

	while(1) {
		sleep(1000);
	}
	
	return 0;
}
```

这是驱动测试程序。它不会主动去读按键值“`read(fd, &key_val, 1); `”。这个“my_signal_fun”
信号函数是在驱动服务程序“buttons_irq”里，发现有按键按下时，给应用程序发“`kill_fasync
(&button_async, SIGIO, POLL_IN);`”信号，这个信号会触发应用程序来调用它的信号处理函数
“my_signal_fun”。