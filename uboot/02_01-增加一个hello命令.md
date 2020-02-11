# 增加一个hello 命令

# 1. 按cmd_bootm.c中的结构来写
新建一个hello_cmd.c，含Cmd_bootm.c中头文件。。与Cmd_bootm.c 一样放在UBOOT 源
码的 common 目录下。

# 2.在Cmd_boot.c中有：

```c
int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf ("## Booting image at %08lx ...\n", addr);
	//后面也有返回值。
}
```
我们仿制成：加了参数打印。

```c
int do_hello (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	printf("Hello World!，%d\n",argc);
	for (i = 0; i < argc; i++) {
		printf("argv[%d]:%s\n",i,argv[i]);
	}

	return 0;
}
```

# 3.仿照着定义一个 U_BOOT_CMD 宏。

在Cmd_bootm.c中

```c
U_BOOT_CMD(
bootm, CFG_MAXARGS, 1, do_bootm,
"bootm - boot application image from memory\n",
"[addr [arg ...]]\n - boot application image stored in memory\n"
"\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
"\t'arg' can be the address of an initrd image\n"
#ifdef CONFIG_OF_FLAT_TREE
"\tWhen booting a Linux kernel which requires a flat device-tree\n"
"\ta third argument is required which is the address of the of the\n"
"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
"\tuse a '-' for the second argument. If you do not pass a third\n"
"\ta bd_info struct will be passed instead\n"
#endif
);
```

改成如下：

```c
U_BOOT_CMD(
hello, CFG_MAXARGS, 1, do_hello,
"hello - just for test\n",
"hello, long help ................\n"
);
```

完整代码如下：

```c
/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <u-boot/zlib.h>
#include <bzlib.h>
#include <environment.h>
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>

int do_hello (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	printf("Hello World!，%d\n",argc);
	for (i = 0; i < argc; i++) {
		printf("argv[%d]:%s\n",i,argv[i]);
	}

	return 0;
}

U_BOOT_CMD(
bootm, CFG_MAXARGS, 1, do_bootm,
"bootm - boot application image from memory\n",
"[addr [arg ...]]\n - boot application image stored in memory\n"
"\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
"\t'arg' can be the address of an initrd image\n"
#ifdef CONFIG_OF_FLAT_TREE
"\tWhen booting a Linux kernel which requires a flat device-tree\n"
"\ta third argument is required which is the address of the of the\n"
"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
"\tuse a '-' for the second argument. If you do not pass a third\n"
"\ta bd_info struct will be passed instead\n"
#endif
);
```

# 4.将hello_cmd.c放在Makefile中

```makefile
# command
COBJS-$(CONFIG_CMD_AMBAPP) += cmd_ambapp.o
COBJS-$(CONFIG_SOURCE) += cmd_source.o
```

写一个hello的增加项

```c
COBJS-$(CONFIG_HELLO) += cmd_hello.o
```

# 5.在config进行配置项

CONFIG_HELLO项目