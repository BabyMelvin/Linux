# 基础

* `man bash`阅读
* `apropos` 是LINUX 系统中的一个命令，用来通过关键字查找定位手册页的名字和描述
* `help` 和 `help -d` 命令获取帮助信息.
* `type` 命令 来判断这个命令到底是可执行文件、shell 内置命令还是别名。
* 任务管理：`&`,ctrl-z,ctrl-c,`jobs`,`fg`,`bg`,`kill`
* `du -hs *`:硬盘使用情况概述.`df`，`mount`，`fdisk`，`mkfs`，`lsblk`
* inode 是什么（与 `ls -i` 和 `df -i` 等命令相关）。
* 网络管理工具:`ip` 或 `ifconfig`，`dig`
* 熟悉正则表达式，学会使用 `grep／egrep`，它们的参数中 `-i`，`-o`，`-v`，`-A`，`-B` 和 `-C` 这些是很常用并值得认真学习的。

# 日常使用

* 在 Bash 中,` ctrl-r`搜索命令行历史记录:按下按键之后，输入关键字便可以搜索，重复按下`ctrl-r`会向后查找匹配项，按下Enter键会执行当前匹配的命令，而按下右方向键会将匹配项放入当前行中，不会直接执行，以便做出修改
* 在 Bash 中，可以按下`ctrl-w`删除你键入的最后一个单词，`ctrl-u`可以删除行内光标所在位置之前的内容，`alt-b`和`alt-f`可以以单词为单位移动光标,`ctrl-a`可以将光标移至行首，`ctrl-e`可以将光标移至行尾，`ctrl-k`可以删除光标至行尾的所有内容，`ctrl-l`可以清屏。键入`man readline`可以查看 Bash 中的默认快捷键。内容有很多，例如`alt-`. 循环地移向前一个参数，而`alt-*` 可以展开通配符。
* 你的默认编辑器后（例如 `export EDITOR=vim`），`ctrl-x ctrl-e`会打开一个编辑器来编辑当前输入的命令。在 vi 风格下快捷键则是 escape-v。
* 键入history查看命令行历史记录，再用 `!n`（n 是命令编号）就可以再次执行。其中有许多缩写，最有用的大概就是`!$`， 它用于指代上次键入的参数，而`!!`可以指代上次键入的命令了（参考 man 页面中的“HISTORY EXPANSION”）。不过这些功能，你也可以通过快捷键 `ctrl-r` 和 `alt-.` 来实现。
* 使用`xargs`（ 或`parallel`）。他们非常给力。注意到你可以控制每行参数个数（`-L`）和最大并行数（`-P`）。如果你不确定它们是否会按你想的那样工作，先使用 `xargs echo` 查看一下。此外，使用 `-I{}` 会很方便`replace-str`。例如：
  
```bash
find . -name '.py' | xargs grep some_function
cat hosts | xargs -I{} ssh root@{} hostname
```

* `pstree -p`以一种优雅的方式展示进程树。
* 使用 `pgrep` 和 `pkill` `根据名字查找进程`或`发送信号`（-f 参数通常有用）。
* `man 7 signal`
* 使用 `nohup` 或 `disown` 使一个后台进程**持续运行**。
* 使用 `netstat -lntp` 或 `ss -plat` 检查哪些进程在监听端口（默认是检查 TCP 端口; 添加参数 `-u` 则检查 `UDP` 端口）或者 `lsof -iTCP -sTCP:LISTEN -P -n` (这也可以在 OS X 上运行)。
* `lsof` 来查看开启的套接字和文件。
* 使用 `uptime` 或 `w` 来查看系统已经运行多长时间。
* 使用 alias 来创建常用命令的快捷形式。例如：`alias ll='ls -latr'` 创建了一个新的命令别名 `ll`。
* 可以把别名、shell 选项和常用函数保存在 ~/.bashrc，具体看下这篇[文章](https://superuser.com/questions/183870/difference-between-bashrc-and-bash-profile/183980#183980)。这样做的话你就可以在所有 shell 会话中使用你的设定。
* 把环境变量的设定以及登陆时要执行的命令保存在 `~/.bash_profile`。而对于从图形界面启动的 shell 和 cron 启动的 shell，则需要单独配置文件。
* 当变量和文件名中包含**空格的时候要格外小心**。Bash 变量要用引号括起来，比如 `"$FOO"`。尽量使用 `-0` 或 `-print0` 选项以便用 `NULL` 来分隔文件名，例如 `locate -0 pattern | xargs -0 ls -al` 或 `find / -print0 -type d | xargs -0 ls -al`。如果 for 循环中循环访问的文件名含有空字符（空格、tab 等字符），只需用 `IFS=$'\n'` 把内部字段分隔符设为换行符。
* 在 Bash 脚本中，使用 `set -x` 去调试输出（或者使用它的变体 `set -v`，它会记录原始输入，包括多余的参数和注释）。尽可能地使用严格模式：使用 `set -e` 令脚本在发生错误时退出而不是继续运行；使用 `set -u` 来检查是否使用了未赋值的变量；试试 `set -o pipefail`，它可以监测管道中的错误。当牵扯到很多脚本时，使用 `trap` 来检测 `ERR` 和 `EXIT`。一个好的习惯是在脚本文件开头这样写，这会使它能够检测一些错误，并在错误发生时中断程序并输出信息：

```bash
set -euo pipefail
trap "echo 'error: Script failed: see failed command above'" ERR
```

* 在 Bash 脚本中，子shell（使用括号 (`...`)）是一种组织参数的便捷方式。一个常见的例子是临时地移动工作路径，代码如下：

```bash
 # do something in current dir
(cd /some/other/dir && other-command)
# continue in original dir
```

* 在 Bash 中，变量有许多的扩展方式。
  * `${name:?error message}` 用于检查变量是否存在。
  * 当 Bash 脚本只需要一个参数时，可以使用这样的代码 `input_file=${1:?usage: $0 input_file}`。
  * 在变量为空时使用默认值：`${name:-default}`。
  * 如果你要在之前的例子中再加一个（可选的）参数，可以使用类似这样的代码`output_file=${2:-logfile}`,如果省略了 $2，它的值就为空，于是 output_file 就会被设为 logfile。
  * 数学表达式：`i=$(( (i + 1) % 5 ))`。
  * 序列：`{1..10}`
  * 截断字符串：`${var%suffix}` 和 `${var#prefix}`。例如，假设 `var=foo.pdf`，那么 `echo ${var%.pdf}.txt` 将输出 `foo.txt`。
* 使用括号扩展（`{...}`）来减少输入相似文本，并自动化文本组合。这在某些情况下会很有用，例如`mv foo.{txt,pdf} some-dir`（同时移动两个文件），`cp somefile{,.bak}`（会被扩展成`cp somefile somefile.bak`）或者 `mkdir -p test-{a,b,c}/subtest-{1,2,3}`（会被扩展成所有可能的组合，并创建一个目录树）。
* 通过使用 `<(some command)` 可以将输出视为文件。例如，对比本地文件 `/etc/hosts` 和一个远程文件：` diff /etc/hosts <(ssh somehost cat /etc/hosts)`