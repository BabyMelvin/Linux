Use Bash Strict Mode (Unless You Love Debugging)

# Use Bash Strict Mode (Unless You Love Debugging)
Let's start with the punchline. Your bash scripts will be more robust, reliable and maintainable if you start them like this:
```bash
#!/bin/bash

# 1.等价于set -e;set -u;set -o pipefail
set -euo pipefail

# 2.IFS(Internal Field Separator)
IFS=$'\n\t'
```
I call this the unofficial bash strict mode. This causes bash to behave in a way that makes many classes of subtle bugs impossible. You'll spend much less time debugging, and also avoid having unexpected complications in production.

There is a short-term downside: these settings make certain common bash idioms harder to work with. Most have simple workarounds, detailed below: jump to Issues & Solutions. But first, let's look at what these obscure lines actually do.

* `set -e`:非0退出，报错。结束脚本
* `set -u`:未定义变量，报错，结束脚本

```bash
#!/bin/bash
firstName="Aaron"
# 未定义，则结束
fullName="$firstname Maxwell"
echo "$fullName"
```

* `set -o pipefail`:管道中只要有错误，就作为管道最终的返回值。
  * 默认不设置是最后命令的执行结果

```bash
#!/bin/bash

names = (
  "Aaron Maxwell"
  "Wayne Gretzky"
  "David Beckham"
  "Anderson da Silva" 
)
echo "With default IFS value..."
for name in ${names[@]}; do
  echo "$name"
done

echo ""
echo "With strict-mode IFS value..."
IFS=$'\n\t'
for name in ${names[@]}; do
  echo "$name"
done
```

输出结果

```
With default IFS value...
Aaron
Maxwell
Wayne
Gretzky
David
Beckham
Anderson
da
Silva

With strict-mode IFS value...
Aaron Maxwell
Wayne Gretzky
David Beckham
Anderson da Silva
```

# 常见问题
# source 一个nonconforming文档

```
# set +u disables this variable strictness, and set -u enables it
set +u
source some/bad/file.env
set -u
```

# 使用默认参数`:-`

```bash
# Variable $foo has not been set. In strict mode, next line triggers an error.
bar=$foo

# ${VARNAME:-DEFAULT_VALUE} evals to DEFAULT_VALUE if VARNAME undefined.
# So here, $bar is set to "alpha":
bar=${foo:-alpha}

# Now we set foo explicitly:
foo="beta"

# ... and the default value is ignored. Here $bar is set to "beta":
bar=${foo:-alpha}

# To make the default an empty string, use ${VARNAME:-}
empty_string=${some_undefined_var:-}
```

Under strict mode, you need to use this for all positional parameter references:

```bash
#!/bin/bash
set -u
name=${1:-}
if [[ -z "$name" ]]; then
    echo "usage: $0 NAME"
    exit 1
fi
echo "Hello, $name"
```

Intentionally Undefined Variables:explicitly set the variable to an empty string early on in the script, before any reference to it:

```bash
someVar=""
# ...
# a bunch of lines of code that may or may not set someVar
# ...
if [[ -z "$someVar" ]]; then
# ...
```

# Commands You Expect To Have Nonzero Exit Status
知道可能失败，但是不想结束整个脚本,有两种选择,最简单方法是命令后面添加`|| true`

```bash
# "grep -c" reports the number of matching lines. If the number is 0,
# then grep's exit status is 1, but we don't care - we just want to
# know the number of matches, even if that number is zero.

# Under strict mode, the next line aborts with an error:
count=$(grep -c some-string some-file)

# But this one behaves more nicely:
count=$(grep -c some-string some-file || true)

echo "count: $count"
```
This short-circuiting with the boolean operator makes the expression inside $( ... ) always evaluate successfully.

But what if you want to know the return value of a command, even if that return value is nonzero? Then you can temporarily disable the exit-immediately option:

```bash
# We had started out this script with set -e . And then...
set +e
count=$(grep -c some-string some-file)
retval=$?
set -e

# grep's return code is 0 when one or more lines match;
# 1 if no lines match; and 2 on an error. This pattern
# lets us distinguish between them.
echo "return value: $retval"
echo "count: $count"
```

# Essential Clean-up
you define a bash function that performs the clean-up or release of resources, and then register the function to be automatically invoked on exit. Here's how you would use it to robustly clean up a scratch directory:

```bash
scratch=$(mktemp -d -t tmp.XXXXXXXXXX)
function finish {
    rm -rf "$scratch"
}
trap finish EXIT

# Now your script can write files in the directory "$scratch".
# It will automatically be deleted on exit, whether that's due
# to an error, or normal completion.
```

# Short-Circuiting Considerations
The whole point of strict mode is to convert many kinds of hidden, intermittent, or subtle bugs into immediate, glaringly obvious errors. Strict mode creates some special concerns with short-circuiting, however. By "short-circuiting", I mean chaining together several commands with `&&` or `||` - for example:

```bash
# Prints a message only if $somefile exists.
[[ -f "$somefile" ]] && echo "Found file: $somefile"
```
The first short-circuit issue can happen when chaining three or more commands in a row:

```bash
first_task && second_task && third_task
# And more lines of code following:
next_task
```
When people write COND && COMMAND, typically they mean "if COND succeeds (or is boolean true), then execute COMMAND. Regardless, proceed to the next line of the script." It's a very convenient shorthand for a full "if/then/fi" clause. But when such a construct is the last line of the file, strict mode can give your script a surprising exit code:

```bash
% cat short-circuit-last-line.sh
#!/bin/bash
set -euo pipefail

# omitting some lines of code ...

# print a message only if $somefile exists
# note structure:CND && COMMAND
[[ -f "$somefile" ]] && echo "Found file:$somefile"

%./short-circuit-last-line.sh
%echo $?
1
```
When the script got to the last line, $somefile did not in fact exist. Thus COND evaluated false, and COMMAND was not executed - which is what should happen. But the script exits with a non-zero exit code, which is a bug: the script in fact executed correctly, so it really should exit with 0. In fact, if the last line of code is something else, that's exactly what we get:

```bash
% cat short-circuit-before-last-line.sh
#!/bin/bash
set -euo pipefail
# omitting some lines of code...

# Prints a message only if $somefile exists.
# Structure: COND && COMMAND
# (When we run this, $somefile will again not exist,
# so COMMAND will not run.)
[[ -f "$somefile" ]] && echo "Found file: $somefile"

echo "Done."

% ./short-circuit-before-last-line.sh
Done.
% echo $?
0
```
What's going on? It turns out the -e option has a special exception in short-circuiting expressions like this: if COND evaluates as false, COMMAND will not run, and flow of execution will proceed to the next line. However, the result of the entire line - the entire short-circuit expression - will be nonzero, because COND was. And being the last line of the script, that becomes the program's exit code.

This is the kind of bug we don't want to have, since it can be subtle, non-obvious, and hard to reproduce. And it's mainly difficult to deal with because it shows up only if it's the last command of the file; on any other line, it is well-behaved and causes no problem. It's easy to forget this in normal, day-to-day development, and have it slip through the cracks. For example, what if you delete an innocuous-looking echo statement from the end, making a short-circuit line now be last?

In the specific example above, we can expand the expression in a full "if" clause. This is perfectly well-behaved:

```bash
# Prints a message only if $somefile exists.
if [[ -f "$somefile" ]]; then
    echo "Found file: $somefile"
fi

# If COND is a command or program, it works the same. This:
first_task && second_task

# ... becomes this:
if first_task; then
     second_task
fi
```
What's the final, full solution? You could decide to trust yourself and your teammates to always remember this special case. You can all freely use short-circuiting, but **simply don't allow a short-circuit expression to be on the last line of a script**, for anything actually deployed.his may work 100% reliably for you and your team, but I don't believe that is the case for myself and many other developers. Of course, some kind of linter or commit hook might help.

Perhaps a safer option is to decide never to use short-circuiting at all, and always use full if-statements instead. This may be unappealing, though: short-circuiting is convenient, and people like to do it for a reason. For now, I'm still seeking a more satisfactory solution. Please contact me if you have a suggestion.