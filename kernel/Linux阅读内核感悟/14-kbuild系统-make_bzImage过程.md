从以上例子中可以看到，内核的编译系统 kbuild 是个很庞大的系统。但是，它所使用的 make 和我们平时用的 make 是一模一样的。kbuild 只是通过预定义一些变量（objm,obj-y 等等）和目标(bzImage ,menuconfig 等等)，使内核的编译和扩展变得十分方便。我们不妨 yy 一下 kbuild 的一些功能：

* 1.考虑到 Linux 能够方便地移植到各个硬件平台，kbuild 也必须很容易添加对某个新的平台的支持，同时上层的 Makefile 不需要做大的改动。
* 2.Linux 下有众多驱动设备。它们的 Makefile 希望能够尽可能简洁。简洁到只要指定要编译的.o 文件就行。（这方面 kbuild 定义了很多有用的变量如 obj-m obj-y,<module>-objs 等等，用户只要为这些变量赋值，kbuild 会自动把代码编译到内核或者编译成模块）
* 3.要有方便的可定制性。很多参数可以让用户指定。这方面 kbuild 也提供了大量的变量如 EXTRA_CFLAGS，用户如果想 include 自己的头文件或者加其它编译参数，只要设置一下`EXTRA_CFLAGS`就可以。
* 4.有能力递归地调用 Makefile。因为内核是一个庞大的软件。它的源代码的目录层次很深。要提供一种简洁的机制，使上层的 Makefile 能方便地调用下层的 Makefile。在这过程中，面向对象的思想也许值得借鉴。
* 5.在配置内核时，要提供友好的用户界面。这方面 kbuild 也提供了不少工具，如常用的 make menuconfig 等等。
我们完全可以把 kbuild 想象成一个类库，它为普通的内核开发人员提供了接口（obj-m obj-y EXTRA_CFLAGS 等等），为用户提供了定制工具（make menuconfig）

如果想了解 kbuild 的使用方法，可以参阅源代码自带的文档：

```
Documentation/kbuild/makefiles.txt
Documentation/kbuild/modules.txt
```
一般情况下是不需要知道具体的编译顺序的。除了在个别情况下，如
`do_initcalls()`中就和函数在`.initcall.init section`中的顺序有关。不过喜欢寻根究底的我，还是想理一下编译内核时几个常用的命令，如 make bzImage,make menuconfig 等等，进而了解 kbuild 的架构。先看 make bzImage 吧。

> 它的大概脉络是怎样的呢？可以用以下命令查看。`make -n bzImage`
如果嫌内容太多，可以过滤掉多余的信息：`make -n bzImage | grep “make -f”`

可以猜到：

先作一些准备工作:

`make -f scripts/Makefile.build obj=scripts/basic`

然后依次递归地调用源代码中的 Makefile

```
make -f scripts/Makefile.build obj=init
make -f scripts/Makefile.build obj=usr
make -f scripts/Makefile.build obj=arch/i386/kernel
make -f scripts/Makefile.build obj=arch/i386/kernel/acpi
make -f scripts/Makefile.build obj=arch/i386/kernel/cpu
make -f scripts/Makefile.build obj=arch/i386/kernel/cpu/cpufreq
make -f scripts/Makefile.build obj=arch/i386/kernel/cpu/mcheck
make -f scripts/Makefile.build obj=arch/i386/kernel/cpu/mtrr
make -f scripts/Makefile.build obj=arch/i386/kernel/timers
。。。
```
最后压缩内核，生成 bzImage

```
make -f scripts/Makefile.build obj=arch/i386/boot arch/i386/boot/bzImage
make -f scripts/Makefile.build obj=arch/i386/boot/compressed
IMAGE_OFFSET=0x100000 arch/i386/boot/compressed/vmlinux
```

好，我们从头开始。找 make bzImage 的入口：第一反应，自然是在`/usr/src/linux/Makefile`中找

```
bzImage:
    ...
```
可惜没找到。不过没关系， 搜索一下，可知 bzImage 定义在`arch/i386/Makefile`，所以可以猜测，该 makefile 一定是被 include 了。果然，在`/usr/src/linux/Makefile`中有：

```makefile
447 include $(srctree)/arch/$(ARCH)/Makefile
```
又因为在`arch/i386/Makefile`中定义有

```makefile
141 zImage bzImage: vmlinux
142 $(Q)$(MAKE) $(build)=$(boot) $(KBUILD_IMAGE)
```
其中这个`$(build)`定义在`/usr/src/linux/Makefile`中

```makefile
1335 build := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.build obj
```
我们在之前查看`make -n bzImage`信息和之后会经常看到。我们会发现 kbuild 通常不会直接去调用某个目录下的 Makefile，而是让该目录作为 `scripts/Makefile.build`的参数。`scripts/Makefile.build`会对该目录下的 Makefile 中的内容(主要是 obj-m 和 obj-y等等)进行处理。由此看来`scripts/Makefile.build`这个文件很重要。看看它做了什么：

由于 `scripts/Makefile.build`后面没跟目标，所以默认为第一个目标：

```makefile
007 .PHONY: __build
008 __build:
009
010 # Read .config if it exist, otherwise ignore
011 -include .config
012
013 include $(if $(wildcard $(obj)/Kbuild), $(obj)/Kbuild, $(obj)/Makefile)
014 
015 include scripts/Makefile.lib
```
这里可以看到，scripts/Makefile.build 执行时会`include .config`文件。`.config`是 `make menuconfig`后生成的内核配置文件。里面有如下语句：

```
CONFIG_ACPI_THERMAL=y
CONFIG_ACPI_ASUS=m
CONFIG_ACPI_IBM=m
```
以前我一直对它的格式表示奇怪，现在很清楚了，它们是作为 makefile 的一部分，通过读取 CONFIG_XXX 的值就可以知道他们是作为模块还是作为内核的一部分而编译的。

此外，还包含了`$(obj)/Makefile`。这就是通过在make时传递目录名`$(obj)`间接调用Makefile 的手法。对于内核普通代码所对应的 Makefile 而言，里面只是对 `obj-m` `obj-y`,`<module>-objs`等变量进行赋值操作。

接下去是`include scripts/Makefile.lib`.正如它的文件名所示，这类似于一个库文件。它负责对 `obj-m` `obj-y`,`<module>-objs`等变量进行加工处理。从中提取出`subdir-ym`等变量，这是个很重要的变量，记录了需要递归调用的子目录。以后递归调用`Makefile`全靠它了。这里也充分体现了 GNU make 对字符串进行操作的强大功能。

回到 Makefile.build。这时，重要变量`$(builtin-target)`，`$(subdir-ym)`等都已经计算完毕。开始列依赖关系和具体操作了。

```makefile
079 __build: $(if $(KBUILD_BUILTIN),$(builtin-target) $(lib-target) $(extray)) \
080 $(if $(KBUILD_MODULES),$(obj-m)) \
081 $(subdir-ym) $(always)
082 @:
```

* `$(builtin-target)`是指当前目录下的目标文件，即`$(obj)/built-in.o`
* 如前文所说，$(subdir-ym)用来递归调用子目录的 Makefile

```makefile
306 # Descending
307 #
---------------------------------------------------------------------------
308
309 .PHONY: $(subdir-ym)
310 $(subdir-ym):
311 $(Q)$(MAKE) $(build)=$@
```
通过这种方式，实现了对某个目录及其子目录的编译。

> 现在看看 vmlinux 是如何生成的。

分析完 Makefile.build，回过头来再看 bzImage.从 arch/i386/Makefile 中可以看到，bzImage 是在 vmlinux 基础上加以压缩拼接而成。从 vmlinux 到 bzImage 的过程在《读核感悟-Linux 内核启动-内核的生成》中已经有介绍。现在看看 vmlinux 是如何生成的。见/usr/src/linux/Makefile

```makefile
728 vmlinux: $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main) $(kallsyms.o) FORCE
729 $(call if_changed_rule,vmlinux__)
611 vmlinux-init := $(head-y) $(init-y)
612 vmlinux-main := $(core-y) $(libs-y) $(drivers-y) $(net-y)
613 vmlinux-all := $(vmlinux-init) $(vmlinux-main)
614 vmlinux-lds := arch/$(ARCH)/kernel/vmlinux.lds
```
vmlinux 所依赖的目标`$(vmlinux-lds)`是对 arch/i386/kernel/vmlinux.lds.S 进行预处理的结果`arch/i386/kernel/vmlinux.lds` ,其它的依赖关系也都可以在/usr/src/linux/Makefile 中查到。

所以，当用户在源代码目录下执行 make bzImage。make 会检查 bzImage 的依赖目标，然后不停地递归调用各个 Makefile，最终生成一个 bzImage 文件。如果我们换个角度，还可以归纳出不少有趣的东西。如果把 make 看成是一种脚本语言，那么 Makefile 就是代码。make 就是解释器。make 里也有函数，也有变量。通过定义目标，可以实现类似于函数的效果。而目标之间的依赖关系则类似于函数内部再调用其它函数。

如果我们考虑变量的作用域，还可以归纳出以下几点：

* 1.Makefile 内部定义的变量作用域只限于那个 Makefile 中，如 obj-m。
* 2.要使变量的作用域扩展到整个 make 命令的执行过程（包括递归调用的其它Makefile），需要使用 export 命令。

调用 Makefile 的方式也有很多种：

* 1.一种是隐式调用，如运行 make，它会自动在当前目录寻找 Makefile 等。
* 2.一种是显式调用，如用 make -f 指定。
* 3.一种是用 include 来调用。




