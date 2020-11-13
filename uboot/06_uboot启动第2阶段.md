# 1.UBOOT 第二阶段
UBOOT 最终目标：启动内核。从 FLASH 读出内核---> 启动

## 1.1 cpu/arm920t/start.S

```c
_start_armboot: .word start_armboot
```

第二阶段开始。在 Board.c 中读代码

```c
/* Pointer is writable since we allocated a register for it */
gd = (gd_t *)(_armboot_start - CFG_MALLOC_LEN -sizeof(gd_t));

/* compiler optimization barrier need for GCC >= 3.4 */
__asm__ __volatile__ ("":::"memory");

memset((void *)gd, 0, sizeof(gd_t));
gd->bd = (bd_t *)((char *)gd-sizeof(bd_t));
memset(gd->bd, 0, sizeof(bd_t));

monitor_flash_len = _bss_start - _armboot_start;

// init_sequence函数指针数组
for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
    if ((*init_func_ptr)() != 0) {
            hang();
    }
}
```

`gd = (gd_t*)(_armboot_start - CFG_MALLOC_LEN - sizeof(gd_t));`是分配一个类型为`gd_t*`这样的结构体指针的内存空间给 gd，gd 是指向下面128 字节这块内存。

[uboot内存分布](image/06_01.jpg)

```c
// init_sequence函数指针数组
init_fnc_t *init_sequence[] = {
    cpu_init,       // basic cpu dependent setup
    board_init,     // basic board dependent setup
    dram_init,      // configure available RAM banks
    mem_malloc_init,// dependant on dram_init
    interrupt_init, // set up exceptions
    timer_init,
    serial_init,
    env_init,       // initialize environment
    init_baudrate,  // initialize baudrate settings
    serial_init,    // serial communication setup
    display_banner,
    display_dram_config,
    NULL,
};
```
CPU 初始化，不能关。目标是从 FLASH 读出内核启动内核。

# 2.单板初始化

```c
int board_init(void)
{
    S3C24X0_CLOCK_POWER * const clk_power = S2C24X0_GetBase_CLOCK_POWER();
    S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

    // set up the I/O ports
    gpio->GPACON = 0x007FFFFF;
    gpio->GPBCON = 0x00044555;
    gpio->GPBUP  = 0x007FFFFFF;
    gpio->GPCCON = 0x000007FFF;
    gpio->GPCUP  = 0x007FFFFFF;
    gpio->GPDCON = 0x007FFFFFF;
    gpio->GPDUP  = 0x007FFFFFF;
    gpio->GPECON = 0x007FFFFFF;
    gpio->GPEUP  = 0x007FFFFFF;
    gpio->GPFCON = 0x007FFFFFF;
    gpio->GPFUP  = 0x007FFFFFF;
    gpio->GPGCON = 0x007FFFFFF;
    gpio->GPGUP  = 0x007FFFFFF;
    gpio->GPGCON = 0x007FFFFFF;
    gpio->GPGUP  = 0x007FFFFFF;

    /* support both of S3C2410 and S3C2440 */
    if (isS32410) {
        /* arch number of SMDK2410-Board*/
        gd->bd->bi_arch_number = MACH_TYPE_SMDK2410;
    } else {
        /* arch number of SMDK2440-Board */
        gd->bd->bi_arch_number = MACH_TYPE_S3C2440;
    }

    /* address of boot parameters */
    gd->bd->bi_boot_params = 0x30000100;
    return 0;
}
```
以上是 2440 相关管脚的初始化。其中有“机器 ID”的设置。其中有两个如下的设置：

```c
/* arch number of SMDK2410-Board */
gd->bd->bi_arch_number = MACH_TYPE_S3C2440; /* 值为193 */
```

gd 指向 128K 的内存处。图“U-BOOT 内存使用情况”。机器 ID，这个机器 ID 在讲解内核时用的着。

```c
/*address of boot parameters*/
gd->bd->bi_boot_params = 0x30000100;
```
这是启动参数，就是启动内核的时候会传些参数。这些参数存在 0x30000100 这个地址。
讲解内核时便知道以上两个参数的设置意义。

# 3.要从 FLASH 读出内核

初始化 FLASH则要：

1.支持 FLASH 的读。

NOR 的读和内存的读一样。不需要做什么。

2.这开发方便增加：写

NOR 要写首先识别它是哪种 FLASH。
NAND 的也要先初始化识别它是哪种 FLASH。

Board.c 中有如下初始化：NOR_FLASH 的初始化

```c
#ifndef CFG_NO_FLASH
    /* configure available FLASH banks */
    size = flash_init();
    display_flash_config(size);
#endif
```
这是对 NOR_FLASH 的初始化。flash_init()会识别是哪种 NOR_FLASH。

UBOOT 是单片机的程序，从内存上分配些空间等函数都得自已实现：

```c
/* armboot_start is defined in the board-special linker script */
mem_malloc_init(_armboot_start - CFG_MALLOC_LEN);
```

mem_malloc_init 是从内存上分配空间。它对应内存上的如下块：192K 空间(CFG_MALLOC_LEN)

这个函数在这块内存上实现了 堆的分配和释放。

# 4.NAND 的初始化

```c
#if (CONFIG_COMMAND & CFG_CMD_NAND)
    puts("NAND: ");
    nand_init();
#endif
```

nand操作实现:

```c
void nand_init(void)
{
    int i;
    unsigned int size = 0;

    for (i = 0; i < CFG_MAX_NAND_DEVICE; i++) {
        nand_init_chip(&nand_info[i], &nand_chip[i], base_address[i]);
        size += nand_info[i].size;
        if (nand_curr_device == -1)
            nand_curr_device = i;
    }
    printf("%lu MiB\n", size/(1024 * 1024));

#ifdef CFG_NAND_SELECT_DEVICE
    /* select the chip in the board/cpu specific driver */
    board_nand_select_device(nand_info[nand_cur_device].priv, nand_curr_device);
#endif
}

static void nand_init_chip(struct mtd_info *mtd, struct nand_chip *nand, ulong base_addr)
{
    mtd->priv = nand;
    nand->IO_ADDR_R = nand->IO_NAND_W = (void __iomem *)base_addr;
    board_nand_init(nand);

    if (nand_scan(mtd, 1) == 0) {
        if (!mtd->name)
            mtd->name = (char *)default_nand_name;
    } else
        mtd->name = NULL;
}
```
有了以上几个函数，UBOOT 已有能力读写 flash 了。

# 5.环境变量
环境变量：`env_relocate()`.单板启动后，可以按“4”进入 UBOOT 命令中，printenv 命令会显示“环境变量”。一个变量的等于号后面是变量的值。

环境变量从哪里来：

* 1.代码里默认写死的。
* 2.在 FLASH 上的，可以修改，则要保存。
* 
UBOOT 是启动后先看 FLASH 上看是否有可用的环境变量，若没有则用“默认”的环境变量。 

```c
/* initialize environment */
env_relocate();
```
这是环境变量的初始化。
在 UBOOT 中输入 print 会显示当前的环境变量信息。

```c
if (!PreLoadedONRAM) {
    /* enable exceptions */
    enable_interrupts();
    usb_init();
}
```
“PreLoadedONRAM 意思是指事先装载进内存中去的”。若用调试器下载的话，PreLoadedONRAM这个值就会被值成 1

```c
/* main_loop() can return to retry autoboot, if so just run it again */
for (;;) {
    main_loop();
}
/* NOTREACHED-no way out of command loop except booting*/
```
这里有个 死循环 。

以上经过如下：`start_armboot----flash_init----nand_init----main_loop(主循环)`

在 UBOOT 上输入命令等等，都是在这个主循环之中。我们输入什么，UBOOT 就解析什么。如：UBOOT 中的环境变量中有个 bootdelay

```c
#if define(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
    s = getenv("bootdelay");
    bootdelay = s ? (int)simple_strtol(s, NULL, 10):CONFIG_BOOTDELAY;

    debug("### main_loop entered: bootdelay = %d\n\n", bootdelay);
#endif
```
这是在 main_loop 主循环中，**进入主循环后等待我们输入**，上面这个是 bootdelay.`getenv()`获得环境变量，这里将“bootdelay”取出来。`bootdelay=10`这里取的是 10，是字符串。转换成数字。

> s = geten("bootcmd)

想知道是怎么启动，有一个 `s = getenv("bootcmd")`,`boot 是启动，cmd 是命令。即“启动命令”

```c
s = getenv("bootcmd");
debug("### main_loop:bootcmd=\"%s\:\n", s ? s : "<UNDEFINED>");
if (bootdelay >= 0 && s && !abortboot(bootdelay) {
    int prev = disable_ctrl(1); // disable Control C checking
})
```
这是倒数计时。如果在这个倒数计时没到达到 0 之前，没有输入空格键，就会打印下面的信息：

```c
#ifndef CFG_HUSH_PARSER
printf("Booting Linux...\n");
run_command(s, 0);
```

`run_command(s,0)`这个命令参数哪里来的，就是上面`s = getenv("bootcmd")`这样子来的。
从这里可见，如何从 FLASH 读出内核，如何启动内核，取决于`s = getenv("bootcmd")`这条命令。bootcmd
是环境变量。在 UBOOT 启动后，这个环境变量 bootcmd 是如下内容：

```
> print
bootcmd=nand read.jffs2 0x30007FC0 kernel; bootm 0x30007FC0
```

# 6.uboot 启动内核
## 6.1 从 flash 上读出内核。
从 bootcmd 可知：用命令`nand read.jffs2`将内核读到内存0x30007FC0 处。从哪里读？从kernel读，这是个**分区**.

## 6.2 启动
第二条命令：`bootm 0x30007FC0`.可知内核的启动 依赖bootcmd这个环境变量。这个环境变量里定义了两个命令，一个是从 FLASH 上读出内核，一个是启动内核。

如果倒数计时到 0 之有，按下了空格，则是执行接着的如下：

```c
#ifdef CONFIG_MENUKEY
    if (menukey == CONFIG_MENUKEY) {
        s = getenv("menucmd");
        if (s) {
            #ifndef CONFIG_HUSH_PARASER
                run_command(s, 0);
                // 执行run_command("menu", 0);
            #else
                parse_string_outer(s, FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP);
            #endif
        }
    }
```
走到run_command中。这个 menu 就是有个菜单的原因。没有修改的 UBOOT 没有这个菜单。接着又是一个 死循环。

```c
for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
    if (rc >= 0) {
        // saw enough of a valid command to restart the timeout
        reset_cmd_timeout();
    }
#endif
    len = readline(CFG_PROMPT);
    flag = 0; // assume no special flags for now
    if (len > 0) {
        strcmpy(lastcommand, console_buffer);
    } else if (len == 0) {
        flag |= CMD_FLAG_REPEAT;
    } else if (len == -2) {
        // -2 means timed out, retry autoboot
        puts("\n Timed out waiting for command\n");
    #ifdef CONFIG_RESET_TO_RETRY
        // reinit board to run initialize code again
        do_reset(NULL, 0, 0, NULL);
    #else
        return; // retry autoboot
    #endif

        if(len == -1)
            puts("<INTERRUPT>\n");
        else
            rc = run_command(lastcommand, flag);
        
        if (rc <= 0) {
            // invalid command or not repeatable, forget it
            lastcommand[0] = 0;
        }
}
```
这个循环表现在如下位置。黑色处就是这个 循环。它是等待读取串口`readline(CFG_PROMPT);`的输入。会解析我们的输入。读取串口输入。然后运行`run_command(lastcommand, flag);`我们启动内核时，也是一个 `run_command(s,0)`。所以 UBOOT 的核心就是这些命令。

## 6.3 小结

1. 启动内核：

```c
s = getenv("bootcmd"); //获取环境变量。
run_command(s ,)
```

2. 在 u-boot 控制界面。

readline 读入串口的数据。run_command()。UBOOT 的核心就是这些"命令" run_commnd().故只有分析了这些命令的实现之后，才能明白内核的加载与启动。