# shell语法("SHELL GRAMMAR")
# 1.Simple Commands 简单命令
simple command(简单命令) 是(可选的)一系列变量赋值, 紧接着是 blank(空格)分隔的词和重定向, 然后以一个 control operator 结束. 

* 第一个词指明了要执行的命令, 它被作为第0个参数. 其余词被作为这个命令的参数.
* simple command简单命令的**返回值**是它的退出状态, 或是`128+n`, 如果命令被signal(信号)`n`结束的话.

# 2.Pipelines 管道
pipeline(管道) 是一个或多个命令的序列，用字符 | 分隔。管道的格式是这样:

```bash
[time [-p]] [ ! ] command [ | command2 ... ]
```

命令command的**标准输出**通过管道连接到命令command2的**标准输入**。连接是在命令指定的任何重定向之前进行的(参见下面的 REDIRECTION 重定向)。

* 如果保留字`!`作为管道前缀，管道的退出状态将是最后一个命令的退出状态的**逻辑非值**。
* 否则，管道的退出状态就是最后一个命令的。 

shell在返回退出状态值之前，等待管道中的所有命令返回。

如 果保留字 time 作为管道前缀，管道中止后将给出执行管道耗费的用户和系统时间。选项`-p`将使输出符合 POSIX 指定的格式。环境变量`TIMEFORMAT`可以设置为一个格式字符串，指定时间信息应当如何显示；参见下面的 Shell Variables环境变量中TIMEFORMAT的讲述。

管道中的每个命令都作为单独的进程来执行(即，在**一个子 shell**中启动)。

# 3.List 序列

list(序列)是一个或多个管道，用操作符 `;`, `&`, `&&`, 或 `││` 分隔的序列, 并且可以选择用 `;`, `&`, 或 `<newline>`新行符结束.这些序列操作符中， `&&` 和 `││` 优先级相同，其次是 `;` 和 `&`, 它们的优先级是相同的。序列中可以有一个或多个新行符来分隔命令，而不是使用分号分隔。

* 如果一个命令是由控制操作符`&`结束的, shell 将在后台的子 shell 中执行 这个 命令。 shell不会等待命令执行结束，返回状态总是 0。
* 以分号 ; 分隔的命令会被顺序执行；shell 会等待每个命令依次结束。返回状态是最后执行的命令的返回状态。

控制操作符 `&&` 和 `││` 分别代表 `AND` 和 `OR` 序列。一个 AND 序列的形式是:`command1 && command2`

* `command2`只有在`command1返回0`时才被执行。

 一个 OR 序列的形式是`command1 ││ command2`

 * command2  只有在 command1 返回非 0 状态时才被执行。
AND 和 OR 序列的返回状态是序列中最后执行的命令的返回状态。

# 4.Compound Commands 复合命令

compound command(复合命令) 是如下情况之一：

* `(list)`:list序列将在一个**子shell中执行**。变量赋值和影响shell环境变量的内建命令在命令**结束后不会再起作用**。返回值是序列的返回值。
* `{ list; }`:list序列将在**当前shell环境**中执行。序列必须以一个新行符或分号结束。这种做法也称为 group command(命令组)。返回值是序列的返回值。注意与元字符 `(`和不同， `{` 和 `}`是reserved words(保留字)，必须出现在能够识别保留字的场合。由于它们不会产生断词(cause a wordbreak)，它们和序列之间必须用空格分开。
* `((expression))`:表达式expression将被求值。求值规则在下面的算术求值(ARITH-METIC EVALUATION)章节中描述。如果表达式的值非零，返回值就是0；否则返回值是 1。这种做法和 `let "expression"`等价。
* `[[ expression ]]`:返 回 0 或 1，取决于条件表达式 expression 求值的情况。表达式是由下面 CONDITIONAL EXPRESSIONS 条件表达式章节中描 述 的 原 语(pri-maries) 组成。 `[[`和`]]`中的词不会进行词的拆分和路径的扩展处理；而tilde 扩展，参数和变量扩展，算术扩展，命令替换，函数替换和引用的去除则都将进行。
  * 当使用 `==` 和 `!=` 操作符时，操作符**右边的字符串**被认为是一个模式，根据下面 Pattern Matching(模式匹配) 章节中的规则进行匹配。如果匹配则返回值是 0，否则返回 1。模式的任何部分可以被引用，强制使它作为一个字符串而被匹配。
  * 表达式可以用下列操作符结合起来。根据优先级的降序列出如下：
    * `(expression)`:返回表达式 expression 的值。括号可以用来提升操作符的优先级。
    * `! expression`:返回真，如果表达式 expression 返回假。
    * `expression1 && expression2`:返回真，如果表达式 expression1 和 expression2 都返回真。
    * `expression1 || expression2`:返回真，如果表达式 expression1 或者 expression2 二者之一返回真。
  * `&&(与)` 和 `||` 操作符不会对表达式 expression2 求值，如果expression1 可以决定整个条件表达式的返回值的话。
* `for name [ in word ] ; do list ; done`:in  之后的一系列词会被扩展，产生一个项目列表。变量 name 被依次赋以这个列表中的每个元素，序列 list 每次都被执行。如果 in word  被忽略，那么 for 命令遍历已设置的位置参数(positional parameter，参见下面的 PARAMETERS 参数)，为每一个执行一次序列 list。返回值是最后一个命令的返回值。如果 in 之后的词扩展的结果是空列表，就不会执行任何命令，返回值是 0。
* `for (( expr1 ; expr2 ; expr3 )) ; do list ; done`:首先，**算术表达式expr1**被根据下面 算术求值  (ARITHMETIC  EVALUATION)中的规则进行求值。然后算术表达式expr2被循环求值，直到它等于 0。每次 expr2 结果非零时，序列 list 都被执行，算术表达 式expr3 被求值。如果任何表达式被忽略，将被视为执行结果是 1。返回值是序列 list 中被执行的最后一个命令的返回值；或者是 false，如果任何表达式非法的话。
* `select name [ in word ] ; do list ; done`:in之后的一系列词会被扩展，产生一个项目列表。这个扩展后的词集合**被输出到标准错误上**，每个前面加上一个数字。如果in word 被忽略 ，将输出位置参数 (参见下面的 PARAMETERS 参数章节)。 `PS3`提示符将被显示出来，等待从标准输入得到一行输入。如果输入是一个数字且显示中有对应的词，那么变量 name 的值将设置为这个词。如果输入一个空行，那么词和提示符将再次显示出来。如果读入了一个 EOF，命令就结束。任何 其 他 值将设置变量 name 为空。读入的行保存为变量 REPLY.  序列list 在每次选择之后都会执行，直到执行了一个 break 命令。    select的退出状态是序列 list 中执行的最后一个命令的退出状态，如果没有执行命令就是 0。

```bash
#!/bin/bash

echo "What is your favourite OS?"
select var in "Linux" "Gnu Hurd" "Free BSD" "Other"; do
  break;
done
echo "You have selected $var"
```

运行结果：

```bash
What is your favourite OS?
1) Linux
2) Gnu Hurd
3) Free BSD
4) Other
#? 1
You have selected Linux
```
* `case word in [ [(] pattern [ | pattern ] ... ) list ;; ] ... esac`:case命令首先扩展word, 然后依次试着用每个pattern 来匹配它，使用 与路径扩展相同的匹配规则(参见下面的 Pathname Expansion 路径扩展章节)。如果找到一个匹配，相应的序列将被执行。找到一个匹配之 后，不会再尝试其后的匹配。如果没有模式可以匹配，返回值是 0。否则，返回序列中最后执行的命令的返回值。
* `if list; then list; [ elif list; then list; ] ... [ else list; ] fi`:序列 if list 被执行。如果退出状态是 0，then list 将被执行。否则， 每个 elif 将被一次执行，如果退出状态是 0，相应的 then list 将被执行，命令结束。否则，else list 将被执行，如果存在的话。退出状态是最后执行的命令的退出状态，或者是 0，如果所有条件都不满足。
* `while list; do list; done`和`until list; do list; done`:while命令不断地执行序列do list，直到序列中最后一个命令返回 0。until 命令和 while 命令等价，除了对条件的测试恰好相反；序 列do list执行直到序列中最后一个命令返回非零状态值。 while 和 until命令的退出状态是序列 do list 中最后一个命令的退出状态，或者是0，如果没有执行任何命令。
* `[ function ] name () { list; }`:这样可以定义一个名为 name 的函数。函数体 body 是包含在`{`和`}` 之间的命令序列 list。在指定将 name 作为一个命令运行的场合，这个 序列 将被执行。函数的退出状态是函数体最后执行的命令的退出状态(参见下面的 FUNCTIONS 函数章节)。

# 5.注释(COMMENTS)

在非交互的 shell 中或者使用内建命令 shopt 启用 了interactive_comments选 项的交互的 shell 中，以`#` 起始的词使得这个词和所有同一行上所有剩余的字符都被忽略。

没有启用 interactive_comments 选项的交互式 shell 不允许出现注释。这个选项在交互式 shell 中是默认启用的 (参见下面的 shell 内建命令(SHELL BUILTIN COMMANDS) 章节)。
