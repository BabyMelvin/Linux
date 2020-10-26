Linux 内核是一种单体内核，但是通过动态加载模块的方式，使它的开发非常灵活方便。那么，它是如何编译内核的呢？我们可以通过分析它的 Makefile 入手。以下是一个简单的 hello 内核模块的 Makefile.

```makefile
ifneq ($(KERNELRELEASE),)
    obj-m:=hello.o
else
    KERNELDIR:=/lib/modules/$(shell uname -r)/build
    PWD:=$(shell pwd)
default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
    rm -rf *.o *.mod.c *.mod.o *.ko
endif
```
当我们写完一个 hello 模块，只要使用以上的 makefile。然后 make 一下就行。假设我们把 hello 模块的源代码放在`/home/study/prog/mod/hello/`下。

> 当我们在这个目录运行 make 时，make 是怎么执行的呢？

LDD3 第二章第四节“编译和装载”中只是简略地说到该 Makefile 被执行了两次，但是具体过程是如何的呢？

首先，由于 make 后面没有目标，所以 make 会在 Makefile 中的第一个不是以.开头的目标作为默认的目标执行。于是 default 成为 make 的目标。make 会执行`$(MAKE) -C $(KERNELDIR) M=$(PWD) modules`

shell 是 make 内部的函数,假设当前内核版本是 2.6.13-study,所以`$(shell uname -r)`的结果是 2.6.13-study这里，实际运行的是

`make -C /lib/modules/2.6.13-study/build M=/home/study/prog/mod/hello/ modules`

`/lib/modules/2.6.13-study/build` 是一个指向内核源代码`/usr/src/linux`的符号链接。

可见，make 执行了两次。

* 第一次执行时是读 hello 模块的源代码所在目录`/home/study/prog/mod/hello/`下的 Makefile。
* 第二次执行时是执行/usr/src/linux/下的 Makefile 时.

但是还是有不少令人困惑的问题：

* 1.这个 KERNELRELEASE 也很令人困惑，它是什么呢？
在`/home/study/prog/mod/hello/Makefile`中是没有定义这个变量的，所以起作用的是`else...endif`这一段。不过，如果把 hello 模块移动到内核源代码中。例如放到`/usr/src/linux/driver/`中，KERNELRELEASE 就有定义了。在`/usr/src/linux/Makefile`中有
```
162 KERNELRELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)$(LOCALVERSION)
```
这时候，hello 模块也不再是单独用 make 编译，而是在内核中用 make modules 进行编译。用这种方式，该 Makefile 在单独编译和作为内核一部分编译时都能正常工作。

* 2.这个`obj-m := hello.o`什么时候会执行到呢？

在执行：`make -C /lib/modules/2.6.13-study/build M=/home/study/prog/mod/hello/ modules`时，make 去/usr/src/linux/Makefile 中寻找目标 modules:

```makefile
862 .PHONY: modules
863 modules: $(vmlinux-dirs) $(if $(KBUILD_BUILTIN),vmlinux)
864 @echo ' Building modules, stage 2.';
865 $(Q)$(MAKE) -rR -f $(srctree)/scripts/Makefile.modpost
```
可以看出，分两个 stage:

* 1.编译出 hello.o 文件。
* 2.生成 hello.mod.o hello.ko

在这过程中，会调用`make -f scripts/Makefile.build obj=/home/study/prog/mod/hello`

而在`scripts/Makefile.build`会包含很多文件：

```makefile
011 -include .config
012
013 include $(if $(wildcard $(obj)/Kbuild), $(obj)/Kbuild, $(obj)/Makefile)
```

其中就有`/home/study/prog/mod/hello/Makefile`这时 KERNELRELEASE 已经存在。所以执行的是：

```
obj-m:=hello.o
```
关于 make modules 的更详细的过程可以在 scripts/Makefile.modpost 文件的注释中找到。如果想查看 make 的整个执行过程，可以运行`make -n`。由此可见，内核的 Kbuild 系统庞大而复杂。
