# module_init
module_init这个函数的具体功能和执行过程，在kernel源码目录中找到`include/linux/init.h`文件。
```c
#define module_init(x) __initcall(x)
#define __initcall(fn) device_initcall(fn)

#define device_initcall(fn) __define_initcall(fn, 6)


//initcalls are now grouped by funcitonality into separate subsections.Ordering inside the subsections is determined by link order.

#define __define_initcall() \
	static initcall_t __initcall_##fn##id __used \
	__attribute__((__section__(".initcall"#id ".init"))) = fn; \
	LTO_REFERENCE_INITCALL(__initcall__##fn__##id)
``` 

Note：下面用xxx_initcall来代表pure_initcall，core_initcall、core_initcall_sync … …
我们可以看到非常多的xxx_initcall宏函数定义，他们都是通过__define_initcall 实现的。在__define_initcall里面包含了两个参数，一个是fn，另一个则是id。那么，这么多的宏又有何用？？

```c
#define pure_initcall(fn)       __define_initcall(fn, 0)  
  
#define core_initcall(fn)       __define_initcall(fn, 1)  
#define core_initcall_sync(fn)      __define_initcall(fn, 1s)  
#define postcore_initcall(fn)       __define_initcall(fn, 2)  
#define postcore_initcall_sync(fn)  __define_initcall(fn, 2s)  
#define arch_initcall(fn)       __define_initcall(fn, 3)  
#define arch_initcall_sync(fn)      __define_initcall(fn, 3s)  
#define subsys_initcall(fn)     __define_initcall(fn, 4)  
#define subsys_initcall_sync(fn)    __define_initcall(fn, 4s)  
#define fs_initcall(fn)         __define_initcall(fn, 5)  
#define fs_initcall_sync(fn)        __define_initcall(fn, 5s)  
#define rootfs_initcall(fn)     __define_initcall(fn, rootfs)  
#define device_initcall(fn)     __define_initcall(fn, 6)  
#define device_initcall_sync(fn)    __define_initcall(fn, 6s)  
#define late_initcall(fn)       __define_initcall(fn, 7)  
#define late_initcall_sync(fn)      __define_initcall(fn, 7s)  
  
#define __initcall(fn) device_initcall(fn)  
```
我们来到`init\main.c`文件中可以找到函数do_initcalls

```c
static initcall_t *initcall_leves[] __initdata = {
	__initcall0_start,
	__initcall1_start,
	__initcall2_start,
	__initcall3_start,
	__initcall4_start,
	__initcall5_start,
	__initcall6_start,
	__initcall7_start,
	__initcall_end,
};
static void __init do_initcalls(void)
{
	int level;
	for(level = 0;level < ARRAY_SIZE(initcall_levels) -1; level++)
		do_initcall_level(level);
}
```
很明显do_initcalls中有一个for循环，那么此循环就是按照优先级顺序执行一些函数的。那么问题又来了，执行哪些函数？？我们看看do_initcalls这个名字。是不是initcall非常的眼熟？没错就是上面我们提到过的宏定义xxx_initcall里面就有initcall。
所以，我们先来解释一下这些宏有什么用
还是从我们最熟悉的地方module_init(fn)开始说起，其中fn是module_init的参数，fn是一个函数指针（函数名）。

`module_init(fn)---> __initcall(fn) ---> device_initcall(fn) ---> __define_initcall(fn, 6)`

通过上面的定义，我们把`module_init(test_init)`给替换如下内容:

```c
static initcall_t __initcall_test_init6 __used 
	__attribute__((__section__(".initcall6.init))) = test_init;
```
通过`__attribute__（__section__)`设置函数属性，也就是将test_init放在`.initcall6.init`段中。这个段在哪用？这就要涉及到链接脚本了。

大家可以到kernel目录arch中，根据自己的处理器平台找到对应的链接脚本。例如我现在的平台是君正m200（mips架构），可能大部分是arm架构。

在`arch/mips/kernel/vmlinux.lds`这个链接脚本里面有如下一段代码

```lds
init_begin = .;  
 . = ALIGN(4096); 
.init.text : AT(ADDR(.init.text) - 0) 
{ _sinittext = .; *(.init.text) *(.cpuinit.text) *(.meminit.text) _einittext = .; }  
 .init.data : AT(ADDR(.init.data) - 0) { *(.init.data) *(.cpuinit.data) *(.meminit.data) *(.init.rodata) *(.cpuinit.rodata) *(.meminit.rodata) . = ALIGN(32); __dtb_start = .; *(.dtb.init.rodata) __dtb_end = .; 
. = ALIGN(16);
 __setup_start = .; 
*(.init.setup) __setup_end = .;
 __initcall_start = .; *(.initcallearly.init) __initcall0_start = .;
 *(.initcall0.init) *(.initcall0s.init) __initcall1_start = .;
 *(.initcall1.init) *(.initcall1s.init) __initcall2_start = .;
 *(.initcall2.init) *(.initcall2s.init) __initcall3_start = .;
 *(.initcall3.init) *(.initcall3s.init) __initcall4_start = .; 
*(.initcall4.init) *(.initcall4s.init) __initcall5_start = .;
 *(.initcall5.init) *(.initcall5s.init) __initcallrootfs_start = .; 
*(.initcallrootfs.init) *(.initcallrootfss.init) __initcall6_start = .;
 *(.initcall6.init) *(.initcall6s.init) __initcall7_start = .; 
*(.initcall7.init) *(.initcall7s.init) __initcall_end = .;
 __con_initcall_start = .; 
*(.con_initcall.init) __con_initcall_end = .; __security_initcall_start = .; *(.security_initcall.init) __security_initcall_end = .;
 . = ALIGN(4); __initramfs_start = .; *(.init.ramfs) . = ALIGN(8); *(.init.ramfs.info) }  
 . = ALIGN(4);  
```
字符串使我们比较熟悉的：`__initcall6_start = .; *(.initcall6.init) *(.initcall6s.init)`。链接脚本里的东西看似很乱很难，其实是非常有逻辑有规律可循的，我们来简单解释下面一行的代码作用`initcall6_start = .; *(.initcall6.init) *(.initcall6s.init)`

其中__initcall6_start是一个符号，链接器用到的。`__initcall6_start = .;`，其中的 '.'符号是对当前地址的一个引用，也就说把当前的地址给了符号`__initcall6_start, *(.initcall6.init) *(.initcall6s.init)` 的意思是所有的`.initcall6.init`段和`.initcall6s.init`段的内容从`__initcall6_start`为起始地址开始链接。

```
.initcall0.init .initcall0s.init .initcall1.init .initcall1s.init …… .initcall7.init .initcall7s.init
```
这里面就有`0,0s,1,1s,2,2s …… 6,6s,7,7s`，也就是`__define_initcall(fn, id)`中的第二个参数 id。很显然这个id的值不是我们在调用module_init的时候传过去的。数字id `0~7`代表的是不同的优先级（0最高，module_init对应的优先级为6，所以一般我们注册的驱动程序优先级为6），链接脚本里面根据我们注册不同的id，将我们的函数fn放入对应的地址里面。根据上面的分析，test_init放在.initcall6.init段中。在kernel启动过程中，会调用do_initcalls函数一次调用我们通过xxx_initcall注册的各种函数，优先级高的先执行。所以我们通过module_init注册的函数在kernel启动的时候会被顺序执行。

start_kernel函数在`init/main.c`文件中，由于start_kernel本身功能也比较多，所以为了简介分析过程我把函数从start_kernel到do_initcalls的调用过程按照如下方式展现出来

```
start_kernel -> reset_init -> kernel_thread(kernel_init, NULL, CLONE_FS | CLONE_SIGHAND);  
                                                    |  
                                                    |->static int __ref kernel_init(void *unused)  
                                                        |  
                                                        |-> kernel_init_freeable( )  
                                                                |  
                                                                |-> do_basic_setup();  
                                                                        |  
                                                                        |——> do_initcalls();  
```

do_initcall_level函数原型如下：

```c
static void __init do_initcall_level(int level)
{
	initcall_t *fn;
	
	strcpy(initcall_command_line, saved_command_line);
	parse_args(initcall_level_names[level],
		initcall_command_line, __start___param,
		__stop___param - __start___param,
		level, level,
		&repair_env_string);
	for(fn = initcall_levels[level]; fn < initcall_levels[level+1]; fn++)
		do_one_initcall(*fn);
}
```