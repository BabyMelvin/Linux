# 1.使用条件判断
条件表达式可以是比较变量的值，或是比较变量和常量的值。

```make
libs_for_gcc = -lgnu
normal_libs =
foo: $(objects)
ifeq ($(CC),gcc)
	$(CC) -o foo $(objects) $(libs_for_gcc)
else
	$(CC) -o foo $(objects) $(normal_libs)
endif
```

比较参数 arg1 和 arg2 的值是否相同。当然，参数中我们还可以使用 make 的函数

```make
# 
ifeq ($(strip $(foo)),)
<text-if-empty>
endif
```
**第二个条件**关键字是`ifneq`。语法是

```make
ifneq (<arg1>, <arg2>)
ifneq '<arg1>' '<arg2>'
ifneq "<arg1>" "<arg2>"
ifneq "<arg1>" '<arg2>'
ifneq '<arg1>' "<arg2>"
```

**第三个条件**关键字是 ifdef 

```make
ifdef <variable-name>
```
如果变量 `<variable-name>`的值非空，那到表达式为真。否则，表达式为假。

```make
bar =
foo = $(bar)
ifdef foo
	frobozz = yes
else
	frobozz = no
endif
```

**第四个条件**关键字是 ifndef 

# 2.函数
函数的调用语法

```
$(<function> <arguments>)
#或
${<function> <arguments>}
```
<function> 就是函数名，make 支持的函数不多。<arguments> 为函数的参数，参数间以逗
号 分隔，而函数名和参数之间以“空格”分隔。

为了风格的统一，函数和变量的括号最好一样，如使用`$(subst a,b,$(x))`这样的形式，而不是`$(subst a,b, ${x})`的形式。因为统一会更清楚，也会减少一些不必要的麻烦。

## 2.1 字符串处理函数
* `$(subst <from>,<to>,<text> )`把字串`<text>`中的`<from>`字符串替换成`<to>`。
	* `$(subst ee,EE,feet on the street)`:把 feet on the street 中的 ee 替换成 EE ，返回结果是 fEEt on the strEEt 
*  `$(patsubst <pattern>,<replacement>,<text> )`查找`<text>`中的单词（单词以“空格”、“Tab”或“回车”“换行”分隔）是否符合模式`<pattern>`，如果匹配的话，则以`<replacement>`替换。这里，`<pattern>`可以包括通配符`%`，表示任意长度的字串。如果`<replacement>`中也包含`%`，那么，`<replacement>`中的这个`%`将是`<pattern>`中的那个`%`所代表的字串。（可以用`\\`来转义，以%来表示真实含义的%字符返回：函数返回被替换过后的字符串。`$(var:<pattern>=<replacement>`
	* `$(patsubst %.c,%.o,x.c.c bar.c)`:把字串 x.c.c bar.c 符合模式` %.c` 的单词替换成 `%.o` ，返回结果是 x.c.o bar.o
* `$(strip <string> )`去掉`<string>`字串中开头和结尾的空字符。
	* `$(strip a b c )`:把字串 a b c ` ` 去到开头和结尾的空格，结果是` `a b c 。
* `$(findstring <find>,<in> )`在字串`<in>`中查找`<find>`字串。
	* `$(findstring a,a b c)`:第一个函数返回 a 字符串
* `$(filter <pattern...>,<text> )`以`<pattern>`模式过滤`<text>`字符串中的单词，保留符合模式`<pattern>`的单词。可以有多个模式

```make
sources := foo.c bar.c baz.s ugh.h
foo: $(sources)
	cc $(filter %.c %.s,$(sources)) -o foo
```
`$(filter %.c %.s,$(sources)) `返回的值是 foo.c bar.c baz.s.

* `$(filter-out <pattern...>,<text> )`

```make
objects=main1.o foo.o main2.o bar.o
mains=main1.o main2.o
```
* `$(filter-out $(mains),$(objects))`返回值是 foo.o bar.o 。

* `$(sort <list> )`给字符串`<list>`中的单词排序（升序）。
* `$(word <n>,<text> )`取字符串`<text>`中第`<n>`个单词。（从一开始)
* `$(wordlist <s>,<e>,<text> )`从字符串`<text>`中取从`<s>`开始到`<e>`的单词串。`<s>`和`<e>`是一个数字。
* `$(words <text> )`统计`<text>`中字符串中的单词个数。
* `$(firstword <text> )`取字符串`<text>`中的第一个单词。
* `$(wildcard *.c)`返回当前目录中所有`.c`结尾的文件。

## 2.2 文件名操作函数
* `$(dir <names...> )`从文件名序列`<names>`中取出目录部分。目录部分是指最后一个反斜杠`/`之前的部分。如果没有反斜杠，那么返回`./`。
 	* `$(dir src/foo.c hacks)` 返回值是 `src/ ./ `。
* `$(notdir <names...> )`从文件名序列`<names>`中取出非目录部分。
	*  `$(notdir src/foo.c hacks)` 返回值是 `foo.c hacks`
* `$(suffix <names...> )`从文件名序列`<names>`中取出各个文件名的后缀
 	* `$(suffix src/foo.c src-1.0/bar.c hacks)` 返回值是` .c .c`
* `$(basename <names...> )`返回文件名序列`<names>`的前缀序列，如果文件没有前缀，则返回空字串。
 	* `$(basename src/foo.c src-1.0/bar.c hacks) `返回值是 `src/foo src-1.0/bar hacks` 。
* `$(addsuffix <suffix>,<names...> )`把后缀`<suffix>`加到`<names>`中的每个单词后面。
 	* `$(addsuffix .c,foo bar) `返回值是` foo.c bar.c `。
* `$(addprefix <prefix>,<names...>)`加前缀函数
 	* `$(addprefix src/,foo bar) `返回值是` src/foo  src/bar `。
* `$(join <list1>,<list2> )`把`<list2>`中的单词对应地加到`<list1>`的单词后面。`$(join aaa bbb , 111 222 333)`返回值是`aaa111 bbb222 333`。

## 2.3 foreach 函数
* `$(foreach <var>,<list>,<text> )`把参数`<list>`中的单词逐一取出放到参数`<var>`所指定的变量中，然后再执行`<text>`所包含的表达式。

```make
names := a b c d
files := $(foreach n,$(names),$(n).o)
# files=a.o b.o c.o d.o
```

## 2.4 if 函数
`$(if <condition>,<then-part> )`或`$(if <condition>,<then-part>,<else-part> )`

## 2.5 call 函数
`$(call <expression>,<parm1>,<parm2>,<parm3>...)`调用函数

```make
reverse = $(1) $(2)
foo = $(call reverse,a,b) 
# foo的值就是“b a”。
```

## 2.6 origin 函数
`$(origin <variable> )` origin 函数不像其它的函数，他并不操作变量的值，他只是告诉你你的这个变量是哪里来的？
`<variable> `是变量的名字，不应该是引用

* undefined 如果 `<variable>` 从来没有定义过，origin 函数返回这个值 undefined
* default 如果 `<variable>` 是一个默认的定义，比如“CC”这个变量，这种变量我们将在后面讲述。
* environment 如果 `<variable> `是一个环境变量，并且当 make 被执行时，`-e `参数没有被打开。
* file 如果 `<variable> `这个变量被定义在 make 中。
* command line 如果` <variable> `这个变量是被命令行定义的。
 * override 如果 `<variable>` 是被 override 指示符重新定义的。
* automatic 如果 `<variable> `是一个命令运行中的自动化变量。

```make
ifdef bletch
	ifeq "$(origin bletch)" "environment"
		bletch = barf, gag, etc.
	endif
endif
```

## 2.7 控制 make 的函数
你需要检测一些运行 make 时的运行时信息，并且根据这些信息来决定，你是让 make 继续执行，还是停止。

```make
$(error <text ...>)
```
所以如果你把其定义在某个变量中，并在后续的脚本中使用这个变量，那么也是可以的

```make
ifdef ERROR_001
	$(error error is $(ERROR_001))
endif
```
`$(warning <text ...>)`这个函数很像 error 函数，只是它并不会让 make 退出，只是输出一段警告信息，而 make 继续执行。
