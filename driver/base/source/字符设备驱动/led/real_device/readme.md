# 硬件上的操作

配置引脚：操作 GPFCON 寄存器。（放到 open 函数中做）

设置:设备 GPFDAT 寄存器，让引脚输出高、低电平。（放到 wirte 函数中做）

### 1.映射物理地址

```c
volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;
```
先定义两个变量，和寄存器相对应。他们分别的物理地址要分别映射成虚拟地址。
在入口函数中映射（避免打开一次要映射一次）。用`void __iomem *ioremap(phys_addr_t offset, size_t size)`(物理地址开始地址，结束地址)

```c
gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);

// gpfdat:映射后加4个字节，就是0x56000054映射后的地址
gpfdat = gpfcon + 1;
```
先映射GPFCON为虚拟地址。F寄存为

GPFCON:0X56000050,GPFDAT:0x5600004,GPFUP:0x56000058,Reserved:0x5600005c

GPFCON 寄存器的地址，后面 16 是16 字节，这里刚好有4 个寄存器，每个4 字节，所以
就定成16 字节了。
这里加1 不是加了4 字节，指针的操作是以 指针 所指向的长度为单位的，这里的指针是：
`(volatile unsigned long *)`

在入口函数中用`ioreamp()`，则在出口函数中要用 iounmap(),将建立的映射去掉。`iounmap(gpfcon)`

以上就是物理地址与虚拟地址的映射，以后要操作寄存器时，就用 gpfcon 和 gpfdat
两个指针来操作。这两个指针指向的是**虚拟地址**。

```c
int major;

static int first_drv_init(void) // 这是入口函数
{
	major = register_chrdev(0, "first_drv", &first_drv_fops);
	firstdrv_class = class_create(THIS_MOUDLE, "fistdrv"); //先创建一个类
	firstdrv_class_dev = device_create(first_class, NULL, MKDEV(major, 0), NULL, DEV_NAME);
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpdat = gpfcon + 1;

	return 0;
}

void first_drv_exit(void) // 出口函数
{
	unregister_chrdev(major, "first_drv");
	class_device_unregister(firstdrv_class_dev);
	class_destroy(first_class);
	iounmap(gpfcon);
}
```


### 2.设置引脚为输出引脚：先清零，再或上"01"

之后以为配置成“输出引脚”，是原理图上LED 一端接了高电平，只有2440 这端输出低电
平，才有电流通过。
设置成输出引脚：GPF0~3 先清零，再或上 1 。就是输出引脚了。在“first_drv_open”中初始化GPFCON 引脚电平为输出。

```c
/* 配置GPF0 ~ 3 为输出引脚*/
gpfcon &= ~((0x3 << 0 * 2) | (0x3 << 1 * 2) | (0x3 << 2 * 2) | (0x3 << 2 * 2)) ;
gpfcon |= ((0x1 << 0 * 2) | (0x1 << 1 * 2) | (0x1 << 2 * 2) | (0x1 << 3 * 2));
```

### 3. 将用户空间的数据传递给内核空间
val 如何传进来，就是*buf 应用程序中调用的时候

```c
int main()
{
	int fd;
	int val = 1;
	fd = open("/dev/xyz", O_RDWR);
	if(fd < 0) {
		printf("can't open \n");
		reutrn -1;
	}
	
	write(fd, &val, 4);
	return 0;
}
```

`static ssize_t first_drv_write(struct file *file, const char __user *buf, sizt_t count, loof_t *ppos)`

这个值便是 应用程序 传进来的buf 和count.用一个函数来取这些应用程序传过来的值。

* 从用户空间到内核空间的数据传递函数`copy_from_user()`
* 从内核空间向用户空间传递数据用函数 `copy_to_user();`

```c
static inline unsigned long __must_check copy_from_user(void *to, const void __user *from, unsigned n)
{
	if(access_ok(VERIFY_READ, from, n))
		n = __copy_from_user(to, from, n);
	else /* secirty hole - plug it*/
		memzero(to, n);

	return n;
}
```
将传来的buf 拷贝到val（定义的）空间，拷贝的长度是count（应用程序传进来的）。
接着根据这个值判断：如果这个值`val==1`,打开LED（输出低电平）。否则关掉LED 灯（输
出高电平）。打开和关闭LED，就是操作寄存器。

```c
static int frist_drv_write(struct file* file, const char __user *buf, size_t count, loff_t *ppos)
{
	int val;
	printk("first_drv_write\n");
	copy_from_user(&val, buf, count);

	if(val == 1) {
		/* 点灯，输出低电平 */
		*gpfdat &= ~((1 << 0) | (1<<1) | (1<<2) | (1<<3));
	} else {
		/* 灭灯： 输出为高电平 */
		*gpfdat |= ((1 << 0) | (1<<1) | (1<<2) | (1<<3));
	}
}
```
当传进来的值 val 为1，依原理图就要设置 GPF0~3 这些引脚输出0.那就是清位。要关LED时，就使引脚输出高电平，就或上某一位。这里 val = 1,是通过copy_from_user()将"first_drv_write()"中的用户空间的buf 值传进的。

另一种方法是用ioctl（）。驱动测试程序firstdrv_test 打开`/dev/led_test`设备节点时，就会通过设备节点的主设备号进入到内核的file_operations 结构数组中找到与这个驱动的主设备号相关的结构体，进而关联到sys_open等函数上去。

```c
if(strcmp(argv[1], "on") == 0) {
	val = 1;
} else {
	val = 0;
}
write(fd, &val, 4);
```
上面是测试程序，打开'on'时，val=1,从write(fd,&val,4); 中将这个val=1 写到fd 关联的设备节点“/dev/led_tset”，进而通过它的主设备号进入到内核，关联到：

```c
static struct file_operations first_drv_fops = {
	.owner = THIS_MOUDLE,
	.open = first_dev_open,
	.write = first_drv_write,
};
```

# 总结：写一个驱动程序

1.首先写驱动框架。
2.结硬件的操作。
看原理图--看芯片手册--写代码
和单片机差不多，区别在于：
单片机中直接使用物理地址。
驱动程序中得用物理地址映射后的函数地址。
要单独控制某一个LED 灯

```c
static int frist_drv_write(struct file* file, const char __user *buf, size_t count, loff_t *ppos)
{
	int val;
	printk("first_drv_write\n");
	copy_from_user(&val, buf, count);

	if(val == 1) {
		/* 点灯，输出低电平 */
		*gpfdat &= ~((1 << 0) | (1<<1) | (1<<2) | (1<<3));
	} else {
		/* 灭灯： 输出为高电平 */
		*gpfdat |= ((1 << 0) | (1<<1) | (1<<2) | (1<<3));
	}
}
```
1 种方法是解析传进来的值，如传进来1 点亮LED1,传进来2 val == 2,则点亮LED2.根据传进来的值执行不同动作。

2 种方法：设备节点

主设备号是帮我们在内核的 chardrv 数组中找到那一项，再根据这一项找到 file_operatios结构：

```c
static struct file_operations first_drv_fops = {
	.owner = THIS_MOUDLE,
	.open = first_dev_open,
	.write = first_drv_write,
};
```
但是上面的 次设备号 是留下来给我们用的，由我们管理。

```c
int minor = MINOR(inode->i_rdev);
```
用 MINOR 取出次设备号。据不同次设备号来操作。次设备的操作完全由自已定义。
看“myleds.c”这个驱动程序。

```c
static int s3c24xx_leds_open(struct inode *inode, struct file *file)
{
	int minor = MINOR(inode->i_rdev);
	
	switch(minor) {
		case 0: // dev/leds
			// 配置 3引脚为输出
			GPFCON　＆＝　~(0x3 << (4 *2));
			GPFCON |= (1 << (4 * 2));
			break;
	}	
}
```
下面是内核提供的使用虚拟地址操作方式：封装好的

```c
case 1: // /dev/led1
	s3c2410_gpio_cfgpin(S3C2410_GPF4, S3C2410_GPF4_OUTP);
	s3c2410_gpio_setpin(S3C2410_GPF4, 0);

	down(&leds_lock);
	leds_status &= ~(1 << 0);
	up(&leds_lock);
	break;
```

初始化设备文件:

```c
	leds_class_devs[0] = class_device_create(leds_class, NULL, MKDEV(LED_MAJOR, 0), NULL, "leds");
	/* 上面设备为leds, 主设备好为"LED_MAJOR",次设备号为0 */
	for(minor = 1; minor < 4; minor++) {
		leds_class_devs[minor] = class_device_create(leds_class, NULL, MKDEV(LED_MAJOR, minor), NULL, "led%d", minor);
	}
```
