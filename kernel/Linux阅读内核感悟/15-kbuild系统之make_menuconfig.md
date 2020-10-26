理解了 make bzImage 的过程，理解了整个 kbuild 的结构和运行机制，`make menuconfig`的过程就很容易理解了。

先看`/usr/src/linux/Makefile`。可以找到

```makefile
452 %config: scripts_basic outputmakefile FORCE
453 $(Q)$(MAKE) $(build)=scripts/kconfig $@
```

* `%config`是通配符，所有以 config 结尾的目标（menuconfig xconfig gconfig）都采用这个规则。所以 make menuconfig 在进行一些准备工作如`make scripts_basic`等操作后，最终会运行`$(Q)$(MAKE) $(build)=scripts/kconfig menuconfig`

其中`$@`指要生成的目标文件，这里指伪目标`menuconfig`。接下去调用的是：`scripts/kconfig/Makefile`

```makefile
013 menuconfig: $(obj)/mconf
014 $(Q)$(MAKE) $(build)=scripts/lxdialog
015 $< arch/$(ARCH)/Kconfig
082 hostprogs-y := conf mconf qconf gconf kxgettext
083 conf-objs := conf.o zconf.tab.o
084 mconf-objs := mconf.o zconf.tab.o
085 kxgettext-objs := kxgettext.o zconf.tab.o
```
这里可以看到最终执行的是 scripts/kconfig/mconf arch/i386/Kconfig 其中`$<`代表“起因”，也就是 `scripts/kconfig/mconf`该程序并不属于内核，而是一个用户态程序。Linux 源代码中这一类程序还有很多。如在`scripts/kconfig/`目录下就有 mconf,gconf,qconf 等等。它们用来执行内核的配置工作。

又如在 arch/i386/boot/tools/中有个可执行程序叫 build，它用来把 bootsect(引导扇区) ,setup（辅助程序）和 vmlinux.bin(压缩内核)拼接成 bzImage。

scripts/kconfig/mconf 这个程序采用了 ncurses 类库。这是一个在文本界面下进行画图操作类库。由于要适应不同平台，源代码中的 mconf 不是预编译好的 elf 可执行文件，而是在使用时才去编译生成。这使用户在运行 `make menuconfig`时要依赖 **ncurses 的开发包**。

arch/i386/Kconfig，准确地说是各个 Kconfig 文件记录了各个内核配置的选项。我们在 `make menuconfig` 或者 `make xconfig `时显示的菜单项和帮助信息，都是从这个文件中读出来的。到此，我们对平时使用的 `make menuconfig` 命令的执行流程应该有了一个大概的印象了吧。


