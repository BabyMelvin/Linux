# ARM Linux系统调用详细分析

## 1.系统调用与软中断关系
## 1.1EABI与OABI方式的系统调用
在linux中系统调用是通过软中断实现，应用层通过`int syscall(int number, ...);`,接口将syscall number 放在提前约定好的位置，然后产生软中断swi，并跳转到中断向量表执行。没有接触过的人可能会有疑问：kernel是和什么“提前约定”的，答案是**编译器**。

应用层的系统调用具体实现是在编译器的链接库里，打开你的编译器所在地址，你会发现编译器的工具包里有`lib/`和`include/`文件夹，这里存放的就是kernel正常工作需要的链接库，也就是说在应用层编写的代码发起一个open的syscall，它到底向kernel发出了什么样的命令是由编译器决定的。由于这一块我并没有具体了解过，这里只做一个不精确的描述，不再具体展开。

* 早期linux采用OABI方式传递系统调用的number。流程如下
	* 1.将number放在软终断的中断号内( eg：`swi __NR_OABI_SYSCALL_BASE | number`)。
	* 2.在跳转到软中断后，再在之前已经执行过的指令所在内存地址中获取当时的指令(`ldr r10, [lr, #-4]`)，并解析出syscall number。

很明显**读取已经执行过的指令的内存地址处的数据**是一种很原始的行为，现在用的不多，但代码依然保留了下来。

* 现在linux采用新的EABI（Embedded）方式传递系统调用的number。比如现在的android编译器`arm-linux-androideabi-gcc`。这种新的系统调用方式流程如下：
	* 1.将syscall number 存储到r7寄存器中。
	* 2.执行swi指令跳转到软中断处执行，并从r7中获取syscall number。

注意这里提到的软中断是 supervisor call exception（svc），由于原来名字是software interrupt（swi）一直沿用软中断的叫法，但与linux kernel的softirq实现是完全不同的两个概念。前者是借助硬件实现的一种，使软件可以进入特权模式的指令，后者是kernel完全以软件实现的短暂延时的轻量级处理（出于软中断上下文）

## 2.软中断到syscall代码分析

由于需要涉及汇编代码，做一下说明，本文只分析arm架构下的系统调用流程，下面是arm架构的准备知识：

* 1.应用运行于arm的usr_mode，kernel运行在arm的svc_mode，这两种模式共用`r0-r12`寄存器以及PC(r15)，usr_mode下没有SPSR（r17）寄存器，svc_mode 下有自己的sp(r13),lr(r14),spsr(r17)。
* 2.从usr_mode发生软中断(swi)跳转到**svc_mode**时，原来的cpsr会被arm core自动备份到spsr_svc中，以供从svc返回时恢复usr完整的上下文(`r0-r12+sp+lr+pc+cpsr`)。
*  3.arm架构下linux使用的是**满减栈**，栈顶指针sp永远指向栈顶的元素，栈顶向低地址方向生长。
*  4.无论采取多少级的流水，arm架构下前三级是fetch,decode,execute。PC永远指向fetch地址。
*  5.`lr地址` "可以认为" 是指向跳转前的decode地址。


整个中断的过程，不是本文关注重点，我们只从**进入软中断后开始分析**。涉及文件：

```
arch/arm/kernel/entry-armv.S //中断向量表定义位置，本文不分析
arch/arm/kernel/entry-common.S //软中断真正入口地址
arch/arm/kernel/entry-header.S //部分定义
```

**entry-header.S**：

```asm
/*
	syscall允许我们使用理论寄存器有7个，r0 - r6
	r7是给thumb mode 使用的
*/
scno .req r7   @syscall number
tbl  .req r8   @syscall table pointer
why  .req r8   @Linux syscall (!=0)
tsk  .req r9   @current thread_info进程栈的栈底，存储着该进程的基本信息如id 调度优先级 等等
```

**entry-common.S**：

* 1.将中断发生前的寄存器状态备份入栈

```asm
ENTRY(vector_swi)
/*****将中断发生前的寄存器状态备份入栈********/
	/*在栈上开辟18*4的空间备份跳转之前的寄存器值*/
	sub sp, sp, #S_FRAME_SIZE
	/*Call r0-r12备份共用寄存器*/
	stmia sp, {r0 - r12}
/*r8指向栈空间中存pc的位置*/
ARM(add r8, sp, #S_PC)
/*call sp, lr*/
/*备份usr_mode的r13 r14 这里^表示被备份的sp lr 是usr_mode的寄存器不是目前所在svc_mode的寄存器*/ 
ARM(stmdb r8,{sp,lr}^)

	// called from non-FIQ mode, so ok.从usr发生软中断跳转到svc时，原来的cpsr会被存储到spsr_svc中
	mrs r8,spsr
	str    lr, [sp, #S_PC]  // Save calling PC 备份pc
	str    r8, [sp, #S_PSR] // Save CPSR 备份跳转前的cpsr到栈上
// Save OLD_R0 备份r0到栈空间中r17的位置（因为没有SPSR 所以这里多备份了一个r0,估计是为了标准化）
	str    r0, [sp, #S_OLD_R0] 
	zero_fp //清空fp寄存器r11
```
* 2.取得syscall number

```asm
/**********取得syscall number**********/
#if defined(CONFIG_OABI_COMPAT)
	/*这里是OABI方式的syscall number传递
     * If we have CONFIG_OABI_COMPAT then we need to look at the swi
     * value to determine if it is an EABI or an old ABI call.
     */
	ldr    r10, [lr, #-4] // get SWI instruction 利用lr取得swi指令的内容
    //跳转前code段的内存{地址：内容：流水级},从下面的内存分布可以清楚看出lr - 4的含义
    //{0x8:xxx:fetch<--pc},{0x4:xxx:decode<--lr},{0x0:swi num:exe}
#ifdef CONFIG_CPU_ENDIAN_BE8
	rev r10, r10 // little endian instruction 考虑到CPU的大小端的设计。
#endif

#elif defined(CONFIG_AEABI)
/*EABI方式
     * Pure EABI user space always put syscall number into scno (r7).注意 scno 与 r7等价   
     */
#elif defined(CONFIG_ARM_THUMB)
	/* Legacy ABI only, possibly thumb mode. */
    tst    r8, #PSR_T_BIT            @this is SPSR from save_user_regs
    addne    scno, r7, #__NR_SYSCALL_BASE    @ put OS number in
    ldreq    scno, [lr, #-4]
#else
	/* Legacy ABI only. */
    ldr    scno, [lr, #-4]            @get SWI instruction
#endif
```
* 3.为进入syscall具体函数做准备，主要是使调用符合ATPCS规范

```asm
#ifdef CONFIG_ALIGNMENT_TRAP
	ldr ip, __cr_alignment
	ldr ip, [ip] @以上两句将ip(r12)清0
    mcr    p15, 0, ip, c1, c0        // update control register 协处理器控制指令
#endif
	enable_irq  //放开中断屏蔽
/*
	栈顶指针低13位清0，显然高位是base地址，低13位看来是整个进程栈的偏移地址，由此可以得到栈的base地址
	.macro get_thread_info, rd
		mov \rd, sp, lsr #13
		mov \rd, \rd, lsl #13
	.endm
*/
	get_thread_info tsk @tsk(r9)存储了栈底的地址
	adr    tbl, sys_call_table @ load syscall table pointer 将sys_call_table地址加载到tbl(r8)
#if defined(CONFIG_OABI_COMPAT)
	/*If the swi argument is zero, this is an EABI call and we do nothing.
     * If this is an old ABI call, get the syscall number into scno and
     * get the old ABI syscall table address.
     */
	 bics r10, r10, #0xff000000
	 eorns scno, r10, #__NR_OABI_SYSCALL_BASE
	//从r10 获取中断号，r10存储的本来是swi指令的完整指令码，可以从上面代码看到
	ldrne    tbl, =sys_oabi_call_table
#elif !define(CONFIG_AEABI)
	bic scno, scno, #0xff000000 @mask off SWI op-code
	eor scno, scno, #__NR_SYSCALL_BASE @check OS number
#endif
	ldr r10, [tsk, #TI_FLAGS] @check for syscall tracing
	stmdb sp!, {r4, r5} @push fifth and sixth args
	@将r4 r5入栈，因为系统调用最多有6个参数
	@但APCS规定r0-r3 4个参数可以用寄存器传递
	@r4 r5必须用栈来传递

#ifdef CONFIG_SECCOMP
	tst r10, #_TIF_SECCOMP
	beq 1f
	mov r0, scno
	bl __secure_computing
	add r0, sp, #S_R0 + S_OFF @pointer to regs
	ldmia r0, {r0-r3} @have to reload r0-r3
1:
#endif

	tst r10, #_TIF_SYSCALL_WORK @are we tracing syscalls
	bne __sys_trace //只考虑无tracing这里不跳转
	cmp scno, #NR_syscalls //check upper syscall limit NR_syscalls(这个是系统调用个数)
	adr lr, BSYM(ret_fast_syscall)// return address,将返回的地址预填到lr寄存器
	ldrcc pc, [tbl, scno, lsl #2] //call sys_* routine
	//如果scno 小于NR_syscalls则执行该指令，将PC移动到tbl中的对应syscall位置(tbl+scno*4)
	add r1, sp, #S_OFF //后面都是scno 过大的出错处理
2:  mov why, #0   @no longer a real syscall
	cmp scno, #(__ARM_NR_BASE - __NR_SYSCALL_BASE)
	eor r0, scno, #__NR_SYSCALL_BASE @put OS number back
	bcs arm_syscall
	b sys_ni_syscall @not private func
ENDPROC(vector_swi)
```

上面注释中已经详细分析了代码，这里做一个总结，进入vector_swi后的执行流程：

* (1)在栈上备份中断前的寄存器。
* (2)获取`syscall number(r7)`。
* (3)进入具体syscall函数的准备工作`r8-->tbl,r9-->tsk,lr-->新的返回地址（ret_fast_syscall) r0-r5 6个C函数的参数放在对应位置
* (4)通过tbl记录的table入口和r7记录的偏移量，进入相应的系统调用。
* (5)返回到ret_fast_syscall,具体来说原理都一样，不再分析了

**calls.S** 在entry-common.S中的两次包含及其具体意义

文件：

```
linux-2.6.30.4/arch/arm/kernel/entry-common.S 程序软中断的入口
linux-2.6.30.4/arch/arm/kernel/calls.S 存储了所有sys_func函数的入口地址。
```

```
/* 0 */		CALL(sys_restart_syscall)
		CALL(sys_exit)
		CALL(sys_fork)
		CALL(sys_read)
		CALL(sys_write)
/* 5 */		CALL(sys_open)
		CALL(sys_close)
		CALL(sys_ni_syscall)		/* was sys_waitpid */
		CALL(sys_creat)
		CALL(sys_link)
......
/* 380 */    CALL(sys_sched_setattr)
               CALL(sys_sched_getattr)
#ifndef syscalls_counted
.equ syscalls_padding, ((NR_syscalls + 3) & ~3) - NR_syscalls
#define syscalls_counted
#endif
.rept syscalls_padding
        CALL(sys_ni_syscall)
.endr
```

在entry-common.S中包含了上次calls.S，这里简单分析前两次：

* 1.NR_syscalls记录CALL的个数：

```asm
.equ NR_syscalls,0 @NR_syscalls = 0

@NR_syscalls = NR_syscalls  + 1
#define CALL(x) .equ NR_syscalls,NR_syscalls+1
//将calls.S的内容包进来,上面对CALL(x)进行了定义所以相当于执行了多次NR_syscalls++
#include "calls.S"
#undef CALL
#define CALL(x) .long x
```

在calls.S的结尾对齐处理：由于是第一次包含,故syscalls_counted没有定义, 
`.equ syscalls_padding, ((NR_syscalls + 3) & ~3) - NR_syscalls`

为了保证NR_syscalls是4的整数倍，上面的语句相当于：`syscalls_padding = ((NR_syscalls + 3) & ~3) - NR_syscalls;`

```asm
CALL(sys_preadv)
CALL(sys_pwritev)
//注意:在calls.S的结尾的对齐处理:
#ifndef syscalls_counted
.equ syscalls_padding, ((NR_syscalls + 3) & ~3) - NR_syscalls
#define syscalls_counted
#endif
.rept syscalls_padding
CALL(sys_ni_syscall)
.endr
```

`.equ NR_syscalls,0`中涉及到汇编指令`.equ`:

```
.equ/.set: 赋值语句, 格式如下:
.equ(.set) 变量名,表达式  
如 .equ abc 3 @让abc=3
```

汇编指令`.rept`的用法:

```
.rept:重复定义伪操作, 格式如下: 
.rept 重复次数 
数据定义 
.endr @结束重复定义 

例如: 
.rept 3 
.byte 0x23 
.endr
```

2.`syscall()`函数实现：

```asm
.type sys_call_table ,#object
ENTRY(sys_call_table)
#include "calls.S"
#undef ABI
#undef OBSOLETE

@r0 = syscall number
@r8 = syscall table
syscall:
	bic scno, r0, #__NR_OABI_SYSCALL_BASE
	cmp scno, #__NR_syscall - __NR_SYSCALL_BASE//比较scno 是否是syscall()的系统调用号
   cmpne    scno, #NR_syscalls    // check range; cmp命令与lo条件保证 系统调用号不会超出范围
stmloia    sp, {r5, r6}        // shuffle args 因为int syscall(int number, ...);作为最特殊的一个syscall支持可变的7个参数r0-r6
movlo    r0, r1                //很容易理解因为syscall()需要支持最多6个参数的syscall,并且还要在r0参数中包含应该调用的syscall number
movlo    r1, r2                //所以，r1-r6中存储了规定的六个参数，在进如用户所真正期望的系统调用之前，需要重新按照ATPCS规定，排列相应的参数列表
movlo    r2, r3                //也就造成了r5 r6 入栈，r1-r4顺次前移。
movlo    r3, r4
ldrlo    pc, [tbl, scno, lsl #2]
b    sys_ni_syscall
ENDPROC(sys_syscall);
```
第二次包含是建立在第一次包含的基础上，第一次包含的结果是：

* `#undef CALL`
* `#define CALL(x) .long x`
* NR_syscalls是系统调用的个数，并且进行了4的倍数对齐
* syscalls_padding的值保证了CALL(x)的个数可以4倍对齐，即`.long x`的个数是4的倍数。目的是在下面的sys_call函数中的`ldrlo pc, [tbl, scno, lsl #2]`

即将“系统调用表基地址tbl+系统调用号scno*4”地址单元（上面的某个.long x）的数据（也就是某个系统调用处理函数的入口地址）放到PC寄存器中（因为有了对齐，所以不会产生访问越界的情况，又因为

```asm
bic    scno, r0, #__NR_OABI_SYSCALL_BASE
@这一位清除指令 清除了SYSCALL_BASE的高位，保证存储在scno中的系统调用号是一个基于__NR_OABI_SYSCALL_BASE的偏移量，只表示是第几个系统调用
注意scno中只是第几个的意思，不是系统调用的实际地址或者相对地址，这一点后文会再次利用到）
````

* ` cmp scno, #__NR_syscall - __NR_SYSCALL_BASE `
	* CMP指令实际是影响CPSR的减法指令。
	* 这条指令有两种情况:
		* 1，传递下来的scno是`syscall()`的系统调用号，那么 CPSR的 C位为1，Z位为1  这样带ne与lo的指令都不执行函数结束
		* 2 传递下来的scno是其他系统调用的系统调用号，那么 CPSR的 C位为1，Z位为0 进入下一句`cmpne scno, #NR_syscalls`

* `cmpne scno, #NR_syscalls    @ check range`
	* 这一句在已知scno为一个系统调用的相对偏移量的基础上 `scno < #NR_syscalls`。
	* 这一相减指令的结果一定是`< 0` 则，CPSR的C=0 Z=0，下面的lo指令全部生效

以上两条语句保证了系统调用号scno的大小不会超出范围。超出范围则跳过所有指令直接执行`b sys_ni_syscall`这一句无条件命令 然后退出。

可以看到，在calls.S结尾的系统调用：`sys_ni_syscall`。它除了返回-ENOSYS之外啥也没干：

```
/*
* Non-implemented system calls get redirected here.
 */
asmlinkage long sys_ni_syscall(void)
{
	return -ENOSYS;
}
```

```asm
stmloia    sp, {r5, r6}        @ shuffle args
movlo    r0, r1
movlo    r1, r2
movlo    r2, r3
movlo    r3, r4
ldrlo    pc, [tbl, scno, lsl #2]
```
这几句指令是调整系统调用参数的位置，方便下一步调用。因为系统调用最多6个参数，后文会提到原因。

我们分析一下这段语句运行前的寄存器内容 r0(系统调用号) r1（参数1） r2（参数2） r3（参数3） r4（参数4） r5（参数5） r6（参数6）

（其实从代码可以看出`syscall(int number,...)` 是linux下唯一一个有7个参数的函数，`r0`是实际调用的其他系统调用号，`r1-r6`是其他系统调用的6个参数）(对于一般的系统调用如果是直接调用参数存储在r0-r5共6个)

因为在C和ARM汇编程序间相互调用须遵守ATPCS规则。函数的前四个参数用`r0-r3`传递，之后的参数5 6 用栈传递，所以前5句指令的作用正如注释所说，是调整参数存储位置方便C函数调用。

```
ldrlo pc, [tbl, scno, lsl #2]  @跳转到真正的系统调用的C或者汇编函数入口
```

* 该指令将scno逻辑左移2位加到tbl上，把结果位置处所存储的值赋值给PC。`PC = *（tbl+（scno<<2））`。
* tbl是系统调用table的入口地址，可以把tbl看作char数组头。`char tbl[] = {.......};`
* 因为scno只是第几个系统调用的意思，不是系统调用的实际地址或者相对地址。
* 而实际每一个系统调用的地址（也就是形象理解的函数名），是`.long x 32`位，需要四个字节存储。
* scno逻辑左移2位，意思为在tbl上偏移`scno<<2`个字节的位置上，存储着所需的系统调用函数的入口地址。`tbl[scno<<2]` 存储着所需的系统调用函数的入口地址。

## 1.3 syscall调用过程中各个寄存器都干了什么

> note：这里就解释清楚了另一个小问题，为什么kernel将r7定义为syscall number的存储寄存器？为什么系统调用最多只有6个参数？为什么syscall()函数可以有7个参数？

当然syscall：是汇编中的lable 定义上来看不具备函数的完整特点，不过我们暂且把它说成是函数也没有问题。

svc模式共有`r0-r16` 17个寄存器，其中arm架构制定有特殊作用的`r16-->cpsr r15-->pc r14-->lr` 

ATPCS(ARM-THUMB procedure call standard)规定使用：`r13-->sp r12-->ip r11-->fp r10-->SL`

这些在编译标准C代码的时候，显然是编译器会不通知你直接使用的，我们当然不能用他们固定的存储数据。所以留给我们可以自由使用的寄存器就是r0-r9，我们再来看kernel在系统调用阶段是如何使用这些寄存器的吧

* `r9-->tsk`:(存栈底指针，这是进程栈的起始位置，记录了进程的所有信息，非常重要，很多地方会用到)
* `r8-->tbl`:(存储系统调用table入口地址)
* `r7-->scno`:(系统调用号)
* `r0-r5`:syscall的六个参数）

r6-->在syscall：函数实现的注释中我们已经提到了，是存储`syscall()`这个系统调用的第七个参数，因为`syscall()`函数需要能够正确调用所有的系统调用，所以他必须比其他系统调用多用一个寄存器。

### syscall 号定义位置
所有系统调用的编号定义在`arch/arm/include/asm/unistd.h`中：

```c
#define __NR_OABI_SYSCALL_BASE     0x900000
 
#if defined(__thumb__) || defined(__ARM_EABI__)
#define __NR_SYSCALL_BASE     0
#else
#define __NR_SYSCALL_BASE     __NR_OABI_SYSCALL_BASE
#endif
 
/*
* This file contains the system call numbers.
*/
 
#define __NR_restart_syscall          (__NR_SYSCALL_BASE+  0)
#define __NR_exit               (__NR_SYSCALL_BASE+  1)
#define __NR_fork               (__NR_SYSCALL_BASE+  2)
#define __NR_read               (__NR_SYSCALL_BASE+  3)
#define __NR_write               (__NR_SYSCALL_BASE+  4)
#define __NR_open               (__NR_SYSCALL_BASE+  5)
.................
#define __NR_pipe2               (__NR_SYSCALL_BASE+359)
#define __NR_inotify_init1          (__NR_SYSCALL_BASE+360)
#define __NR_preadv               (__NR_SYSCALL_BASE+361)
#define __NR_pwritev               (__NR_SYSCALL_BASE+362)
#define __NR_rt_tgsigqueueinfo          (__NR_SYSCALL_BASE+363)
#define __NR_perf_event_open          (__NR_SYSCALL_BASE+364)
#define __NR_recvmmsg               (__NR_SYSCALL_BASE+365)
```

### syscall的定义宏
在`include/linux/syscalls.h`中

```c
#define SYSCALL_DEFINE0(name)	   asmlinkage long sys_##name(void)
#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEx(1, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEx(2, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEx(3, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEx(4, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEx(5, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEx(6, _##name, __VA_ARGS__)
 
#define SYSCALL_DEFINEx(x, sname, ...)                \
    __SYSCALL_DEFINEx(x, sname, __VA_ARGS__)
 
#define __SYSCALL_DEFINEx(x, name, ...)                    \
    asmlinkage long sys##name(__SC_DECL##x(__VA_ARGS__))
```
在无strace的模式下实现很简单，只是将每个函数定义前面加上`sys_`前缀。

```
SYSCALL_DEFINE0(fork) //asmlinkage long sys_fork(void)
{
    struct pt_regs *regs = task_pt_regs(current);
    return do_fork(SIGCHLD, regs->gprs[15], regs, 0, NULL, NULL);
}
 
SYSCALL_DEFINE2(dup2, unsigned int, oldfd, unsigned int, newfd) // asmlinkage long sys_dup2(unsigned int old fd,unsigned int newfd)
{
    ...
}
```