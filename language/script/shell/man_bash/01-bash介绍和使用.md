# 1.描述(DESCRIPTION)
Bash 是一个与 sh 兼容的命令解释程序，可以执行从标准输入或者文件中读取的
命令。 Bash 也整合了 Korn 和 C Shell (ksh 和 csh) 中的优秀特性。

Bash 的目标是成为遵循 IEEE POSIX Shell and  Tools  specification  (IEEE
Working Group 1003.2，可移植操作系统规约： shell 和工具) 的实现。

* `bash -c "help set"` 获取更多关于shell options.
* `bash -c help`更多关于内建命令。 built-in

# 2.选项(OPTIONS)

除了在`set`内建命令的文档中讲述的单字符选项 (option) 之外，bash 在启动
时还解释下列选项。

* `-c string` 如果有`-c`选项，那么命令将从string中读取。如果string后面有参数(argument)，它们将用于给位置参数(positional parameter，以`$0`起始) 赋值。
* `-i`如果有`-i`选项，shell将交互地执行 ( interactive )。
* `-l`选项使得bash以类似登录`shell (login shell)`的方式启动 (参见下面的 启动(**INVOCATION**) 章节)。
* `-r`如果有`-r`选项，shell 成为受限的 (`restricted`) (参见下面的受限的shell(RESTRICTED SHELL) 章节)。
* `-s`如果有`-s`选项，或者如果选项处理完以后，没有参数剩余，那么 命令将从标准输入读取。这个选项允许在启动一个交互 shell 时可以设置位置参数。
* `-D`向标准输出打印一个以`$`为前导的，以双引号引用的字符串列表。这是 在当前语言环境不是 C 或 POSIX 时，脚本中需要翻译的字符串。这个选项隐含了`-n`选项；不会执行命令。
* `[-+]O [shopt_option]`shopt_option 是一个 shopt 内建命令可接受的选项 (参见 下 面 的shell   内 建 命 令(SHELL  BUILTIN  COMMANDS)   章节)。
  * 如果有shopt_option，-O 将设置那个选项的取值； +O 取消它。
  * 如果没有给出 shopt_option，shopt 将在标准输出上打印设为允许的选项的名称和值。
  * 如果启动选项是 +O，输出将以一种可以重用为输入的格式显示
* `-- `:   `--` 标志选项的结束，禁止其余的选项处理。任何`--`之后的参数将作 为文件名和参数对待。参数 `-`与此等价。

Bash 也解释一些**多字节的选项**。在命令行中，**这些选项必须置于需要被识别的单字符参数之前**。

* `--dump-po-strings`等价于`-D`，但是输出是 GNU gettext po (可移植对象) 文件格式
* `--dump-strings`:等价于`-D`
* `--help` 在标准输出显示用法信息并成功退出
* `--init-file file`
* `--rcfile file`:如 果 shell 是交互的，执行 file 中的命令，而不是标准的个人初始化文件 `~/.bashrc` (参见下面的 启动(INVOCATION) 章节)
* `--login`等价于 `-l`
* `--noediting`:如果 shell 是交互的，不使用 GNU readline 库来读命令行
* `--noprofile`:不读取系统范围的启动文件 `/etc/profile` 或者任何个人初 始 化 文 件`~/.bash_profile`,  `~/.bash_login`,   或  `~/.profile` 。默认情况下，bash 在作为登录 shell 启动时读取这些文件 (参见下面的 启 动(INVOCATION) 章节)
* `--norc`:如果 shell 是交互的，不读取/执行个人初始化文件 `~/.bashrc` 这个选项在 shell 以 sh 命令启动时是默认启用的
* `--posix`如果默认操作与 POSIX 1003.2 标准不同的话，改变 bash 的行为来符合标准 (posix mode)
* `--restricted`:shell 成为受限的 (参见下面的 受限的shell(RESTRICTED SHELL) 章节)
*  `--rpm-requires`:产生一个为使脚本运行，需要的文件的列表。这个选项包含了`-n`选项。它 是为了避免进行编译期错误检测时的限制－－ Backticks, [] tests,还有 evals 不会被解释，一些依赖关系可能丢失
*  `--verbose`:等价于 -v
* `--version`:在标准输出显示此 bash 的版本信息并成功退出。

# 3.参数(ARGUMENTS)
如果选项处理之后仍有参数剩余，并且没有指定 `-c` 或 `-s` 选项

* 第一个参数将假定为一个包含 shell 命令的**文件的名字**。
  * 如果 bash 是以这种方式启动的，`$0`将设置为这个文件的名字，**位置参数**将设置为剩余的其他参数
  
**Bash**从这个**文件**中读取并执行命令，然后退出。 

* Bash 的退出状态是脚本中执行的最后一个命令的退出状态。
  * 如果没有执行命令，退出状态是0。

尝试的步骤是先试图打开在当前目录中的这个文件，接下来，如果没有找到，shell 将搜索脚本的PATH环境变量中的路径。

# 4.启动(INVOCATION)
login shell 登录 shell，参数零的第一个字符是`-`，或者 启 动 时 指 定 了`--login`选项的shell。

interactive交互的 shell，是一个启动时**没有指定非选项的参数**，并且没有指定`-c`选项，标准输出和标准输入都连接到了终端 (在`isatty(3)`中 判 定)的shell ，或者启动时指定了`-i`选项的shell。

* 如果bash是交互的， `PS1`环境变量将被设置，并且`$-`包含 i ，允许一个shell脚本或者一个启动文件来检测这一状态。

下列段落描述了 bash 如何执行它的启动文件。如果这些启动文件中的任一个存在但是不可读取， bash 将报告一个错误。文件名中的波浪号 (`~`,tilde)  将像EXPANSION章节中Tilde Expansion段描述的那样展开。

## 4.1 交互登录
当  bash 是作为交互的登录 shell 启动的，或者是一个非交互的 shell 但是指定了`--login`选项，它首先读取并执行`/etc/profile`中的命令，只要那个文件存 在 。 读 取 那 个 文 件 之 后 ， 它以如下的顺序查找 `~/.bash_profile`,`~/.bash_login`, 和 `~/.profile`, 从存在并且可读的第一个文件中读取并执行 其 中的命令。 `--noprofile` 选项可以用来在 shell 启动时阻止它这样做。
 
当一个登录 shell 退出时， bash 读取并执行文件`~/.bash_logout`中的命令，只要它存在。

当一个交互的 shell 但不是登录 shell 启动时， bash 从文件`~/.bashrc`中读取并执行命令，只要它存在。可以用`--norc`选项来阻止它这样做。 `--rcfile file`选项将强制 bash 读取并执行文件 file 中的命令，而不是 `~/.bashrc`的。

## 4.2 非交互方式启动
当 bash 以非交互的方式启动时，例如在运行一个 shell 脚本时，它在环境中查找变量`BASH_ENV`，如果它存在则将它的值展开，使用展开的值作为一个文件的名称，读取并执行。 Bash运作的过程就如同执行了下列命令：

```bash
if [ -n "$BASH_ENV" ]; then . "$BASH_ENV"; fi
```
但是没有使用`PATH`变量的值来搜索那个文件名。

## 4.3 sh启动
如 果 bash 以名称 sh 启动，它试图模仿 (mimic) sh 历史版本的启动过程，尽可能地相似，同时也遵循 POSIX 标准。当作为交互式登录 shell 启动时,或者是非 交 互 但 使 用 了  --login   选项启动的时候,它首先尝试读取并执行文件`/etc/profile` 和`~/.profile`, 中的命令.  选项`--noprofile`用于避免这种 行为.当使用命令 sh 来启动一个交互式的 shell 时, bash 查找环境变量`ENV`, 如果有定义的话就扩展它的值,然后使用扩展后的值作为要读取和执行的文件 的 名称. 由于使用sh启动的 shell不会读取和执行任何其他的启动文件,选项
--rcfile 没有意义.使用名称sh启动的非交互的shell不会读取任何其他启动文件.当以sh启动时, bash 在读取启动文件之后进入**posix模式**.

## 4.4 posix 模式启动
当 bash以posix模式启动时,(和使用`--posix`命令行参数效果相同),它遵循POSIX 标准.  这种模式下,交互式 shell 扩展`ENV`环境变量的值,读取并执行以扩展后值为文件名的配置文件.  不会读取其他文件.

## 4.5 bash 本身
Bash 试着检测它是不是由远程 shell 守护程序,通常为`rshd`启动的.如果bash发现它是由`rshd`启动的,它将读取并执行`~/.bashrc`文件中的命令, 只要这个文件存在并且可读.如果以 sh 命令启动,它不会这样做.  选项 `--norc`可以用来阻止这种行为,选项`--rcfile`用来强制读取另一个文件,但是通常rshd不会 允许它们, 或者用它们来启动 shell.

如果 shell 是以与真实用户(组) id 不同的有效用户(组) id 来启动的, 并且没有 - 选项,那么它不会读取启动文件, 也不会从环境中继承 shell 函数. 环境变量中如果出现 SHELLOPTS，它将被忽略.有效用户 id 将设置为真实用户 id.  如果启动时给出了选项,那么启动时的行为是类似的, 但是不会重置有效用户 id.

# 5.定义(DEFINITIONS)

下列定义在文档余下部分中通用.

* blank 空白:一个空格或是 tab .
* word 词:一个字符序列, shell 将它们视为一个结构单元. 也称为一个 token片段。
* name 名称:一个只由字母,数字和下划线构成的词,并且以字符或下划线起始. 也称为一个 word identifier标识符.
* metacharacter 元字符:一个字符, 如果不是引用的话, 将成为词的分隔符. 它是如下字符之一:`|`  `&` `;` `(` `)` `<` `>` `space` `tab`
* control operator 控制操作符: 一个 token(标识), 拥有控制功能. 它是如下符号之一:`||` `&` `&&` `;` `;;` `( )` `|` `<newline>`

# 6. 保留字("RESERVED WORDS")

Reserved  words( 保留字) 是对 shell 有特殊意义的词.  下列词被识别为保留的, 如果不是引用, 并且不是一个简单命令的起始词 (参见 下 面 的  shell 语法("SHELL GRAMMAR") ), 也不是 case 或者 for 命令的第三个词:

```bash
!  case  do done elif else esac fi for function if in select then until
       while { } time [[ ]]
```