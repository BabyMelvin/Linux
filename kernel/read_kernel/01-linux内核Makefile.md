Linux 内核 Makefile

# 0.内核编译

1.打补丁

`patch`命令，-p是指需要忽略的目录层数。

```
patch -p1 < ./linux.jz2440.patch
```

2.配置

*  `make menuconfig`，手动配置所有 太复杂
*  使用默认配置，在这基础上进行修改
	*  `cd arch/arm/configs;ls`，然后配置`cd -;make s3c2410_deconfig`
*  使用厂家提供的配置文集那(将_config直接复制一份为`.config`，然后make menuconfig)
	*  make menuconfig 在s3c2410_deconfig的基础上进行修改,其中"高亮首字母按下对应的字母直接跳转到相应的目录上去"

3.编译:make

生成uImage:make uImage
uImage：头部+真正的内核，我们想编译一个内核给 UBOOT 用，则用 make uImage.
上电开发板，接上USB 线（装驱动）在UBOOT 命令中选：`[K]`

```
[k] Download Linux Kernel uImage
```
之后会等待通过USB 上传文件。用工具 “dnw.exe”,查看下源代码，看看[k]对应于哪个命令：在Cmd_menu.c 中查找。

```c
	case 'k':
		strcpy(cmd buf, "usbslave 1 0x300000000; nand erase kernel; nand write.jffs2 0x3000000 kernel $(filesize)");
		run_command(cmd_buf, 0);
		break;
```

首先上传数据 ：`usbslave 1 0x30000000`来接收 dnw.exe 发出来的数据，放到 0x3000
0000 地址处。
然后擦除内核分区 ：`nand erase kernel`收到数据后，接着擦除这个kernel 内核分区。
最后写到内核分区：`nand write.jffs2 0x30000000 kernel $(filesize)`
将原来接收到 0x3000 0000 处的文件烧到 kernel 分区去。
烧多大`$(filesize)`由这个宏定义，表示接收到的文件大小。

烧写完后，用`[b]`命令启动。
进入UBOOT 后，可以用`nand erase root`是删除文件系统。然后 boot 是UBOOT 启动内
核。

# 1.概述
从Linux内核2.6开始，Linux内核的编译采用Kbuild系统，这同过去的编译系统有很大的不同,尤其对于Linux内核模块的编译。在新的系统下，Linux编译系统会**两次扫描**Linux的Makefile：首先编译系统会读取Linux内核顶层的 Makefile，然后根据读到的内容第二次读取Kbuild的Makefile来编译Linux内核。

Makefile由五个部分组成:

* `Makefile`：根目录Makefile，它读取`.config文件`，并负责创建vmlinux（内核镜像）和modules（模块文件）.这编译内核或模块是，这个文件会被首先 读取，并根据读到的内容配置编译环境变量。对于内核或驱动开发人员来说，这个文件几乎不用任何修改。
* `.config`：内核配置文件（一般由make menuconfig生成）
* `arch/$(ARCH)/Makefile`：目标处理器的Makefile是系统对应平台的Makefile。Kernel Top Makefile会包含这个文件来指定平台相关信息。只有平台开发人员会关心这个文件。
* `scripts/Makefile.*`：所有kbuild Makefile的规则，它们包含了定义/规则等。
* `kbuild Makefiles`：每个子目录都有kbuild Makefile，它们负责生成built-in或模块化目标。（注意：kbuild Makefile是指使用kbuild结构的Makefile，内核中的大多数Makefile都是kbuild Makefile。）Kernel Makefile被解析完成后，Kbuild会读取相关的Kbuild Makefile进行内核或模块的编译。Kbuild Makefile有特定的语法指定哪些编译进内核中、哪些编译为模块、及对应的源文件是什么等。内核及驱动开发人员需要编写这个Kbuild Makefile文件。

# 2.kbuild文件
Kbuild Makefile的文件名不一定是Makefile，尽管推荐使用Makefile这个名字。 大多的Kbuild文件的名字都是Makefile。为了与其他Makefile文件相区别，你也可以指定Kbuild Makefile的名字为`Kbuild`。 而且如果“Makefile”和“Kbuild”文件同时存在，则Kbuild系统会使用“Kbuild”文件。
## 2.1 obj-y和obj-m
最简单的kbuild Makefile可以仅包含：

```
obj-$(CONFIG_FOO) += foo.o
```

其中`$(CONFIG_FOO)`可以等于y或m，它的值由`.config`文件给出。如果`$(CONFIG_FOO)`既不是y也不是m，那么该文件不会被编译和链接

* 当`$(CONFIG_FOO)`等于y时，上面语句等价于`obj-y += foo.o`，它告诉kbuild在当前目录下，有一个叫做foo.o的目标文件，它将从`foo.c`或则`foo.S`编译得到。
* 当`$(CONFIG_FOO)`等于m时，表示`foo.o`需要被编译成模块。

### 2.1.1 obj-y生成built-in.o

Kbuild编译所有的`$(obj-y)`文件，并调用`$(LD) -r`把所有这些文件合并到`built-in.o`文件。这个`built-in.o`会被上一级目录的Makefile使用，最终链接到vmlinux中。

### 2.1.2 目标由多个源文件编译得到
如果某个目标由多个源文件编译得到，那么可以通过`$(<module_name>-objs)`或`$(<module_name>-y)`把这些源文件告诉kbuild。Kbuild能够识别后缀`-objs`和`-y`，例如：

```
#drivers/isdn/i4l/Makefile
obj-$(CONFIG_ISDN) += isdn.o
isdn-objs := isdn_net_lib.o isdn_v110.o isdn_common.o
```
Kbuild会编译所有`$(isdn-objs)`中的对象，并调用`"$(LD) -r"`把它们链接成`isdn.o`文件。

下面是一个使用后缀`-y`的例子。后缀`-y`的好处是，可以使用`CONFIG_XXX`符号来决定是否加入某些源文件（`.o`从对应的`.c`或`.S`编译得到)：

```
#fs/ext2/Makefile

obj-$(CONFIG_EXT2_FS)        += ext2.o
ext2-y                       := balloc.o bitmap.o
ext2-$(CONFIG_EXT2_FS_XATTR) += xattr.o
```

### 2.1.3 调用子目录Makefile
Makefile只负责编译当前目录中的对象。子目录中的对象，由子目录中的Makefile负责。如何让make调用子目录中的Makefile？答案是把子目录包含到`obj-y`或`obj-m`中。例如：

```
#fs/Makefile
obj-$(CONFIG_EXT2_FS) += ext2/
```

## 2.2 lib-y和lib-m
在一个目录下，obj-y所列出的文件，将被编译成`built-in.o`文件，而lib-y或lib-m所列出的文件，将在当前目录下生成`lib.a`文件。

**注意**：一般`lib-y`或`lib-m`只用在`lib/`和`arch/*/lib`这两个目录中。

## 2.3 编译选项变量
### 2.3.1 ccflags-y、asflags-y、ldflags-y
这三个变量只在当前Makefile中有效。补充：`$(KBUILD_CFLAGS)`是定义在根目录Makefile中的变量，它适用于整个内核数。

### 2.3.2 subdir-ccflags-y、subdir-asflags-y
这两个变量作用于当前Makefile及其所有子目录。

### 2.3.3 `CFLAGS_$@`、`AFLAGS_$@`
这两个变量只在当前Makefile中有效.`$@`可以用来指定文件名，让不同文件可以使用不同的编译选项。例如：

```
# drivers/scsi/Makefile
CFLAGS_aha152x.o =   -DAHA152X_STAT -DAUTOCONF
CFLAGS_gdth.o    = # -DDEBUG_GDTH=2 -D__SERIAL__ -D__COM2__ /
	DGDTH_STATISTICS
CFLAGS_seagate.o =   -DARBITRATE -DPARITY -DSEAGATE_USE_ASM
```

## 2.4 `$(src)`、`$(obj)`、`$(kecho)`
`$(src)`指向当前Makefie所在目录的相对路径。
`$(obj)`指向用来保存目标文件的相对目录。示例如下：

```
#drivers/scsi/Makefile
$(obj)/53c8xx_d.h: $(src)/53c7,8xx.scr $(src)/script_asm.pl
$(CPP) -DCHIP=810 - < $< | ... $(src)/script_asm.pl
```
这里，`$(src)`等于`drivers/scsi/`，`$(obj)`同样也等于`drivers/scsi/`。

使用“`make -s`”命令时，只会输出警告和错误信息。`$(kecho)`能够将其后的内容输出到标准输出流（一般就是显示器），前提是没有使用“`make -s`”。示例如下：

```
#arch/blackfin/boot/Makefile
$(obj)/vmImage: $(obj)/vmlinux.gz
$(call if_changed,uimage)
@$(kecho) 'Kernel: $@ is ready'
```

## 2.5 `$(CC)`相关功能
### 2.5.1 `as-option`、`ld-option`、`cc-option`
当编译、链接文件时，`xx-opiton`可以用来检查当前使用的`$(CC)`是否支持给出的编译选项。如前者不支持，可使用后者。例如：

```
#arch/sh/Makefile
cflags-y += $(call as-option,-Wa$(comma)-isa=$(isa-y),)
```
如果当前的`$(CC)`不支持`-Wa$(comma)-isa=$(isa-y)`，那么可以使用第二个编译选项（这里为空）。

### 2.5.2 cc-option-yn
`cc-option-yn`用来检查`$(CC)`是否支持给出的编译选项。如支持，返回y，否则返回n。例如：

```
#arch/ppc/Makefile
biarch := $(call cc-option-yn, -m32)
aflags-$(biarch) += -a32
cflags-$(biarch) += -m32
```

### 2.5.3 cc-option-align
gcc在3.0之后改变了指定函数、循环等对齐选项的类型。

* `gcc < 3.00`时，`cc-option-align = -malign；`
* `gcc >= 3.00`时，`cc-option-align = -falign`。使用`$(cc-option-align)`来选择正确的前缀。例如：

```m
KBUILD_CFLAGS += $(cc-option-align)-functions=4
```

### 2.5.4 cc-version、cc-ifversion 、cc-fullversion
cc-version返回`$(CC`)版本。如`$(CC)`为gcc 3.41，那么`cc-version`返回`0341`。例如：

```
#arch/i386/Makefile
cflags-y += $(shell /
	if [ $(call cc-version) -ge 0300 ] ; then /
	echo "-mregparm=3"; fi ;)
```
`cc-ifversion`在版本符合条件的前提下返回最后一个参数。示例如下：

```
#fs/reiserfs/Makefile
ccflags-y := $(call cc-ifversion, -lt, 0402, -O1)
```
如果`$(CC)`版本低于4.2，那么`ccflags-y`将等于`-O1`。
`cc-fullversion`给出更详细的版本信息，例如：

```
#arch/powerpc/Makefile
$(Q)if test "$(call cc-fullversion)" = 	"040200" ; then /
	echo -n '*** GCC-4.2.0 cannot compile the 64-	bit powerpc ' ; /
	false ; /
	fi
```

### 2.5.5 cc-cross-prefix
`cc-cross-prefix`用于检查是否存在给定前缀的`$(CC)`，如存在，返回第一个匹配的前缀，否则返回空。如有多个前缀需要匹配，各前缀之间使用单个空格隔开。例如：

```
#arch/m68k/Makefile
ifneq ($(SUBARCH),$(ARCH))
	ifeq ($(CROSS_COMPILE),)
		CROSS_COMPILE := $(call cc-cross-prefix, m68k-linux-gnu-)
	endif
endif
```

# 3.编译外部模块
有时候我们需要在内核源代码数的外面编译内核模块，编译的基本命令是：

```
make -C $(KERNEL_DIR) M=`pwd` modules
```
我们可以把这个命令集成到Makefile里，这样我们就可以只输入“make”命令就可以了。

Makefile，它把Normal Makefile 和Kbuild  Makefile集成到一个文件中了。为了区别Kbuild Makefile 和Normal Makefile，这样我们改写Makefile为如下形式，并且添加Kbuild Makefile - “Kbuild”。

## 3.1 Makefile

```
ifneq ($(KERNELRELEASE),)
include "Kbuild"
else
KERNEL_DIR = /lib/modules/`uname -r`/build
MODULEDIR := $(shell pwd)


.PHONY: modules
default: modules
modules:
        make -C $(KERNEL_DIR)  M=$(MODULEDIR) modules
clean distclean:
        rm -f *.o *.mod.c .*.*.cmd *.ko
        rm -rf .tmp_versions
endif
```

## 3.2 Kbuild Makefile

```
MODULE_NAME = helloworld
$(MODULE_NAME)-objs := hello.o
obj-m   := $(MODULE_NAME).o
一般不需要在Makefile里包含如下代码,这样写完全是为了兼容老版本的Kbuild系统。KERNELRELEASE变量在Kernel Makefile里定义的，因此只有在第二次由Kbuild读取这个Makefile文件时才会解析到Kbuild的内容。 
ifneq ($(KERNELRELEASE),)
include "Kbuild"
else
...
endif
```

## 3.3 外部头文件
有时需要连接内核源代码外部的系统头文件，但Kbuild系统默认的系统头文件都在内核源代码内部，如何使用外部的头文件呢？这个可以借助于Kbuild 系统的特殊规则:

* EXTRA_CFLAGS 

`EXTRA_CFLAGS`可以给Kbuild系统添加外部系统头文件，
`EXTRA_CFLAGS += $(ext_include_path)`
一般外部头文件可能位于外部模块源文件的目录内，如何指定呢？这可以借助`$(src)`或`$(obj)`

* `$(src)`/`$(obj) `
	* `$(src)`是一个相对路径，它就是`Makefile`/`Kbuild`文件所在的路径。
	* 同样`$(obj)`就是编译目标保存的路径，默认就是源代码所在路径。
