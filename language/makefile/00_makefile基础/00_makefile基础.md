# 0.关于程序的编译和链接
* 编译:把源文件编译成中间代码文件`.c->.o`和 `.c->.d`
* 编译器需要的是语法的正确，函数与变量的声明的正确。
* 链接:大量的Object File合成执行文件.
	*  主要是链接函数和全局变量，所以，我们可以使用这些中间目标文件（`O`文件或是`OBJ`文件）来链接我们的应用程序。在UNIX下，是Archive File，也就是 .a 文件。
# 1.变量
> 命令make –p可以打印出很多默认变量和隐含规则。

## 1.1基础补漏
其实变量的定义有三种运算符`=`、`:=`、`?=`、`+=`。

* `=`:运算符可以读取到**后面定义的变量**。
* `:=`:运算符在遇到变量定义时立即展开。符合C语言的编程习惯。
* `?=`:运算符在复制之前先做判断变量是否已经存在。例如var1 ?= $(var2)的意思是：
    * 如果var1没有定义过，那么`?=`相当于`=`
	* 如果var1先前已经定义了，则什么也不做，不会给var重新赋值。
*  `+=`运算符是给变了追加值。如果变量还没有定义过就直接用`+=`赋值，那么`+=`相当于`=`.

## 1.2显示
make会把执行的命令打印在屏幕上，如果我们不想把命令打印在屏幕上，只显示命令结果时，直接在命令前面加上符号“@”就可以实现。如上面help目标一样，只显示命令结果。一般我们会在make时都会输出“Compiling xxx.c…”,不输出编译时的命令。

# 2.嵌套结构

在大一些的项目里面，所有源代码不会只放在同一个目录，一般各个功能模块的源代码都是分开的，各自放在各自目录下，并且头文件和`.c`源文件也会有各自的目录，这样便于项目代码的维护。这样我们可以在每个功能模块目录下都写一个Makefile，各自Makefile处理各自功能的编译链接工作，这样我们就不必把所有功能的编译链接都放在同一个Makefile里面，这可使得我们的Makefile变得更加简洁，并且编译的时候可选择编译哪一个模块，这对分块编译有很大的好处。

在每个目录下写一个Makefile，通过最顶层的Makefile一层一层的向下嵌套执行各层Makefile。

* `$(MAKE) -C src`:就是进入src目录继续执行该目录下的Makefile。

```make
$(MAKE) -C src/ipc  
$(MAKE) -C src/tools  
$(MAKE) -C src/main  
```

每增加一个目录都要在多个伪目标里面加入一行，这样不够自动化啊，于是我们想到shell的循环语 句，我们可以在每条规则的命令处使用for

## 2.1shell循环
```make
DIR=src
SUBDIR=$(shell ls $(DIR))
all:
	for(subdir in $(SUBDIR));\
	do $(MAKE) -C $(DIR)/$$sudir;\
	done
```
## 2.2改进
上面for循环会依次进入系统命令ls列出的目录，但我们对每个目录的make顺序可能有要求，在该项目当中，main目录下的Makefile必须最后执行，因为最终的链接需要其他目录编译生成的库文件，否则会执行失败。并且在当前的Makefile中，**当子目录执行make出现错误时**，**make不会退出**。在最终执行失败的情况下，我们很难根据错误的提示定位出具体是是那个目录下的Makefile出现错误。这给问题定位造成了很大的困难。为了避免这样的问题，**在命令执行错误后make退出**.所以将刚才的Makefile修改为如下`do $(MAKE) -C $(DIR)/$$subdir || exit 1;`

## 3.参数专递

在多个Makefile嵌套调用时，有时我们需要传递一些参数给下一层Makefile。比如我们在顶层Makefile里面定义的打开调试信息变量`DEBUG_SYMBOLS`，我们希望在进入子目录执行子Makefile时该变量仍然有效，这是需要将该变量传递给子Makefile，那怎么传递呢？这里有两种方法：

* 1.在上层Makefile中使用`export`关键字对需要传递的变量进行声明。
	* 当不希望将一个变量传递给子 make 时，可以使用指示符 `unexport`来声明这个变量。
* 2.在命令行上指定变量。比如：
	 `$(MAKE) -C xxx DEBUG_SYMBOLS = TRUE`

## 3.include 嵌套makefile
Makefile的嵌套调用，每一个模块都有自己的Makefile。其实每个模块的Makefile都大同小异，只需要改改最后编译成生成的目标名称或者编译链接选项，规则都差不多，那么我们是否可以考虑将规则部分提取出来，**每个模块只需修改各自变量**即可。这样是可行的，我们将规则单独提取出来，写一个Makefile.rule，将他放在顶层Makefile同目录下，其他模块内部的Makefile只需要include该Makefile就可以了。如下：`include $(SRC_BASE)/Makefile.rule` 

# 4.makefile定义宏
来自kernel kconfig文件

```make
define mergeconfig
$(if $(wildcard $(objtree)/.config),, $(error You need an existing .config for this target))
$(fi $(call configfiles,$(1),, $(error No configuration exists for this target on this architecture)))
$(Q)$(CONFIG_SHELL) $(srctree)/scripts/kconfig/merge_config.sh -m -O $(objtree)/.config $(call configfiles,$(1))
$(Q)yes "" | $(MAKE) -f $(srctree)/Makefile oldconfig
endef
```

# 5.makefile info和eval区别
> info 相当于宏展开，而eval相当于计算

```make
pointer:=pointed_value
define foo
val:=123
arg:=$1
$($(1)):=oooo
endef
#$(info $(call foo,pointer))
$(eval $(call foo,pointer))
.PHONY:target
target:
	@echo ------------
	@echo var:$(var),arg:$(arg)
	@echo pointer:$(pointer),pointed_value:$(pointed_value)
	@echo done
	@echo -------------
```
实例中：

* makefile中定义一个宏foo,第一个参数为$1
* info只是展开宏，不做计算，call调用“函数” foo,参数pointer
* eval计算，call调用"函数" foo,参数为pointer
