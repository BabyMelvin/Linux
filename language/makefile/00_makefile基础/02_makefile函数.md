# 函数
## 1.1 字符串处理函数
* `$(subst <from>,<to>,<text> )`把字串`<text>`中的`<from>`字符串替换成`<to>`。
*  `$(patsubst <pattern>,<replacement>,<text> )`查找`<text>`中的单词（单词以“空格”、“Tab”或“回车”“换行”分隔）是否符合模式`<pattern>`，如果匹配的话，则以`<replacement>`替换。这里，`<pattern>`可以包括通配符`%`，表示任意长度的字串。如果`<replacement>`中也包含`%`，那么，`<replacement>`中的这个`%`将是`<pattern>`中的那个`%`所代表的字串。（可以用`\`来转义，以`%`来表示真实含义的`%`字符）返回：函数返回被替换过后的字符串。`$(var:<pattern>=<replacement> )`
* `$(strip <string> )`去掉`<string>`字串中开头和结尾的空字符。
* `$(findstring <find>,<in> )`在字串`<in>`中查找`<find>`字串。
* `$(filter <pattern...>,<text> )`以`<pattern>`模式过滤`<text>`字符串中的单词，保留符合模式`<pattern>`的单词。可以有多个模式
* `$(filter-out <pattern...>,<text> )`
* `$(sort <list> )`给字符串`<list>`中的单词排序（升序）。
* `$(word <n>,<text> )`取字符串`<text>`中第`<n>`个单词。（从一开始)
* `$(wordlist <s>,<e>,<text> )`从字符串`<text>`中取从`<s>`开始到`<e>`的单词串。`<s>`和`<e>`是一个数字。
* `$(words <text> )`统计`<text>`中字符串中的单词个数。
* `$(firstword <text> )`取字符串`<text>`中的第一个单词。
* `$(wildcard *.c)`返回当前目录中所有`.c`结尾的文件。
## 1.2 文件名操作函数
* `$(dir <names...> )`从文件名序列`<names>`中取出目录部分。目录部分是指最后一个反斜杠`/`之前的部分。如果没有反斜杠，那么返回`./`。
* `$(notdir <names...> )`从文件名序列`<names>`中取出非目录部分。
* `$(suffix <names...> )`从文件名序列`<names>`中取出各个文件名的后缀
* `$(basename <names...> )`返回文件名序列`<names>`的前缀序列，如果文件没有前缀，则返回空字串。
* `$(addsuffix <suffix>,<names...> )`把后缀`<suffix>`加到`<names>`中的每个单词后面。
* `$(join <list1>,<list2> )`把`<list2>`中的单词对应地加到`<list1>`的单词后面。`$(join aaa bbb , 111 222 333)`返回值是`aaa111 bbb222 333`。

## 1.3 foreach 函数
* `$(foreach <var>,<list>,<text> )`把参数`<list>`中的单词逐一取出放到参数`<var>`所指定的变量中，然后再执行`<text>`所包含的表达式。

```make
names := a b c d
files := $(foreach n,$(names),$(n).o)
# files=a.o b.o c.o d.o
```

## 1.4 if 函数
`$(if <condition>,<then-part> )`或`$(if <condition>,<then-part>,<else-part> )`
## 1.5 call 函数
`$(call <expression>,<parm1>,<parm2>,<parm3>...)`调用函数
```
reverse = $(1) $(2)
foo = $(call reverse,a,b) 
# foo的值就是“b a”。
```
## 1.6 origin 函数
`$(origin <variable> )` 返回变量的来源，可能值有：

`undefined` `default` `environment` `file` `command line` `override` `automatic` 

```makefile
ifeq ("$(origin O)","command line")
	KBUILD_OUTPUT :=$(0)
endif
```
* LANG是环境变量（environment），所有printenv打印出来的变量，origin该变量，应该都是environment。
* 对于普通的变量MYVAR，其origin为file；但是，如果在命令行指定了同样的变量（make -f hello.mk MYVAR="value_from_command_line")，则其origin为`command line`，MYVAR的值也会被命令行中的参数取代。可见，命令行参数的优先级大于本地变量。但是也有办法阻止命令行参数覆盖局部变量，即采用override。
* 变量myvar2是override类型的，其origin为`override`。可以看到，即使在命令行指定了`myvar2（make -f hello.mk myvar2="value from command line"）`，也不会覆盖hello.mk中的myvar2的值。
* 未定义的变量novar，其origin为`undefined`。
* make中预置的隐含变量（例如 CC），其origin为default。如果在mk文件中对CC进行重新赋值，则其origin为file，与普通本地变量一样。
