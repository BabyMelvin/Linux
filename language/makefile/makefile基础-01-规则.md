## 1.简介
### 1.1 makefile说明
* 编译时，编译器需要的是语法的正确，函数与变量的声明的正确。
* 链接时，主要是链接函数和全局变量。
链接器并不管函数所在的源文件，只管函数的中间目标文件（Object File），
在大多数时候，由于源文件太多，编译生成的中间目标文件太多，而在链接时需要明显地指出中间目标
文件名，这对于编译很不方便。所以，我们要给中间目标文件打个包，在 Windows 下这种包叫“库文
件”(Library File)，也就是 .lib 文件，在 UNIX 下，是 Archive File，也就是 .a 文件。

Makefile规则：

1. 如果这个工程没有编译过，那么我们的所有 c 文件都要编译并被链接。
2. 如果这个工程的某几个 c 文件被修改，那么我们只编译被修改的 c 文件，并链接目标程序。
3. 如果这个工程的头文件被改变了，那么我们需要编译引用了这几个头文件的 c 文件，并链接目标程
序。

make 会比较 targets 文件和 prerequisites 文件的修改日期，如果 prerequisites 文件的日期要比 targets 文件的日期要新，或者target 不存在的话，那么，make 就会执行后续定义的命令。

Makefile工作过程：make 会在当前目录下找名字叫“Makefile”或“makefile”的文件。如果找不到，它会找文件中的第一个目标文件（target）.整个 make 的依赖性，make 会一层又一层地去找文件的依赖关系，直到最终编译出第一个目标文件。

Makefile 里主要包含了五个东西：显式规则、隐晦规则、变量定义、文件指示和注释。在 Makefile 中的**命令**，必须要以 Tab 键开始。

### 1.2.makefile 中使用变量

```makefile
objects = main.o kbd.o command.o display.o \
insert.o search.o files.o utils.o
edit : $(objects)
	cc -o edit $(objects)
main.o : defs.h
kbd.o : defs.h command.h
command.o : defs.h command.h
display.o : defs.h buffer.h
insert.o : defs.h buffer.h
search.o : defs.h buffer.h
files.o : defs.h buffer.h command.h
utils.o : defs.h

.PHONY : clean
clean :
	-rm edit $(objects)
```
`.PHONY `表示 clean 是一个"伪目标"。而在 rm 命令前面加了一个**小减号**的意思就是，也
许某些文件出现问题，但不要管，继续做后面的事。当然，clean 的规则不要放在文件的开头，不然，这就会变成 make 的默认目标，相信谁也不愿意这样。不成文的规矩是——“clean 从来都是放在文件的最后”。

### 1.3 引用其它的 Makefile
include包含其他makefile：

* `include <filename>`在 include 前面可以有一些空字符，但是绝不能是 Tab 键开始。如a.mk 、b.mk 、c.mk ，还有一个文件叫 foo.make
，以及一个变量 $(bar) ，其包含了 e.mk 和 f.mk:`include foo.make *.mk $(bar)`

如果你想让 make 不理那些无法读取的文件，而继续执行，你可以在include 前加一个减号“-”。如：`-include <filename>`,其表示，无论 include 过程中出现什么错误，都不要报错继续执行。和其它版本 make 兼容的相关命令是 sinclude，其作用和这一个是一样的。

### 1.4  make 的工作方式
GNU 的 make 工作时的执行步骤如下：

1. 读入所有的 Makefile。
2. 读入被 include 的其它 Makefile。
3. 初始化文件中的变量。
4. 推导隐晦规则，并分析所有规则。
5. 为所有的目标文件创建依赖关系链。
6. 根据依赖关系，决定哪些目标要重新生成。
7. 执行生成命令。

1-5 步为第一个阶段，6-7 为第二个阶段。

第一个阶段中，如果定义的变量被使用了，那么，make 会把其展开在使用的位置。但 make 并不会完全马上展开，make 使用的是拖延战术，如果变量出现在依赖关系的规则中，那么仅当这条依赖被**决定要使用**了，变量才会在其内部展开。

## 2.书写规则

```makefile
targets : prerequisites ; command
	command
	...
```
如果命令太长，你可以使用反斜杠(`\`)作为换行符。make 会以 UNIX 的标准 Shell，也就是`/bin/sh 来执行命令(command)`。

### 2.1 通配符
make 支持三个通配符：
`*` ，`?` 和 `~` 。这是和 Unix 的 B-Shell 是相同的。波浪号(~)字符在文件名中也有比较特殊的用途。如果是 `~/test` ，这就表示当前用户的 `$HOME` 目录下的 test 目录。而 `~hchen/test` 则表示用户 hchen 的宿主目录下的 test 目录。

*  `*.c` 表示所有后缀为 c 的文件.如：`*` ，那么可以用转义字符`\` ，如 `\*` 来表示真实的`*`字符，而不是任意长度的字符串。

```makefile
objects = *.o
```

表示了通配符同样可以用在变量中。并不是说 `*.o` 会展开,**objects的值**就是`*.o` 。Makefile 中的变量其实就是`C/C++`中的宏。如果你要让通配符在变量中展开，也就是让 objects的值是所有 .o 的文件名的集合，那么，你可以这样：

```makefile
objects := $(wildcard *.o)
```
另给一个变量使用通配符的例子：

1. 列出一确定文件夹中的所有 `.c` 文件:`objects := $(wildcard *.c)`
2. 列出 (1) 中所有文件对应的 .o 文件，在（3）中我们可以看到它是由 make 自动编译出的:`$(patsubst %.c,%.o,$(wildcard *.c))`
3. 由 (1)(2) 两步，可写出编译并链接所有 `.c` 和 `.o` 文件

```makefile
objects := $(patsubst %.c,%.o,$(wildcard *.c))
foo : $(objects)
	cc -o foo $(objects)
```

### 2.2 文件搜寻
Makefile 文件中的特殊变量 `VPATH`就是完成这个功能的，如果没有指明这个变量，make 只会在当
前的目录中去找寻依赖文件和目标文件。如果定义了这个变量，那么，make 就会在当前目录找不到的情
况下，到所指定的目录中去找寻文件了。`VPATH = src:../headers`,上面的定义指定两个目录，“src”和“`../headers`”，make 会按照这个顺序进行搜索。目录由“冒号”分隔。

另一个设置文件搜索路径的方法是使用 make 的“vpath”关键字(小写),这不是变量，这是一个 make 的关键字。可以指定不同的文件在不同的搜索目录中,三种方法:

* `vpath <pattern> <directories>`为符合模式 `<pattern>`的文件指定搜索目录`<directories>`。
* `vpath <pattern>` 清除符合模式 `<pattern>` 的文件的搜索目录。
* `vpath`清除所有已被设置好了的文件搜索目录。

vapth 使用方法中的`<pattern>`需要包含`%`字符。`%`的意思是匹配零或若干字符,`%.h`表示所有以`.h`结尾的文件。

```makefile
# 要求 make 在“../headers”目录下搜索所有以 .h 结尾的文件。
vpath %h ../headers
```

我们可以连续地使用 vpath 语句，以指定不同搜索策略。如果连续的 vpath 语句中出现了相同的
`<pattern>` ，或是被重复了的`<pattern>`，那么，make 会按照 vpath 语句的先后顺序来执行搜索。如

```makefile
# .c 结尾的文件，先在“foo”目录，然后是“blish”，最后是“bar”目录
vpath %.c foo
vpath %.c blish
vpath %.c bar
```

### 2.3 伪目标
“伪目标”并不是一个文件，只是一个标签，由于“伪目标”不是文件，所以 make 无法生成它的依赖关系和决定它是否要执行。

“伪目标”的取名不能和文件名重名，不然其就失去了“伪目标”的意义了。

为了避免和**文件重名**的这种情况，我们可以使用一个特殊的标记“`.PHONY`”来显式地指明一个目标是“伪目标”，向 make 说明，不管是否有这个文件，这个目标就是“伪目标”

```makefile
.PHONY : clean
clean:
	rm *.o temp
```

伪目标一般没有依赖的文件。但是，我们也可以为伪目标指定所依赖的文件。伪目标同样可以作为
“默认目标”，只要将其放在**第一个**。

一个示例就是，如果你的 Makefile 需要一口气生成若干个可执行文件，但你只想简单地敲一个 make 完事，并且，所有的目标文件都写在一个 Makefile 中，那么你可以使用“伪目标”这个特性

```makefile
all : prog1 prog2 prog3
.PHONY : all
prog1 : prog1.o utils.o
	cc -o prog1 prog1.o utils.o
prog2 : prog2.o
	cc -o prog2 prog2.o
prog3 : prog3.o sort.o utils.o
	cc -o prog3 prog3.o sort.o utils.o
```
由于默认目标的特性是，总是被执行的，但由于“all”又是一个伪目标，伪目标只是一个标签不会生成文件，所以不会有“all”文件产生。于是，其它三个目标的规则总是会被决议。也就达到
了我们一口气生成多个目标的目的。

目标也可以成为依赖。所以，伪目标同样也可成为依赖。

```makefile
.PHONY : cleanall cleanobj cleandiff

cleanall : cleanobj cleandiff
	rm program
cleanobj :
	rm *.o
cleandiff :
	rm *.diff
```

“make cleanall”将清除所有要被清除的文件。“cleanobj”和“cleandiff”这两个伪目标有点像“子程序”的意思。我们可以输入“make cleanall”和“make cleanobj”和“make cleandiff”命令来达到清除不同种类文件的目的。

### 2.4 多目标
有可能我们的多个目标同时依赖于一个文件，并且其生成的命令大体类似。于是我们就能把其合并起来。多个目标的生成规则的执行命令不是同一个，这可能会给我们带来麻烦，不过好在我们可以使用一个自动化变量 `$@`,表示着目前规则中所有的目标的集合。

```makefile
bigoutput littleoutput : text.g
	generate text.g -$(subst output,,$@) > $@
```
上述规则等价于：

```makefile
bigoutput : text.g
generate text.g -big > bigoutput
littleoutput : text.g
generate text.g -little > littleoutput
```
其中，`-$(subst output,,$@)` 中的 $ 表示执行一个 Makefile 的函数，函数名为 subst，后面的为参数。关`$@` 表示目标的集合，就像一个
数组，`$@` 依次取出目标，并执于命令。

### 2.5  静态模式
静态模式可以更加容易地定义多目标的规则

```makefile
<targets ...> : <target-pattern> : <prereq-patterns ...>
	<commands>
	...
```

* targets 定义了一系列的目标文件，可以有通配符。是目标的一个集合。
* target-parrtern 是指明了 targets 的模式，也就是的目标集模式。
* prereq-parrterns 是目标的依赖模式，它对 target-parrtern 形成的模式再进行一次依赖目标的定义。

如果我们的 `<target-parrtern>` 定义成 `%.o`，意思是我们的 `<target>`; 集合中都是以`.o` 结尾的，而如果我们的`<prereq-parrterns>`定义成 `%.c` ，意思是对`<target-parrtern>`所形成的目标集进行二次定义，其计算方法是，取`<target-parrtern>`模式中的`%`也就是去掉了`.o`这个结尾），并为其加上`.c`这个结尾，形成的新集合。

```makefile
objects = foo.o bar.o

all:$(objects)

$(objects):%.o:%.c
	$(CC) -c $(CFLAGS) @< -o $@
```

目标从`$object`中获取，`%.o`表明要所有以 `.o` 结尾的目标，也就是`foo.o bar.o` ，也就是变量 `$object`集合的模式，而依赖模式`%.c`则取模式 `%.o` 的 `%` ，也就是`foo bar`，并为其加下`.c` 的后缀，于是，我们的依赖目标就是`foo.c bar.c` 。

而命令中的 `$<` 和 `$@` 则是自动化
变量，`$<` 表示第**一个依赖文件**，`$@` 表示目标集（也就是“foo.o bar.o”）。于是，上面的规则展开后等价于下面的规则：

```makefile
foo.o : foo.c
	$(CC) -c $(CFLAGS) foo.c -o foo.o
bar.o : bar.c
	$(CC) -c $(CFLAGS) bar.c -o bar.o
```

如果我们的`%.o`有几百个，那么我们只要用这种很简单的“静态模式规则”就可以写完一堆规则，实在是太有效率了。“静态模式规则”的用法很灵活，如果用得好，那会是一个很强大的功能。再看一个例子：

```makefile
files = foo.elc bar.o lose.o
$(filter %.o,$(files)): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
$(filter %.elc,$(files)): %.elc: %.el
	emacs -f batch-byte-compile $<
```

`$(filter %.o,$(files))`表示调用 Makefile 的 filter 函数，过滤“$files”集，只要其中模式为“%.o”的内容。

### 2.6自动生成依赖性

Makefile的自动推导功能只会推导出目标文件对源文件的依赖关系，而不会增加**头文件的依赖关系**!!!这导致的直接问题就是修改项目的头文件，不会导致make的自动更新！除非修改头文件后运行一次make clean，再运行make…… 

如果将每个源文件的依赖关系包含到Makefile里，就可以使得目标文件自动依赖于头文件了！

```makefile
EXE=main
CC=gcc
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
CFLAGS=-g

all:depend $(EXE)
depend:
	@(CC) −MM (SRC) > .depend

-include .depend

(EXE):(OBJ)
	(CC) (OBJ) -o $(EXE)

clean:
	@rm (EXE) (OBJ) .depend -
```
当头文件的依赖关系不发生变化时，每次make也会重新生成.depend文件。如果这样使得工程的编译变得不尽人意，那么我们可以尝试将依赖文件拆分，使得每个源文件独立拥有一个依赖文件，这样每次make时变化的只是一小部分文件的依赖关系。让make自动包含头文件的依赖关系，我们需要做一点额外的工作。

```makefile
EXE=main
CC=gcc
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
DEP=(patsubst %.c,%.d,$(SRC))
CFLAGS=-g
(EXE):(OBJ)
	(CC)^ -o $@

$(DEP):%.d:%.c
	@set -e;
	rm -f $@;
	(CC) −M< > @.$$$;
	sed 's,$@.o[:]*,1.o @:,g′ <@. > $@;
	rm -f @.$$$

-include $(DEP)

clean:
	@rm (EXE)(OBJ) $(DEP) -f
```

Makefile 中，我们的依赖关系可能会需要包含一系列的头文件，比如，如果我们的 main.c 中有一句 `#include "defs.h"` ，那么我们的依赖关系应该是：

```makefile
main.o : main.c defs.h
```

大多数的`C/C++` 编译器都支持一个“-M”的选项，即自动找寻源文件中包含的头文件，并生成一个依赖关系。例如，如果我们执行下面的命令:`gcc -MM main.c`

输出是：`main.o : main.c defs.h`

编译器自动生成的依赖关系，这样一来，你就不必再手动书写若干文件的依赖关系，而由编译器自动生成了。需要提醒一句的是，如果你使用 GNU 的`C/C++`编译器，你得用`-MM`参数，不然，`-M`参数会把一些标准库的头文件也包含进来。

GNU 组织建议把编译器为每一个源文件的自动生成的依赖关系放到一个文件中，为每一个 name.c 的文件都生成一个 name.d 的 Makefile 文件，`.d` 文件中就存放对应`.c`文件的依赖关系。

我们可以写出 `.c` 文件和 `.d` 文件的依赖关系，并让 make 自动更新或生成 `.d` 文件，并把其
包含在我们的主 Makefile 中，这样，我们就可以自动化地生成每个文件的依赖关系了。一个模式规则来产生`.d`文件：

```makefile
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
```

所有的`.d`文件依赖于 `.c` 文件，`rm -f $@`的意思是删除所有的目标，也就是`.d`文件，第二行的意思是，为每个依赖文件`$<`，也就是`.c`文件生成依赖文件，`$@`表示模式`%.d`文件，如果有一个 C 文件是 name.c，那么 % 就是 name ，`$$$$` 意为一个随机编号，第二行生成的文件有可能是“name.d.12345”，第三行使用 sed 命令做了一个替换，第四行就是删除临时文件.

这个模式要做的事就是在编译器生成的依赖关系中加入`.d`文件的依赖，即把依赖关系：

```makefile
main.o : main.c defs.h
main.o main.d : main.c defs.h
```
我们的`.d`文件也会自动更新了，并会自动生成了，当然，你还可以在这个`.d`文件中加入的
不只是依赖关系，包括生成的命令也可一并加入，让每个`.d`文件都包含一个完赖的规则。一旦我们完成
这个工作，接下来，我们就要把这些自动生成的规则放进我们的主 Makefile 中。我们可以使用Makefile的“include”命令，来引入别的 Makefile 文件（前面讲过），例如：

```makefile
sources = foo.c bar.c
include $(sources:.c=.d)
```
上述语句中的`$(sources:.c=.d)`中的`.c=.d`的意思是做一个替换，把变量`$(sources)`所有`.c`
的字串都替换成`.d`，关于这个“替换”的内容，在后面我会有更为详细的讲述。当然，你得注意次序，
因为 include 是按次序来载入文件，最先载入的 .d 文件中的目标会成为默认目标。


