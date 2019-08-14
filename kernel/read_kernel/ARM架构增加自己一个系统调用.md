# 添加自己的系统调用
开始之前必须说明一下，实际上，是没有任何必要也不应该自己添加一个系统调用，这只是便于理解系统调用的一种方式

# 1.内核中增加一个新的函数
    
```
//路径：arch/arm/kernel/sys_arm.c
asmlinkage int sys_add(int x, int y)
{
	printk("enter %s\n", __func__);
	return x+y;
}
```

**注意**：该函数必须是全局的，内核编程不能进行浮点数运算`float`、`double`

# 2.更新unistd.h

```
//路径:arch/arm/include/asm/unistd.h
#define __NR_add （__NR_SYSCALL_BASE + 366）
```

注意：只能添加在所有系统调用号的最后面

# 3.更新系统调用表

```c
//路径：arch/arm/kernel/calls.S
CALL(sys_add)
```
**注意**：只能添加在所有CALL的最后面，并且与**2**的调用号相对应。否则一定使系统调用表混乱。

# 4.重新编译内核，加载新的内核
# 5.编写一个测试程序

```c
#include <unistd.h>
#include <sys/syscall.h>

int main(void)
{
	syscall(366, x, y);
	return 0;
}
# 添加自己的系统调用
开始之前必须说明一下，实际上，是没有任何必要也不应该自己添加一个系统调用，这只是便于理解系统调用的一种方式

# 1.内核中增加一个新的函数
    
```
//路径：arch/arm/kernel/sys_arm.c
asmlinkage int sys_add(int x, int y)
{
	printk("enter %s\n", __func__);
	return x+y;
}
```

**注意**：该函数必须是全局的，内核编程不能进行浮点数运算`float`、`double`

# 2.更新unistd.h

```
//路径:arch/arm/include/asm/unistd.h
#define __NR_add （__NR_SYSCALL_BASE + 366）
```

注意：只能添加在所有系统调用号的最后面

# 3.更新系统调用表

```c
//路径：arch/arm/kernel/calls.S
CALL(sys_add)
```
**注意**：只能添加在所有CALL的最后面，并且与**2**的调用号相对应。否则一定使系统调用表混乱。

# 4.重新编译内核，加载新的内核
# 5.编写一个测试程序

```c
#include <unistd.h>
#include <sys/syscall.h>

int main(void)
{
	syscall(366, x, y);
	return 0;
}
```
