# 1.U-Boot编译命令
对于mini2440开发板，编译U-Boot需要执行如下的命令：

```
$make mini2440_config
$make all
```
使用上面的命令编译U-Boot，编译生成的所有文件都保存在源代码目录中。为了保持源代码目录的干净，可以使用如下命令将编译生成的文件输出到一个外部目录，而不是在源代码目录中，下面的2种方法都将编译生成的文件输出到 `/tmp/build`目录：

```
$export BUILD_DIR=/tmp/build
$make mini2440_config
$make all
```
或
```
$make O=/tmp/build mini2440_config
$make all
```
为了简化分析过程，方便读者理解，这里主要针对第一种编译方式（目标输出到源代码所在目录）进行分析。
uboot复杂项目，不能直接编译，先配置才能编译 `make x210_sd_config`，配置完成就可以`make -j4`

# 2.U-Boot配置、编译、连接过程
U-Boot开头有一些跟主机软硬件环境相关的代码，在每次执行make命令时这些代码都会被执行一次。

## 2.1 U-Boot配置过程
### 2.1.1 定义主机系统架构

```makefile
HOSTARCH:=$(shell uname -m | \
       sed -e s/i.86/i386/ \
           -e s/sun4u/sparc64/ \
           -e s/arm.*/arm/ \
           -e s/sa110/arm/ \
           -e s/powerpc/ppc/ \
           -e s/ppc64/ppc/ \
           -e s/macppc/ppc/)
```
`sed –e`表示后面跟的是一串命令脚本，而表达式`s/abc/def/`表示要从标准输入中，查找到内容为“abc”的，然后替换成“def”。其中“abc”表达式用可以使用`.`作为通配符。

命令`uname –m`将输出主机CPU的体系架构类型。作者的电脑使用Intel Core2系列的CPU，因此`uname –m`输出“i686”。 “i686”可以匹配命令`sed -e s/i.86/i386/`中的“i.86”，因此在作者的机器上执行Makefile，HOSTARCH将被设置成“i386” 。

### 2.1.2 定义主机操作系统类型

```makefile
HOSTOS:= = $(shell uname -s | tr '[:upper:]' '[:lower:]' | \
           sed -e 's/\(cygwin\).*/cygwin/')
```
 `uname –s`输出主机内核名字，作者使用Linux发行版Ubuntu9.10，因此`uname –s`结果是“Linux”。`tr '[:upper:]' '[:lower:]'`作用是将标准输入中的所有大写字母转换为响应的小写字母。因此执行结果是将HOSTOS 设置为“linux”。

### 2.1.3 定义执行shell脚本的shell

```makefile
# Set shell to bash if possible,otherwise fall back to sh
# echo作为返回值给SHELL
SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
       else if [ -x /bin/bash ]; then echo /bin/bash; \
       else echo sh; fi; fi)
```
`"$$BASH"`的作用实质上是生成了字符串`$BASH`（前一个`$`号的作用是指明第二个`$`是普通的字符）。若执行当前Makefile的shell中定义了`$BASH`环境变量，且文件`$BASH`是可执行文件，则SHELL的值为`$BASH`。否则，若`/bin/bash`是可执行文件，则SHELL值为`/bin/bash`。若以上两条都不成立，则将“sh”赋值给`SHELL`变量。

#### 2.1.4 设定编译输出目录

```makefile
ifdef O
ifeq ("$(origin O)", "command line")
BUILD_DIR := $(O)
endif
endif
```
函数`$( origin, variable)`输出的结果是一个字符串，输出结果由变量variable定义的方式决定

* 若variable在命令行中定义过，则origin函数返回值为"command line"。
* 假若在命令行中执行了`“export BUILD_DIR=/tmp/build”`的命令，则`$(origin O)`值为“command line”，而BUILD_DIR被设置为`/tmp/build`。

```makefile
ifneq ($(BUILD_DIR),)
saved-output := $(BUILD_DIR)
# Attempt to create a output directory.
$(shell [ -d ${BUILD_DIR} ] || mkdir -p ${BUILD_DIR})
```
若`${BUILD_DIR}`表示的目录没有定义，则创建该目录。

```makefile
# Verify if it was successful.
BUILD_DIR := $(shell cd $(BUILD_DIR) && /bin/pwd)
$(if $(BUILD_DIR),,$(error output directory "$(saved-output)" does not exist))
endif #ifneq($(BUILD_DIR),)
```
 若`$(BUILD_DIR)`为空，则将其赋值为当前目录路径（源代码目录）。并检查`$(BUILD_DIR)`目录是否存在。

```makefile
OBJTREE:= $(if $(BUILD_DIR),$(BUILD_DIR),$(CURDIR))
SRCTREE:= $(CURDIR)
TOPDIR := $(SRCTREE)
LNDIR  := $(OBJTREE)
… …
MKCONFIG := $(SRCTREE)/mkconfig
… …
ifneq ($(OBJTREE),$(SRCTREE))
obj := $(OBJTREE)/
src := $(SRCTREE)/
else
obj :=
src :=
endif
```
CURDIR变量指示Make当前的工作目录，由于当前Make在U-Boot顶层目录执行Makefile，因此CURDIR此时就是U-Boot顶层目录。

执行完上面的代码后

* SRCTREE:src变量就是U-Boot代码顶层目
* OBJTREE:obj变量就是输出目录
* 若没有定义BUILD_DIR环境变量，则SRCTREE，src变量与OBJTREE，obj变量都是U-Boot源代码目录。
* MKCONFIG则表示U-Boot根目录下的**mkconfig脚本**。

## 2.2 make mini2440_config命令执行过程
下面分析命令`make mini2440_config`执行过程，为了简化分析过程这里主要分析将编译目标输出到源代码目录的情况

```makefile
# 语法:$(string_text:term_text=replace_text)
mini2440_config :      unconfig
       @$(MKCONFIG) $(@:_config=) arm arm920t mini2440 samsung s3c24x0
```
其中的依赖“unconfig”定义如下：

```makefile
unconfig:
    @rm -f $(obj)include/config.h $(obj)include/config.mk \
    $(obj)board/*/config.tmp $(obj)board/*/*/config.tmp \
    $(obj)include/autoconf.mk $(obj)include/autoconf.mk.dep
```
其中`@`的作用是执行该命令时不在shell显示。“obj”变量就是编译输出的目录，因此“unconfig”的**作用**就是清除上次执行`make *_config`命令生成的配置文件（如`include/config.h`，`include/config.mk`等）。

 `$(MKCONFIG)`在上面指定为`“$(SRCTREE)/mkconfig”`。`$(@:_config=)`为将传进来的所有参数中的`_config`替换为`空`（其中`“@”`指规则的目标文件名，在这里就是`“mini2440_config ”`。
 
 * `$(text:patternA=patternB)`，这样的语法表示把text变量每一个元素中结尾的patternA的文本替换为patternB，然后输出） 。因此`$(@:_config=)`的作用就是将mini2440_config中的_config去掉，得到mini2440。

因此`@$(MKCONFIG) $(@:_config=) arm arm920t mini2440 samsung s3c24x0`实际上就是执行了如下命令：

```makefile
./mkconfig mini2440 arm arm920t mini2440 samsung s3c24x0
```
即将`mini2440 arm arm920t mini2440 samsung s3c24x0`作为参数传递给当前目录下的**mkconfig脚本**执行。在mkconfig脚本中给出了mkconfig的用法：

```makefile
# Parameters:  Target  Architecture  CPU  Board [VENDOR] [SOC]
```
因此传递给mkconfig的参数的意义分别是：

* mini2440：Target（目标板型号）
* arm：Architecture （目标板的CPU架构）
* arm920t：CPU （具体使用的CPU型号）
* mini2440：Board
* samsung：VENDOR（生产厂家名）
* s3c24x0：SOC

下面再来看看mkconfig脚本到底做了什么。
### 2.2.1 确定开发板名称BOARD_NAME

在mkconfig脚本中有如下代码：

```bash
APPEND=no      # no表示创建新的配置文件，yes表示追加到配置文件中
BOARD_NAME=""  # Name to print in make output
TARGETS=""
while [ $# -gt 0 ] ; do
    case "$1" in
    --) shift ; break ;;
    -a) shift ; APPEND=yes ;;
    -n) shift ; BOARD_NAME="${1%%_config}" ; shift ;;
    -t) shift ; TARGETS="`echo $1 | sed 's:_: :g'` ${TARGETS}" ; shift ;;
    *)  break ;;
    esac
done
[ "${BOARD_NAME}" ] || BOARD_NAME="$1" 
```
 
 * 环境变量`$#`表示传递给脚本的参数个数，这里的命令有6个参数，因此`$#`是6 。
 *　shift的作用是使`$1=$2，$2=$3，$3=$4….`，而原来的`$1`将丢失。因此while循环的作用是，依次处理传递给mkconfig脚本的选项。由于我们并没有传递给mkconfig任何的选项，因此while循环中的代码不起作用。
最后将BOARD_NAME的值设置为`$1`的值，在这里就是“mini2440”。

### 2.2.2检查参数合法性

```bash
[ $# -lt 4 ] && exit 1
[ $# -gt 6 ] && exit 1

if [ "${ARCH}" -a "${ARCH}" != "$2" ]; then
       echo "Failed: \$ARCH=${ARCH}, should be '$2' for ${BOARD_NAME}" 1>&2
       exit 1
fi
```
上面代码的作用是检查参数个数和参数是否正确，参数个数少于4个或多于6个都被认为是错误的。

### 2.2.3 创建到目标板相关的目录的链接

```bash
#
# Create link to architecture specific headers
#

if [ "$SRCTREE" != "$OBJTREE" ] ; then     #若编译目标输出到外部目录，则下面的代码有效
       mkdir -p ${OBJTREE}/include
       mkdir -p ${OBJTREE}/include2
       cd ${OBJTREE}/include2
       rm -f asm
       ln -s ${SRCTREE}/include/asm-$2 asm
       LNPREFIX="http://www.cnblogs.com/include2/asm/"
       cd ../include
       rm -rf asm-$2
       rm -f asm
       mkdir asm-$2
       ln -s asm-$2 asm
else              
       cd ./include
       rm -f asm
       ln -s asm-$2 asm
fi
```
若将目标文件设定为输出到源文件所在目录，则以上代码在include目录下建立了到asm-arm目录的符号链接asm。其中的`ln -s asm-$2 asm`即`ln -s asm-arm asm`。

```bash
rm -f asm-$2/arch
if [ -z "$6" -o "$6" = "NULL" ] ; then
       ln -s ${LNPREFIX}arch-$3 asm-$2/arch
else
       ln -s ${LNPREFIX}arch-$6 asm-$2/arch
fi
```
建立符号链接`include/asm-arm/arch` ，若$6（SOC）为空，则使其链接到`include/asm-arm/arch-arm920t`目录，否则就使其链接到`include/asm-arm/arch-s3c24x0`目录。（事实上`include/asm-arm/arch-arm920t`并不存在，因此$6是不能为空的，否则会编译失败）

```bash
if [ "$2" = "arm" ] ; then
       rm -f asm-$2/proc
       ln -s ${LNPREFIX}proc-armv asm-$2/proc
fi
```
 若目标板是arm架构，则上面的代码将建立符号连接`include/asm-arm/proc`，使其链接到目录proc-armv目录。

**建立以上的链接的好处**：编译U-Boot时直接进入链接文件指向的目录进行编译，而不必根据不同开发板来选择不同目录。

### 2.2.4 构建include/config.mk文件

```bash
#
# Create include file for Make
#

echo "ARCH   = $2" >  config.mk
echo "CPU    = $3" >> config.mk
echo "BOARD  = $4" >> config.mk
[ "$5" ] && [ "$5" != "NULL" ] && echo "VENDOR = $5" >> config.mk
[ "$6" ] && [ "$6" != "NULL" ] && echo "SOC    = $6" >> config.mk
```
上面代码将会把如下内容写入文件`inlcude/config.mk`文件：

```bash
ARCH   = arm
CPU    = arm920t
BOARD  = mini2440
VENDOR = samsung
SOC    = s3c24x0
```

### 2.2.5 指定开发板代码所在目录

```bash
# Assign board directory to BOARDIR variable

if [ -z "$5" -o "$5" = "NULL" ] ; then
    BOARDDIR=$4
else
    BOARDDIR=$5/$4
fi
```
以上代码指定board目录下的一个目录为当前开发板专有代码的目录。若`$5（VENDOR）`为空则BOARDDIR设置为`$4（BOARD）`，否则设置为`$5/$4`（VENDOR/BOARD）。在这里由于`$5`不为空，因此BOARDDIR被设置为`samsung/mini2440`。

### 2.2.6 构建`include/config.h`文件

```bash
#
# Create board specific header file
#

if [ "$APPEND" = "yes" ] # Append to existing config file
then
       echo >> config.h
else
       > config.h            # Create new config file
fi

echo "/* Automatically generated - do not edit */" >>config.h
for i in ${TARGETS} ; do
       echo "#define CONFIG_MK_${i} 1" >>config.h ;
done

cat << EOF >> config.h
#define CONFIG_BOARDDIR board/$BOARDDIR
#include <config_defaults.h>
#include <configs/$1.h>
#include <asm/config.h>
EOF
exit 0
```

### 2.2.7 总结
下面总结命令`make mini2440_config`执行的结果（仅针对编译目标输出到源代码目录的情况）：

（1）创建到目标板相关的文件的链接

```bash
ln -s asm-arm asm
ln -s arch-s3c24x0 asm-arm/arch
ln -s proc-armv asm-arm/proc
```

（2）创建`include/config.mk`文件，内容如下所示：

```makefile
ARCH   = arm
CPU    = arm920t
BOARD  = mini2440
VENDOR = samsung
SOC    = s3c24x0
```

（3）创建与目标板相关的文件`include/config.h`，如下所示：

```bash
/* Automatically generated - do not edit */
#define CONFIG_BOARDDIR board/samsung/mini2440
#include <config_defaults.h>
#include <configs/mini2440.h>
#include <asm/config.h>
```

## 2.3  make all命令执行过程
若没有执行过`“make <board_name>_config”`命令就直接执行`“make all”`命令则会出现如下的才错误信息，然后停止编译：

```
System not configured - see README
```
U-Boot是如何知道用户没有执行过`“make <board_name>_config”`命令的呢？阅读U-Boot源代码就可以发现了，Makefile中有如下代码：

```bash
# sinclude等价于-include，不存在不报错
ifeq ($(obj)include/config.mk,$(wildcard $(obj)include/config.mk)) # config.mk存在
all: 
	sinclude $(obj)include/autoconf.mk.dep
	sinclude $(obj)include/autoconf.mk
	… …
else        # config.mk不存在
	… …
    @echo "System not configured - see README" >&2
    @ exit 1
    … …
endif      # config.mk
```
若`include/config.mk`文件存在，则`$(wildcard $(obj)include/config.mk)`命令执行的结果是`“$(obj)include/config.mk”`展开的字符串，否则结果为空。由于`include/config.mk`是`“make <board_name>_config”`命令执行过程生成的，若从没有执行过`“make <board_name>_config”`命令则`include/config.mk`必然不存在。因此Make就执行else分支的代码，在输出“System not configured - see README”的信息后就返回了。

下面再来分析`make all`命令正常执行的过程，在Makefile中有如下代码：

### 2.3.1 `include/autoconf.mk`生成过程 

```makefile
all:
	sinclude $(obj)include/autoconf.mk.dep
	sinclude $(obj)include/autoconf.mk
```
 `include/autoconf.mk`文件中是与**开发板相关的一些宏定义**，在Makefile执行过程中需要根据某些宏来确定执行哪些操作。下面简要分析`include/autoconf.mk`生成的过程，`include/autoconf.mk`生成的规则如下：

```makefile
$(obj)include/autoconf.mk: $(obj)include/config.h
@$(XECHO) Generating $@ ; \
       set -e ; \
       : Extract the config macros ; \
       $(CPP) $(CFLAGS) -DDO_DEPS_ONLY -dM include/common.h | \
              sed -n -f tools/scripts/define2mk.sed > $@.tmp && \
       mv $@.tmp $@
```
`include/autoconf.mk`依赖于`make <board_name>_config`命令生成的`include/config.h`。因此执行`make <board_name>_config`命令后再执行`make all`将更新`include/autoconf.mk`。

编译选项`-dM`的作用是输出`include/common.h`中定义的所有宏。根据上面的规则，编译器提取`include/common.h`中定义的宏，然后输出给`tools/scripts/define2mk.sed`脚本处理，处理的结果就是`include/autoconf.mk`文件。其中`tools/scripts/define2mk.sed`脚本的主要完成了在`include/common.h`中查找和处理以`“CONFIG_”`开头的宏定义的功能。

 `include/common.h`文件包含了`include/config.h`文件，而`include/config.h`文件又包含了`config_defaults.h`，`configs/mini2440.h`，`asm/config.h`文件。因此`include/autoconf.mk`实质上就是`config_defaults.h`，`configs/mini2440.h`，`asm/config.h`三个文件中`“CONFIG_”`开头的有效的宏定义的集合。

下面接着分析Makefile的执行。

```makefile
# load ARCH, BOARD, and CPU configuration
include $(obj)include/config.mk
export    ARCH CPU BOARD VENDOR SOC
```
将`make mini2440_config`命令生成的`include/config.mk`包含进来。

```makefile
# 若主机架构与开发板结构相同，就使用主机的编译器，而不是交叉编译器
ifeq ($(HOSTARCH),$(ARCH))
	CROSS_COMPILE ?=
endif
```
若主机与目标机器体系架构相同，则使用gcc编译器而不是交叉编译器。

```makefile
# load other configuration
include $(TOPDIR)/config.mk
```
最后将U-Boot顶层目录下的config.mk文件包含进来，该文件包含了对**编译的一些设置**。下面对U-Boot顶层目录下的config.mk文件进行分析：

### 2.3.2 config.mk文件执行过程
#### 2.3.2.1 设置obj与src

在U-Boot顶层目录下的config.mk文件中有如下代码：

```makefile
ifneq ($(OBJTREE),$(SRCTREE))
	ifeq ($(CURDIR),$(SRCTREE))
		dir :=
	else
		dir := $(subst $(SRCTREE)/,,$(CURDIR))
	endif
obj := $(if $(dir),$(OBJTREE)/$(dir)/,$(OBJTREE)/)
src := $(if $(dir),$(SRCTREE)/$(dir)/,$(SRCTREE)/)
$(shell mkdir -p $(obj))
else
obj :=
src :=
endif
```
由于目标输出到源代码目录下，因此执行完上面的代码后，src和obj都是空。

#### 2.3.2.2 设置编译选项

```makefile
PLATFORM_RELFLAGS =
PLATFORM_CPPFLAGS =          #编译选项
PLATFORM_LDFLAGS =           #连接选项
```
用这3个变量表示**交叉编译器的编译选项**，在后面Make会检查交叉编译器支持的编译选项，然后将适当的选项添加到这3个变量中。

```makefile
#
# Option checker (courtesy linux kernel) to ensure
# only supported compiler options are used
#

cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
              > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)
```
变量CC和CFLAGS在后面的代码定义为**延时变量**，其中的CC即`arm-linux-gcc`。

函数`cc-option`用于检查编译器CC是否支持某选项。将2个选项作为参数传递给cc-option函数，该函数调用CC编译器检查参数1是否支持，若支持则函数返回参数1，否则返回参数2 （因此CC编译器必须支持参数1或参数2，若两个都不支持则会编译出错）。可以像下面这样调用cc-option函数，并将支持的选项添加到FLAGS中：

```makefile
FLAGS +=$(call cc-option,option1,option2)
```

#### 2.3.2.3 指定交叉编译工具

```makefile
#
# Include the make variables (CC, etc...)
#

AS  = $(CROSS_COMPILE)as
LD  = $(CROSS_COMPILE)ld
CC  = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
AR  = $(CROSS_COMPILE)ar
NM  = $(CROSS_COMPILE)nm
LDR      = $(CROSS_COMPILE)ldr
STRIP    = $(CROSS_COMPILE)strip
OBJCOPY  = $(CROSS_COMPILE)objcopy
OBJDUMP  = $(CROSS_COMPILE)objdump
RANLIB   = $(CROSS_COMPILE)RANLIB
```
对于arm开发板，其中的CROSS_COMPILE在`lib_arm/config.mk`文件中定义：

```makefile
CROSS_COMPILE ?= arm-linux-
```
 因此以上代码指定了使用前缀为“arm-linux-”的编译工具，即arm-linux-gcc，arm-linux-ld等等。

#### 2.3.2.4 包含与开发板相关的配置文件

```makefile
# Load generated board configuration
sinclude $(OBJTREE)/include/autoconf.mk

ifdef ARCH
sinclude $(TOPDIR)/lib_$(ARCH)/config.mk   # include architecture dependend rules
endif
```
`$(ARCH)`的值是“arm”，因此将`“lib_arm/config.mk”`包含进来。`lib_arm/config.mk`脚本指定了交叉编译器，添加了一些跟CPU架构相关的编译选项，最后还指定了`cpu/arm920t/u-boot.lds`为U-Boot的连接脚本。

```makefile
ifdef CPU
sinclude $(TOPDIR)/cpu/$(CPU)/config.mk             # include  CPU specific rules
endif
```

`$(CPU)`的值是“arm920t”，因此将`“cpu/arm920t/config.mk”`包含进来。这个脚本主要设定了跟arm920t处理器相关的编译选项。

```makefile
ifdef SOC
sinclude $(TOPDIR)/cpu/$(CPU)/$(SOC)/config.mk       # include  SoC  specific rules
endif
```

```makefile
ifdef SOC
sinclude $(TOPDIR)/cpu/$(CPU)/$(SOC)/config.mk       # include  SoC  specific rules
endif
```
`$(SOC)`的值是`s3c24x0`，因此Make程序尝试将`cpu/arm920t/s3c24x0/config.mk`包含进来，而这个文件并不存在，但是由于用的是`“sinclude”`命令，所以并不会报错。

```makefile
ifdef  VENDOR
	BOARDDIR = $(VENDOR)/$(BOARD)
else
	BOARDDIR = $(BOARD)
endif
```
`$(BOARD)`的值是mini2440，VENDOR的值是samsung，因此BOARDDIR的值是`samsung/mini2440`。BOARDDIR变量表示开发板特有的代码所在的目录。

```makefile
ifdef  BOARD
sinclude $(TOPDIR)/board/$(BOARDDIR)/config.mk   # include board specific rules
endif
```
Make将`“board/samsung/mini2440/config.mk”`包含进来。该脚本只有如下的一行代码：

```makefile
TEXT_BASE = 0x33F80000
```
 U-Boot编译时将使用TEXT_BASE作为**代码段连接的起始地址**。

```makefile
LDFLAGS += -Bstatic -T $(obj)u-boot.lds $(PLATFORM_LDFLAGS)
ifneq ($(TEXT_BASE),)
	LDFLAGS += -Ttext $(TEXT_BASE)
endif
```
执行完以上代码后，LDFLAGS中包含了`“-Bstatic -T u-boot.lds ”`和`“-Ttext 0x33F80000”`的字样。

链接地址是0x33f80000,意思是这个 UBOOT 运行时应该位于 0x33f8000 这里。

#### 2.3.2.5 指定隐含的编译规则

```makefile
# Allow boards to use custom optimize flags on a per dir/file basis

BCURDIR := $(notdir $(CURDIR))
$(obj)%.s:     %.S
       $(CPP) $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $<

$(obj)%.o:    %.S
       $(CC)  $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $< -c

$(obj)%.o:    %.c
       $(CC)  $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c

$(obj)%.i:     %.c
       $(CPP) $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c

$(obj)%.s:     %.c
       $(CC)  $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c -S
```
例如：根据以上的定义，以`.s`结尾的目标文件将根据第一条规则由同名但后缀为`.S`的源文件来生成，若不存在`.S`结尾的同名文件则根据最后一条规则由同名的`.c`文件生成。

下面回来接着分析Makefile的内容：

```makefile
# U-Boot objects....order is important (i.e. start must be first)
OBJS  = cpu/$(CPU)/start.o
LIBS += cpu/$(CPU)/lib$(CPU).a

ifdef SOC
LIBS += cpu/$(CPU)/$(SOC)/lib$(SOC).a
endif

ifeq ($(CPU),ixp)
LIBS += cpu/ixp/npe/libnpe.a
endif

LIBS += lib_$(ARCH)/lib$(ARCH).a
LIBS += fs/cramfs/libcramfs.a fs/fat/libfat.a fs/fdos/libfdos.a fs/jffs2/libjffs2.a \
        fs/reiserfs/libreiserfs.a fs/ext2/libext2fs.a fs/yaffs2/libyaffs2.a \
        fs/ubifs/libubifs.a
… …
LIBS += common/libcommon.a
LIBS += libfdt/libfdt.a
LIBS += api/libapi.a
LIBS += post/libpost.a
LIBS := $(addprefix $(obj),$(LIBS))
```
LIBS变量指明了U-Boot需要的库文件，包括`平台/开发板`相关的目录、通用目录下相应的库，都通过相应的子目录编译得到的。

对于mini2440开发板，以上跟平台相关的有以下几个：

```makefile
cpu/$(CPU)/start.o
board/$(VENDOR)/common/lib$(VENDOR).a
cpu/$(CPU)/lib$(CPU).a
cpu/$(CPU)/$(SOC)/lib$(SOC).a
lib_$(ARCH)/lib$(ARCH).a
```
其余都是与平台无关的

```makefile
ifeq ($(CONFIG_NAND_U_BOOT),y)
NAND_SPL = nand_spl
U_BOOT_NAND = $(obj)u-boot-nand.bin
endif

ifeq ($(CONFIG_ONENAND_U_BOOT),y)
ONENAND_IPL = onenand_ipl
U_BOOT_ONENAND = $(obj)u-boot-onenand.bin
ONENAND_BIN ?= $(obj)onenand_ipl/onenand-ipl-2k.bin
endif
```
对于有的开发板，U-Boot支持在NAND Flash启动，这些开发板的配置文件定义了CONFIG_NAND_U_BOOT，CONFIG_ONENAND_U_BOOT。对于s3c2440，U-Boot原始代码并不支持NAND Flash启动，因此**也没有定义这两个宏**。

```makefile
ALL += $(obj)u-boot.srec $(obj)u-boot.bin $(obj)System.map $(U_BOOT_NAND) $(U_BOOT_ONENAND)

all: $(ALL)
```
其中U_BOOT_NAND与U_BOOT_ONENAND 为空，而`u-boot.srec`，`u-boot.bin`，`System.map`都依赖与u-boot。因此执行`make all`命令将生成u-boot，u-boot.srec，u-boot.bin，System.map 。

* 其中u-boot是**ELF文件**
* u-boot.srec是Motorola S-Record format文件
* System.map 是U-Boot的符号表
* u-boot.bin是最终烧写到开发板的**二进制可执行的文件**

### 2.4 下面再来分析u-boot.bin文件生成的过程
ELF格式“u-boot”文件生成规则如下：

```makefile
$(obj)u-boot:  depend $(SUBDIRS) $(OBJS) $(LIBBOARD) $(LIBS) $(LDSCRIPT) $(obj)u-boot.lds 
	$(GEN_UBOOT)

ifeq ($(CONFIG_KALLSYMS),y)
	smap=`$(call SYSTEM_MAP,u-boot) | \
	awk '$$2 ~ /[tTwW]/ {printf $$1 $$3 "\\\\000"}'` ; \
	$(CC) $(CFLAGS) -DSYSTEM_MAP="\"$${smap}\"" \
	-c common/system_map.c -o $(obj)common/system_map.o
	$(GEN_UBOOT) $(obj)common/system_map.o
endif
```
这里生成的`$(obj)u-boot`目标就是ELF格式的U-Boot文件了。由于CONFIG_KALLSYMS未定义，因此`ifeq ($(CONFIG_KALLSYMS),y)`与`endif`间的代码不起作用。

其中`depend，$(SUBDIRS)，$(OBJS)，$(LIBBOARD)，$(LIBS)，$(LDSCRIPT)， $(obj)u-boot.lds`是`$(obj)u-boot`的依赖，而`$(GEN_UBOOT)`编译命令。

下面分析`$(obj)u-boot`的各个依赖：

#### 2.4.1 依赖目标depend

```makefile
# Explicitly make _depend in subdirs containing multiple targets to prevent
# parallel sub-makes creating .depend files simultaneously.

depend dep: $(TIMESTAMP_FILE) $(VERSION_FILE) $(obj)include/autoconf.mk
	for dir in $(SUBDIRS) cpu/$(CPU) $(dir $(LDSCRIPT)) ; do \
	$(MAKE) -C $$dir _depend ; done
```

对于`$(SUBDIRS)`，`cpu/$(CPU)`，`$(dir $(LDSCRIPT))`中的每个元素都进入该目录执行`“make _depend”`，生成各个子目录的`.depend`文件，`.depend`列出每个目标文件的依赖文件。

#### 2.4.2 依赖SUBDIRS

```makefile
SUBDIRS = tools \
	examples/standalone \
	examples/api

$(SUBDIRS): depend
	$(MAKE) -C $@ all
```
执行`tools`,`examples/standalone` ，`examples/api`目录下的Makefile。

#### 2.4.3 OBJS
 OBJS的值是`“cpu/arm920t/start.o”`。它使用如下代码编译得到：

```makefile
$(OBJS): depend
       $(MAKE) -C cpu/$(CPU) $(if $(REMOTE_BUILD),$@,$(notdir $@))
```
以上规则表明，对于OBJS包含的每个成员，都进入`cpu/$(CPU)`目录（即`cpu/arm920t`）编译它们。

#### 2.4.4 LIBBOARD

```makefile
LIBBOARD = board/$(BOARDDIR)/lib$(BOARD).a
LIBBOARD := $(addprefix $(obj),$(LIBBOARD))

… …
$(LIBBOARD): depend $(LIBS)
              $(MAKE) -C $(dir $(subst $(obj),,$@))
```
这里LIBBOARD的值是`$(obj)board/samsung/mini2440/libmini2440.a`。make执行`board/samsung/mini2440/`目录下的Makefile，生成`libmini2440.a`。

#### 2.4.5 LIBS
 LIBS变量中的每个元素使用如下的规则编译得到：

```makefile
$(LIBS):   depend $(SUBDIRS)
	$(MAKE) -C $(dir $(subst $(obj),,$@))
```
上面的规则表明，对于LIBS中的每个成员，都进入相应的子目录执行“make”命令编译它们。例如对于LIBS中的`common/libcommon.a`成员，程序将进入common目录执行Makefile，生成libcommon.a 。

#### 2.4.6 LDSCRIPT

```makefile
LDSCRIPT := $(SRCTREE)/cpu/$(CPU)/u-boot.lds
… …
$(LDSCRIPT): depend
              $(MAKE) -C $(dir $@) $(notdir $@)
```
`$(MAKE) -C $(dir $@) $(notdir $@)`命令经过变量替换后就是`make -C cpu/arm920t/  u-boot.lds`。也就是转到`cpu/arm920t/`目录下，执行`make u-boot.lds`命令。

#### 2.4.7 `$(obj)u-boot.lds`

```makefile
$(obj)u-boot.lds: $(LDSCRIPT)
              $(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$^ >$@
```
以上执行结果实质上是将`cpu/arm920t/u-boot.lds`经编译器简单预处理后输出到U-Boot顶层目录下的u-boot.lds文件。其中的`cpu/arm920t/u-boot.lds`文件内容如下：

```lds
/* 输出为ELF文件，小端方式， */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)   
ENTRY(_start)
SECTIONS
{
       . = 0x00000000;
       . = ALIGN(4);
       .text :
       {
            /* cpu/arm920t/start.o放在最前面，保证最先执行的是start.o */
            cpu/arm920t/start.o    (.text)
            /*以下2个文件必须放在前4K，因此也放在前面，其中board/samsung/mini2440/lowlevel_init.o 包含内存初始化所需代码，而 board/samsung/mini2440/nand_read.o 包含U-Boot从NAND Flash搬运自身的代码 */
	     board/samsung/mini2440/lowlevel_init.o (.text)
	     board/samsung/mini2440/nand_read.o (.text)

            /* 其他文件的代码段 */
	     *(.text)

       }

       /* 只读数据段 */
	. = ALIGN(4);
       .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }

       /* 代码段 */
       . = ALIGN(4);
       .data : { *(.data) }

       /* u-boot自定义的got段 */
       . = ALIGN(4);
       .got : { *(.got) }
       . = .;
       __u_boot_cmd_start = .;            /*将 __u_boot_cmd_start指定为当前地址 */
       .u_boot_cmd : { *(.u_boot_cmd) }   /* 存放所有U-Boot命令对应的cmd_tbl_t结构体 */
       __u_boot_cmd_end = .;              /*  将__u_boot_cmd_end指定为当前地址  */
       /* bss段 */
       . = ALIGN(4);
       __bss_start = .;
       .bss (NOLOAD) : { *(.bss) . = ALIGN(4); }
       _end = .;              /*  将_end指定为当前地址  */

}
```
u-boot.lds实质上是U-Boot连接脚本。对于生成的U-Boot编译生成的“u-boot”文件，可以使用**objdump命令可以查看它的分段信息**：

```
$objdump -x u-boot | more
```
部分输出信息如下：

```
u-boot:     file format elf32-little
u-boot
architecture: UNKNOWN!, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x33f80000
Program Header:
    LOAD off    0x00008000 vaddr 0x33f80000 paddr 0x33f80000 align 2**15
         filesz 0x0002f99c memsz 0x00072c94 flags rwx
   STACK off    0x00000000 vaddr 0x00000000 paddr 0x00000000 align 2**2
         filesz 0x00000000 memsz 0x00000000 flags rwx


Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         00024f50  33f80000  33f80000  00008000  2**5
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .rodata       00008b78  33fa4f50  33fa4f50  0002cf50  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 .data         00001964  33fadac8  33fadac8  00035ac8  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  3 .u_boot_cmd   00000570  33faf42c  33faf42c  0003742c  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  4 .bss          00043294  33fafa00  33fafa00  0003799c  2**8
                  ALLOC
… …
```
u-boot.lds还跟U-Boot启动阶段复制代码到RAM空间的过程以及U-Boot命令执行过程密切相关，具体请结合U-Boot源代码理解。

#### 2.4.8 编译命令GEN_UBOOT

```makefile
GEN_UBOOT = \
	UNDEF_SYM=`$(OBJDUMP) -x $(LIBBOARD) $(LIBS) | \
    sed  -n -e 's/.*\($(SYM_PREFIX)__u_boot_cmd_.*\)/-u\1/p'|sort|uniq`;\
	cd $(LNDIR) && $(LD) $(LDFLAGS) $$UNDEF_SYM $(__OBJS) \
    --start-group $(__LIBS) --end-group $(PLATFORM_LIBS) \
	-Map u-boot.map -o u-boot
```
以上命令使用`$(LDFLAGS)`作为连接脚本，最终生成“u-boot”文件。

`board/100ask24x0/u-boot.lds`这样一个链接脚本。这个链接脚本的第一个代码段文件从0x33f80000 处开始运行（当前地址 0 加上 0x33f80000，即放 UBOOT 放在 64M SDRAM 的最上边 512K 空间处）。

最后替换相关变量后，结果如下：

```
arm-linux-ld -Bstatic -T /work/arm920t/sourceCode/u_boot/u-boot-1.1.6/board/100ask24x0/u-boot.lds -Ttext 0x33F80000 
```
若想你的 UBOOT 放在另一个地址，则可以修改这个值。如这里单板有 64MSDRAM，将UBOOT 放在这 64M SDRAM 的最上边。

```
-------------- -----> 0x34000000
|             |
|512k放UBOOT  |
--------------  ----> 0x33f80000
|             |
|             |
|  64M SRAM   |
|             |
|             |
--------------  ----> 0x30000000
```

若是生成的 UBOOT 超过 512K,则把这个“TEXT_BASE”放到下面点，保证上面放 UBOOT 的空间即可。
若内存没有 64M，则这个值也要修改。

从 0 地址开始的话只有 4k 的 SRAM 可用，从 0x30000000 开始的话就有 64M 的 SDRAM 可用。
RAM 的地址范围是从 0x30000000 到 0x34000000 共 64MByte。

#### 2.4.9 u-boot.bin文件生成过程

生成u-boot.bin文件的规则如下：

```makefile
$(obj)u-boot.bin: $(obj)u-boot
    $(OBJCOPY) ${OBJCFLAGS} -O binary $< $@
```
从U-Boot编译输出信息中可以知道上面的命令实质上展开为：

```
arm-linux-objcopy --gap-fill=0xff -O binary u-boot u-boot.bin
```
编译命令中的`-O binary`选项指定了输出的文件为二进制文件。而`--gap-fill=0xff`选项指定使用`0xff`填充段与段间的空闲区域。

这条编译命令实现了ELF格式的U-Boot文件到BIN格式的转换。

### 2.5 System.map文件的生成

System.map是U-Boot的符号表，它包含了U-Boot的全局变量和函数的地址信息。将System.map生成的规则如下：

```makefile
SYSTEM_MAP = \
	$(NM) $1 | \
	grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | \
	LC_ALL=C sort
$(obj)System.map:     $(obj)u-boot
	@$(call SYSTEM_MAP,$<) > $(obj)System.map

# 等价于
arm-linux-nm u-boot | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | LC_ALL=C sort  > System.map
```

也就是将arm-linux-nm命令查看u-boot的输出信息经过过滤和排序后输出到System.map。为了了解System.map文件的作用，打开System.map：

```
33f80000 T _start
33f80020 t _undefined_instruction
33f80024 t _software_interrupt
33f80028 t _prefetch_abort
33f8002c t _data_abort
33f80030 t _not_used
33f80034 t _irq
33f80038 t _fiq
33f80040 t _TEXT_BASE
33f80044 T _armboot_start
33f80048 T _bss_start
33f8004c T _bss_end
… …
```
 System.map表示的是地址标号到该标号表示的地址的一个映射关系。System.map每一行的格式都是`addr type name`
 
* addr是标号对应的地址值
* name是标号名
* type表示标号的类型。

U-Boot的编译和运行并不一定要生成System.map，这个文件主要是提供给**用户或外部程序调试时使用的**。