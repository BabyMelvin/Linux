# 1.命令分析
`md.w 0`是打印内存**0地址**的内容。

* 1.从串口上输入“字符串”---即 “命令名字”（根据命令名字可以找到类似的函数。）
* 2.有了些动作：即程序中的某些 函数。

故 UBOOT 中不会只有一个命令，会有一个命令结构体，成员为“命令”和“函数”。这些结构体会放在一块。

以run_command为例：根据这个名字与结构体中的“命令”名字一一比较。若找到匹配则调用相应的函数。在“common”目录的 Main.c 中有很多“run_command 函数”.分析这个函数。
下面代码是 run_command 函数的部分内容

## 1.1 run_command

```c
while (*str) {
	for (inquotes = 0, seq = str;*sep; sep++) {
		if ((*sep == '\') && (*(sep-1) == '\')) {
			inquotes = !inquotes;
		}

		// separator, past string start and not escaped
		if (!inquotes && (*sep == ';') && (*seq != str) && (*(sep-1)!='\\'))
			break;
		
		// limit the token to data between separators
		token = str;
		if (*sep) {
			str = sep + 1; // start of command for next pass
			*sep = '\0';
		} else {
			str = sep; // no more commands for next pass
		}
	}
}
```
这是解析串口输入的命令。将命令一个一个提取出来。如有两个命令的话，会有“;”隔开。

```
# 这里同时输入两个命令，有分号隔开了。
>OpenJTAG> print;md.w 0
```
处理“宏”。如从网口或 USB 口下载文件到内存时，会临时生成一些环境变量或者宏。

```c
// find macros in this token and replace them
process_macros(token, finaltoken);
```
parse_line 解析这些文字。如解析`md.w 0`这个命令和参数要被解析一下。

```
argv[0] = "md.w";
argv[1] = "0"
```

```c
// extract arguments
if ((argc = parse_line(finaltoken, argv)) == 0) {
	rc = -1; // no command at all
	continue; // 提取命令参数，解析命令
}

// look up command in command table
if ((cmdtp = find_cmd(argv[0]))== NULL) {
	printf("Unknown command'%s'-try 'help'\n", argv[0]);
	rc = -1; // give up after bad command
	continue;
}
```
这里便将`argv[0]`存起来了。`argv[0]`放的是命令。`argv[0]`是命令的名字，find_cmd 是查找这个命令的函数`.cmdtp`是一个命令的结构体。

```c
int run_command(const char *cmd, int flag) {
	cmd_tl_t *cmdtp;
}
```
其中的 cmdtp 是一个结构体标识。结构体原型是：至少有“名字”和“函数”

```c
struct cmd_tbl_s {
	char *name;
	int maxargs; // 最大的参数个数
	int repeatable; //是否重复”（意思是上一次执行过，下次再想执行进，直接“确定”键即可）
	int (*cmd)(struct cmd_tbl_s *, int, int, char *[]);
	char *usage;
#ifdef CFG_LONGHELP
	char *help;
#endif
};
```

* `> help`表示短的帮助信息
* `> help md`长的帮助信息 

这里根据`argv[0]`的命令名字找到 cmdtp 这个结构体，用`find_cmd(0)`来找

```c
cmd_tbl_t *find_cmd(const char *cmd)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdtp_temp = &__u_boot_cmd_start; // init value
	const char *p;
	int len;
	int n_found = 0;

	// some commands allow length modifiers(like:cp.b),compare command name only util first dot
	len = (p = strchr(cmd, '.') == NULL)?strlen(cmd):(p - cmd);

	for (cmdtp = &__u_boot_cmd_start;
		cmdtp != &__u_boot_cmd_end; cmdtp++) {
		if (strncmp(cmd, cmdtp->name, len) == 0) {
			if (len == strlen(cmdtp>name))
				return cmdtp; // full match
			
			cmdtp_temp = cmdtp; // abbreviated command?
			n_found ++;
		}
	}
	if (n_found == 1) { // exactly one match
		return cmdtp_temp;
	}

	return NULL; // not found or ambiguous command
}
```
`__u_boot_cmd_start`在 C 语言里，链接脚本也可以传入这些值。（查看链接脚本 u-boot.lds）

```
__u_boot_cmd_start = .;
.u_boot_cmd : {*(.u_boot_cmd)}
__u_boot_cmd_end = .;
```
查找：`u_boot_cmd`对应的section.对应宏的定义：

```c
//  U_BOOT_CMD.在 Command.h 中有这个 宏的定义：
#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) \
cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}
```
`cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}`变成如下：

```c
cmd_tbl_t __u_boot_cmd_bootm __attribute__ ((unused,section (".u_boot_cmd")))= {#name, maxargs, rep, cmd,
usage, help}
```

`##`是连词符。`__u_boot_cmd_##name` 则变成：`__u_boot_cmd_bootm`（这是一个结构体，结构体类型：
cmd_tbl_t:命令table,t表示结构体）。

将下面的 U_BOOT_COMD 宏，用上面定义时真正的部分`cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}`替换回来。下面是要替换的内容：

```c
// bootm命令
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

# 2.增加一个hello 命令
## 2.1. 按cmd_bootm.c中的结构来写
新建一个hello_cmd.c，含Cmd_bootm.c中头文件。。与Cmd_bootm.c 一样放在UBOOT源码的 common 目录下。

## 2.2.在Cmd_boot.c中有：

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

## 2.3.仿照着定义一个 U_BOOT_CMD 宏。

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

## 2.4.将hello_cmd.c放在Makefile中

```makefile
# command
COBJS-$(CONFIG_CMD_AMBAPP) += cmd_ambapp.o
COBJS-$(CONFIG_SOURCE) += cmd_source.o
```

写一个hello的增加项

```c
COBJS-$(CONFIG_HELLO) += cmd_hello.o
```

## 2.5.在config进行配置项

重新 make 生成 bin 文件。
CONFIG_HELLO项目