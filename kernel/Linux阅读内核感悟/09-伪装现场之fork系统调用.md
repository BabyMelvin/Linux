不仅进入系统调用时要伪装现场，fork 系统调用时返回时也需要伪装现场。因为是“无中生有”。

例如在 fork 创建新进程时，系统要保证新进程与旧进程一样，从相同的代码开始执行。比如：

```c
#include <stdio.h>
#include <unistd.h>
int main()
{
    pid_t pid;
    if ((pid=fork())>0) {
        printf("parent\n");
    } else if (pid == 0) {
        printf("child\n");
    } else {
        printf("error\n");
    }

    return 0;
}
```
在 fork()处，也就是执行 call 指令的过程中，产生了一个新进程。

```c
80483f0: e8 ef fe ff ff call 80482e4 <fork@plt>
```
执行完 fork()后，新旧两个进程都执行相同的指令：

```c
80483f5: 89 45 fc mov %eax,0xfffffffc(%ebp)
80483f8: 83 7d fc 00 cmpl $0x0,0xfffffffc(%ebp)
```
给人的感觉是新进程和旧进程一样，也是从 main 函数开始执行，只是执行到 fork()处，返回值不一样而已.

> 内核是如何做到的呢？

当内核执行 fork()系统调用时，内核堆栈里保存有很多寄存器的值。其中包括了 eax 等几个通用寄存器。通过`SAVE_ALL`和`RESTORE_ALL`这两个宏，pt_regs 结构体里的变量与对应的寄存器建立了一一对应关系。只要修改 pt_regs结构体里的变量，就可以达到修改系统调用结束后，进程执行那一刹那的寄存器值。

这样就简单了，因为 fork()时首先把 PCB 复制了一份`(sys_fork()->do_fork()->copy_process->dup_task_struct)`。所以内核堆栈 pt_regs 结构体里的值也跟着复制了一份之后，在`copy_thread()`中，把新进程的 pt_regs 的 eax，也就是新进程的返回值设置为 0，eip 不变。这样当新进程返回到用户态时，就仿佛刚执行完fork()返回 0。这是伪装用户态的现场。

> 还有内核态的现场呢？

`copy_thread()`中有以下代码：

```c
468 childregs = (struct pt_regs *) ((unsigned long) childregs - 8);
469 *childregs = *regs;
470 childregs->eax = 0;
471 childregs->esp = esp;
472
473 p->thread.esp = (unsigned long) childregs;
```
当子进程被调度到时，子进程处于内核态。经过 switch_to（）进程切换，eip 设置为 thread.eip，esp 设置为 thread.esp。 eip 是ret_from_fork，esp 是 childregs。伪装成刚执行完系统调用 fork 准备从内核态返回到用户态。

```c
474 p->thread.esp0 = (unsigned long) (childregs+1);
```
 thread.esp0 在 switch_to()->__switch_to()中，会通过 load_esp0 设置为tss_struct 中的 esp0。这个字段是在 CPU 运行状态变化时（如系统调用从用户态进入内核态）保存 0 级也就是内核态的堆栈指针。这样的话不是有两个地方有内核态堆栈指针了么？不过他们不矛盾。thread.esp 在进程切换时使用。thread.esp0 在 CPU 运行状态变化时使
用。当子进程返回到用户态后又发生中断或者系统调用。这时使用的是 esp0，内核堆栈是空的。正好指向(unsigned long) (childregs+1)。
如何同步，我也不清楚。

```c
476 p->thread.eip = (unsigned long) ret_from_fork;
```
 p->thread 中保存了进程切换所需的很多信息。包括内核态的 esp，eip
伪装的现场，给人的感觉是新进程在内核态正好快执行到 ret_from_fork 时由于中断等原因被抢占，所以在 fork()完毕后可能进行的进程调度中，新进程可能被调度到，在执行完 ret_from_fork 中的一些代码，返回到用户态后，开始执行 fork()之后的代码。