# 一、前言

kernel的整个启动过程涉及的内容很多，不可能每一个细节都描述清楚，因此我打算针对部分和ARM64相关的启动步骤进行学习、整理，并方便后续查阅。本文实际上描述在系统启动最开始的时候，bootloader和kernel的交互以及kernel如何保存bootloader传递的参数并进行校验，此外，还有一些最基础的硬件初始化的内容。

本文中的source来自4.1.10内核，这是一个long term的版本，后续一段时间的文章都会基于这个long term版本进行。

# 二、进入kernel之前

系统启动过程中，linux kernel不是一个人在战斗，在kernel之前bootloader会执行若干的动作，然后把控制权转移给linux kernel。需要特别说明的是：这里bootloader是一个宽泛的概念，其实就是为kernel准备好执行环境的那些软件，可能是传统意义的bootloader（例如Uboot），也可能是Hypervisor或者是secure monitor。具体bootloader需要执行的动作包括：

1、初始化系统中的RAM并将RAM的信息告知kernel

2、准备好device tree blob的信息并将dtb的首地址告知kernel

3、解压内核（可选）

4、将控制权转交给内核。当然，bootloader和kernel的交互的时候需求如下：

```
MMU = off, D-cache = off, I-cache = on or off
   x0 = physical address to the FDT blob
```

这里需要对data cache和instruction cache多说几句。我们知道，具体实现中的ARMv8处理器的cache是形成若干个level，一般而言，可能L1是分成了data cache和instruction cache，而其他level的cache都是unified cache。上面定义的D-cache off并不是说仅仅disable L1的data cache，实际上是disable了各个level的data cache和unified cache。同理，对于instruction cache亦然。

此外，在`on/off`控制上，MMU和data cache是有一定关联的。在ARM64中，SCTLR, System Control Register用来控制MMU icache和dcache，虽然这几个控制bit是分开的，但是并不意味着MMU、data cache、instruction cache的on/off控制是彼此独立的。一般而言，这里MMU和data cache是绑定的，即如果MMU 是off的，那么data cache也必须要off。因为如果打开data cache，那么要设定memory type、sharebility attribute、cachebility attribute等，而这些信息是保存在页表（Translation table）的描述符中，因此，如果不打开MMU，如果没有页表翻译过程，那么根本不知道怎么来应用data cache。当然，是不是说HW根本不允许这样设定呢？也不是了，在MMU OFF而data cache是ON的时候，这时候，所有的memory type和attribute是固定的，即memory type都是normal Non-shareable的，对于inner cache和outer cache，其策略都是Write-Back，Read-Write Allocate的。

更详细的ARM64 boot protocol请参考Documentation/arm64/booting.txt文档。

# 三、参数的保存和校验


最开始的ARM64启动代码位于arch/arm64/kernel/head.S文件中，代码如下：


```
ENTRY(stext)
    bl    preserve_boot_args
    bl    el2_setup            // Drop to EL1, w20=cpu_boot_mode
    adrp    x24, __PHYS_OFFSET
    bl    set_cpu_boot_mode_flag

    bl    __vet_fdt 
    ……
ENDPROC(stext)
```

## 3.1 preserve_boot_args

```
preserve_boot_args:
    mov    x21, x0－－－－－－将dtb的地址暂存在x21寄存器中，释放出x0以便后续做临时变量使用

    adr_l    x0, boot_args－－－x0保存了boot_args变量的地址
    stp    x21, x1, [x0]－－－－保存x0和x1的值到boot_args[0]和boot_args[1]
    stp    x2, x3, [x0, #16] －－－保存x2和x3的值到boot_args[2]和boot_args[3]

    dmb    sy－－－－－－－－－full system data memory barrier

    add    x1, x0, #0x20－－－－x0和x1是传递给__inval_cache_range的参数
    b    __inval_cache_range
ENDPROC(preserve_boot_args)
```

由于MMU = off, D-cache = off，因此写入boot_args变量的操作都是略过data cache的，直接写入了RAM中（前面说过了，这里的D-cache并不是特指L1的data cache，而是各个level的data cache和unified cache），为了安全起见（也许bootloader中打开了D-cache并操作了boot_args这段memory，从而在各个level的data cache和unified cache有了一些旧的，没有意义的数据），需要将boot_args变量对应的cache line进行清除并设置无效。在调用__inval_cache_range之前，x0是boot_args这段memory的首地址，x1是末尾的地址（boot_args变量长度是`4x8byte＝32`byte，也就是0x20了）。

为何要保存`x0～x3`这四个寄存器呢？因为ARM64 boot protocol对启动时候的`x0～x3`这四个寄存器有严格的限制：x0是dtb的物理地址，`x1～x3`必须是0（非零值是保留将来使用）。在后续setup_arch函数执行的时候会访问boot_args并进行校验。

对于invalidate cache的操作而言，我们可以追问几个问题：如果boot_args所在区域的首地址和尾部地址没有对齐到cache line怎么办？具体invalidate cache需要操作到那些level的的cache？这些问题可以通过阅读`__inval_cache_range`的代码获得答案，这里就不描述了。

还有一个小细节是如何访问boot_args这个符号的，这个符号是一个虚拟地址，但是，现在没有建立好页表，也没有打开MMU，如何访问它呢？这是通过adr_l这个宏来完成的。这个宏实际上是通过adrp这个汇编指令完成，通过该指令可以将符号地址变成运行时地址（通过PC relative offset形式），因此，当运行的MMU OFF mode下，通过adrp指令可以获取符号的物理地址。不过adrp是page对齐的（adrp中的p就是page的意思），boot_args这个符号当然不会是page size对齐的，因此不能直接使用adrp，而是使用adr_l这个宏进行处理，如果读者有兴趣可以自己看source code。

最后，我们来解释一下`dmb sy`这一条指令。在ARM ARM文档中，有关于数据访问指令和 data cache指令之间操作顺序的约定，原文如下：

```
All data cache instructions, other than DC ZVA, that specify an address can execute in any order relative to loads or stores that access any address with the Device memory attribute,or with Normal memory with Inner Non-cacheable attribute unless a DMB or DSB is executed between the instructions.
```
因此，在Non-cacheable的情况下，必须要使用DMB来保证`stp`指令在`dc ivac`指令之前执行完成。

## 3.2 el2_setup

程序执行至此，CPU处于哪一个exception level呢？根据ARM64 boot protocol，CPU要么处于EL2（推荐）或者non-secure EL1。如果在EL1，情形有些类似过去arm处理器的感觉，处于EL2稍微复杂一些，需要对virtualisation extensions进行基本的设定，然后将cpu退回到EL1。代码太长了，我们分成两段来阅读，第一段如下：

```
ENTRY(el2_setup)
    mrs    x0, CurrentEL－－－－－－－－－－－－－－－－－－－－－－－－（1）
    cmp    x0, #CurrentEL_EL2－－－－－－判断是否处于EL2
    b.ne    1f－－－－－－－－－－－－－－不是的话，跳到1f
    mrs    x0, sctlr_el2－－－－－－－－－－－－－－－－－－－－－－－－－（2）
CPU_BE(    orr    x0, x0, #(1 << 25)    )    // Set the EE bit for EL2
CPU_LE(    bic    x0, x0, #(1 << 25)    )    // Clear the EE bit for EL2
    msr    sctlr_el2, x0－－－－写回sctlr_el2寄存器
    b    2f
1:    mrs    x0, sctlr_el1－－－－－－－－－－－－－－－－－－－－－－－－－（3）
CPU_BE(    orr    x0, x0, #(3 << 24)    )    // Set the EE and E0E bits for EL1
CPU_LE(    bic    x0, x0, #(3 << 24)    )    // Clear the EE and E0E bits for EL1
    msr    sctlr_el1, x0
    mov    w20, #BOOT_CPU_MODE_EL1－－－－w20寄存器保存了cpu启动时候的Eexception level
    isb－－－－－－－－－instruction memory barrier
    ret

2:    mov    x0, #(1 << 31) －－－－－－－－－－－－－－－－－－－－－－－－（4）
    msr    hcr_el2, x0

    mrs    x0, cnthctl_el2 －－－－－－－－－－－－－－－－－－－－－－－－－（5）
    orr    x0, x0, #3                 // Enable EL1 physical timers
    msr    cnthctl_el2, x0
    msr    cntvoff_el2, xzr        // Clear virtual offset

    mrs    x0, id_aa64pfr0_el1 －－－－－－－－－－－－－－－－－－－－－－－（6）
    ubfx    x0, x0, #24, #4 －－－－取出24 bit开始的4个bit的值并将该值赋给x0
    cmp    x0, #1
    b.ne    3f －－－－－不支持system register接口

    mrs_s    x0, ICC_SRE_EL2
    orr    x0, x0, #ICC_SRE_EL2_SRE    // Set ICC_SRE_EL2.SRE==1
    orr    x0, x0, #ICC_SRE_EL2_ENABLE    // Set ICC_SRE_EL2.Enable==1
    msr_s    ICC_SRE_EL2, x0
    isb                    // Make sure SRE is now set
    msr_s    ICH_HCR_EL2, xzr        // Reset ICC_HCR_EL2 to defaults

3:               ……
```

* （1）当前的exception level保存在PSTATE中，程序可以通过MRS或者MSR来访问PSTATE，当然需要传递一个Special-purpose register做为参数，CurrentEL就是获取`PSTATE中current exception level`域的特殊寄存器。
* (2) sctlr_el2也是一个可以通过`MRS/MSR`指令访问的寄存器，当CPU处于EL2状态的时候，该寄存器可以控制整个系统的行为。当然，这里仅仅是设定EL2下的数据访问和地址翻译过程中的endianess配置，也就是EE bit[25]。根据配置，CPU_BE和CPU_LE包围的指令只会保留一行。对于little endian而言，实际上就是将sctlr_el2寄存器的EE（bit 25）设定为0。顺便说一下，这个bit不仅仅控制EL2数据访问的endianess以及EL2 stage 1的地址翻译过程中的endianess（当然，EL2只有stage 1），还可以控制EL1和EL0 stage 2地址翻译的过程的endianess（这时候有两个stage的地址翻译过程）。
* (3) 执行到这里说明CPU处于EL1，这种状态下没有权限访问sctlr_el2，只能是访问sctlr_el1。sctlr_el1可以通过EE和E0E来控制EL1和EL0状态下是little endian还是big endian。EE bit控制了EL1下的数据访问以及EL1和EL0 stage 1地址翻译的过程的endianess。E0E bit用来控制EL0状态下的数据访问的endianess。此外，需要注意的是：由于修改了system control register（设定endianess状态），因此需要一个isb来同步（具体包括两部分的内容，一是确认硬件已经执行完毕了isb之前的所有指令，包括修改system control寄存器的那一条指令，另外一点是确保isb之后的指令从新来过，例如取指，校验权限等）。
* （4）执行到这里说明CPU处于EL2，首先设定的是hcr_el2寄存器，Hypervisor Configuration Register。该寄存器的大部分bit 值在reset状态的时候就是0值，只不过bit 31（Register Width Control）是implementation defined，因此这里set 31为1，确保Low level的EL1也是Aarch64的
* （5）这一段代码是对Generic timers进行配置。要想理解这段代码，我们需要简单的了解一些ARMv8上Generic timer的运作逻辑。一个全局范围的system counter、各个PE上自己专属的local timer以及连接这些组件之间的bus或者信息传递机制组成了Generic Timer。对于PE而言，通过寄存器访问，它能看到的是physical counter（实际的system counter计数）、virtual counter（physical counter基础上的offset）、physical timer、virtual timer等。NTHCTL_EL2，Counter-timer Hypervisor Control register，用来控制系统中的physical counter和virutal counter如何产生event stream以及在EL1和EL0状态访问physical counter和timer的硬件行为的。在EL1（EL0）状态的时候访问physical counter和timer有两种配置，一种是允许其访问，另外一种就是trap to EL2。这里的设定是：不陷入EL2（对应的bit设置为1）。更详细的信息可以参考ARMv8 ARM文档。cntvoff_el2是virtual counter offset，所谓virtual counter，其值就是physical counter的值减去一个offset的值（也就是cntvoff_el2的值了），这里把offset值清零，因此virtual counter的计数和physical counter的计数是一样的。
* （6）这一段代码是对GIC V3进行配置。ID_AA64PFR0_EL1，AArch64 Processor Feature Register 0，该寄存器描述了PE实现的feature。GIC bits [27:24]描述了该PE是否实现了system register来访问GIC，如果没有（GIC bits 等于0）那么就略过GIC V3的设定。ICC_SRE_EL2，Interrupt Controller System Register Enable register (EL2)，该寄存器用来（在EL2状态时候）控制如何访问GIC CPU interface模块的，可以通过memory mapped方式，也可以通过system register的方式。将SRE bit设定为1确保通过system register方式进行GIC interface cpu寄存器的访问。将enable bit设定为1确保在EL1状态的时候可以通过ICC_SRE_EL1寄存器对GIC进行配置而不是陷入EL2。


下面我们进入第二段代码：

```
 mrs    x0, midr_el1 －－－－－－－－－－－－－－－－－－－－－－－－－－－－－（1）
    mrs    x1, mpidr_el1
    msr    vpidr_el2, x0
    msr    vmpidr_el2, x1

    mov    x0, #0x0800            // Set/clear RES{1,0} bits －－－－－－－－－－－－－－－（2）
CPU_BE(    movk    x0, #0x33d0, lsl #16    )    // Set EE and E0E on BE systems
CPU_LE(    movk    x0, #0x30d0, lsl #16    )    // Clear EE and E0E on LE systems
    msr    sctlr_el1, x0

    mov    x0, #0x33ff－－－－－－－Disable Coprocessor traps to EL2
    msr    cptr_el2, x0            // Disable copro. traps to EL2

#ifdef CONFIG_COMPAT－－－－－是否支持64 bit kernel上运行32bit 的application
    msr    hstr_el2, xzr            // Disable CP15 traps to EL2
#endif

    mrs    x0, pmcr_el0－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－（3）
    ubfx    x0, x0, #11, #5            // to EL2 and allow access to
    msr    mdcr_el2, x0            // all PMU counters from EL1 
    msr    vttbr_el2, xzr －－－－清除Stage-2 translation table base address register

    adrp    x0, __hyp_stub_vectors
    add    x0, x0, #:lo12:__hyp_stub_vectors
    msr    vbar_el2, x0 －－－－－－－－－－－－－－－设定EL2的异常向量表的基地址

    mov    x0, #(PSR_F_BIT | PSR_I_BIT | PSR_A_BIT | PSR_D_BIT |\
              PSR_MODE_EL1h)
    msr    spsr_el2, x0 －－－－－－－－－－－－－－－－－－－－－－－－－－－－－－（4）
    msr    elr_el2, lr
    mov    w20, #BOOT_CPU_MODE_EL2        // This CPU booted in EL2
    eret－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－（5）
ENDPROC(el2_setup)
```

（1）midr_el1和mpidr_el1都属于标识该PE信息的read only寄存器。MIDR_EL1，Main ID Register主要给出了该PE的architecture信息，Implementer是谁等等信息。MPIDR_EL1，Multiprocessor Affinity Register，该寄存器保存了processor ID。vpidr_el2和vmpidr_el2是上面的两个寄存器是对应的，只不过是for virtual processor的。

（2）这段代码实际上是将0x33d00800（BE）或者0x30d00800（LE）写入sctlr_el1寄存器。BE和LE的设定和上面第一段代码中的描述是类似的，其他bit的设定请参考ARMv8 ARM文档

（3）PMCR_EL0，Performance Monitors Control Register，该寄存器的[15:11]标识了支持的Performance Monitors counter的数目，并将其设定到MDCR_EL2（Monitor Debug Configuration Register (EL2)）中。MDCR_EL2中其他的bit都设定为0，其结果就是允许EL0和EL1进行debug的操作（而不是trap to EL2），允许EL1访问Performance Monitors counter（而不是trap to EL2）。

（4）当系统发生了异常并进入EL2，SPSR_EL2，Saved Program Status Register (EL2)会保存处理器状态，ELR_EL2，Exception Link Register (EL2)会保存返回发生exception的现场的返回地址。这里是设定SPSR_EL2和ELR_EL2的初始值。w20寄存器保存了cpu启动时候的Eexception level ，因此w20被设定为BOOT_CPU_MODE_EL2。

（5）eret指令是用来返回发生exception的现场。实际上，这个指令仅仅是模拟了一次异常返回而已，SPSR_EL2和ELR_EL2都已经设定OK，执行该指令会使得CPU返回EL1状态，并且将SPSR_EL2的值赋给PSTATE，ELR_ELR就是返回地址（实际上也恰好是函数的返回地址）。

完成了el2_setup这个函数分析之后，我们再回头思考这样的问题：为何是el2_setup？为了没有el3_setup？当一个SOC的实现在包括了EL3的支持，那么CPU CORE缺省应该进入EL3状态，为何这里只是判断EL2还是EL1，从而执行不同的流程，如果是EL3状态，代码不就有问题了吗？实际上，即便是由于SOC支持TrustZone而导致cpu core上电后进入EL3，这时候，接管cpu控制的一定不是linux kernel（至少目前来看linux kernel不会做Secure monitor），而是Secure Platform Firmware（也就是传说中的secure monitor），它会进行硬件平台的初始化，loading trusted OS等等，等到完成了secure world的构建之后，把控制权转交给non-secure world，这时候，CPU core多半处于EL2（如果支持虚拟化）或者EL1（不支持虚拟化）。因此，对于linux kernel而言，它感知不到secure world（linux kernel一般也不会做Trusted OS），仅仅是在non-secure world中呼风唤雨，可以是Hypervisor或者rich OS。

## 3.3 set_cpu_boot_mode_flag

在进入这个函数的时候，有一个前提条件：w20寄存器保存了cpu启动时候的Eexception level ，具体代码如下：

```
ENTRY(set_cpu_boot_mode_flag)
    adr_l    x1, __boot_cpu_mode
    cmp    w20, #BOOT_CPU_MODE_EL2
    b.ne    1f
    add    x1, x1, #4
1:    str    w20, [x1]            // This CPU has booted in EL1
    dmb    sy
    dc    ivac, x1            // Invalidate potentially stale cache line
    ret
ENDPROC(set_cpu_boot_mode_flag)
```

由于系统启动之后仍然需要了解cpu启动时候的Eexception level（例如判断是否启用hyp mode），因此，有一个全局变量__boot_cpu_mode用来保存启动时候的CPU mode。代码很简单，大家自行体会就OK了，我这里补充几点描述：

（1）本质上我们希望系统中所有的cpu在初始化的时候处于同样的mode，要么都是EL2，要么都是EL1，有些EL2，有些EL1是不被允许的（也许只有那些精神分裂的bootloader才会这么搞）。

（2）所有的cpu core在启动的时候都处于EL2 mode表示系统支持虚拟化，只有在这种情况下，kvm模块可以顺利启动。

（3）set_cpu_boot_mode_flag和el2_setup这两个函数会在各个cpu上执行。

（4）变量__boot_cpu_mode定义如下：

```
ENTRY(__boot_cpu_mode)
    .long    BOOT_CPU_MODE_EL2－－－－－－－－A
    .long    BOOT_CPU_MODE_EL1－－－－－－－－B
```
如果cpu启动的时候是EL1 mode，会修改变量__boot_cpu_mode A域，将其修改为BOOT_CPU_MODE_EL1。如果cpu启动的时候是EL2 mode，会修改变量`__boot_cpu_mode B`域，将其修改为BOOT_CPU_MODE_EL2。

## 3.4 __vet_fdt

在进入具体函数之前，x21和x24都被设定成了指定的值。x21被设定为fdt在RAM中的物理地址（参考preserve_boot_args函数），x24被设定为__PHYS_OFFSET，定义为：

```
#define __PHYS_OFFSET    (KERNEL_START - TEXT_OFFSET)
#define KERNEL_START    _text
```

KERNEL_START是kernel开始运行的虚拟地址，更确切的说是内核正文段开始的虚拟地址。 在链接脚本文件中（参考arch/arm64/kernel下的vmlinux.lds.S），KERNEL_START被设定为：

```
. = PAGE_OFFSET + TEXT_OFFSET;

.head.text : {
    _text = .;
    HEAD_TEXT
}
```

KERNEL_START是kernel开始运行的虚拟地址，更确切的说是内核正文段开始的虚拟地址。 在链接脚本文件中（参考arch/arm64/kernel下的vmlinux.lds.S），KERNEL_START被设定为：

```
. = PAGE_OFFSET + TEXT_OFFSET;

.head.text : {
    _text = .;
    HEAD_TEXT
}
```
因此，KERNEL_START的值和PAGE_OFFSET以及TEXT_OFFSET这两个offset的设定有关。TEXT_OFFSET标识了内核正文段的offset，其实如果该宏被定义为KERNEL_TEXT_OFFSET会更好理解。我们知道，操作系统运行在内核空间，应用程序运行在用户空间，假设内核空间的首地址是x（一般也是RAM的首地址），那么是否让kernel运行在x地址呢？对于arm，在内核空间的开始有32kB（0x00008000）的空间用于保存内核的页表（也就是进程0的`PGD`）以及bootload和kernel之间参数的传递，对于ARM64，在其Makefile中定义了这个offset是512KB（0x00080000）。

```
ifeq ($(CONFIG_ARM64_RANDOMIZE_TEXT_OFFSET), y)
TEXT_OFFSET := $(shell awk 'BEGIN {srand(); printf "0x%03x000\n", int(512 * rand())}')
else
TEXT_OFFSET := 0x00080000
endif
```

kernel image的开始部分包括了一个ARM64 image header的内容，这个header定义了bootloader如何来copy kernel image。ARM64 image header中有一个域（text_offset）就是告知bootloader，它应该按照多大的偏移来copy kernel image。当然了，也许有些bootloader不鸟这些，对于ARM64平台，反正大家一直都是固定为0x80000，因此，bootloader也没有什么太大的动力来修改支持这个特性。怎么破？虽然目前ARM64的kernel的TEXT_OFFSET就是固定为0x80000，但是也许将来内核会修改这个offset啊。在这种情况下，内核的开发者提供了一个CONFIG_ARM64_RANDOMIZE_TEXT_OFFSET选项，在编译内核的时候可以randomize内核的TEXT_OFFSET值，以此来测试bootloader是否能够正确的copy kernel image到正确的内存偏移位置上去。通过这样一个配置项，可以尽快的暴露问题，确保了整个系统（bootloader ＋ kernel）稳定的运行。

搞定了TEXT_OFFSET，我们再来看看PAGE_OFFSET，在arch/arm64/include/asm/memory.h中，PAGE_OFFSET被定义为：

```
#define VA_BITS            (CONFIG_ARM64_VA_BITS)
#define PAGE_OFFSET        (UL(0xffffffffffffffff) << (VA_BITS - 1))
```
VA_BITS定义了虚拟地址空间的bit数（该值也就是定义了用户态程序或者内核能够访问的虚拟地址空间的size），假设VA_BITS被设定为39个bit，那么PAGE_OFFSET就是0xffffffc0-00000000。`PAGE_OFFSE`T的名字也不好（个人观点，可能有误），OFFSET表明的是一个偏移，内核空间被划分成一个个的page，PAGE_OFFSET看起来应该是定义以page为单位的偏移。但是，以什么为基准的偏移呢？PAGE_OFFSET的名字中没有给出，当然实际上，这个符号是定义以整个address space的起始地址（也就是0）为基准。另外，虽然这个地址是要求page对齐，但是实际上，这个符号仍然定义的是虚拟地址的offset（而不是page的offset）。根据上面的理由，我觉得定义成KERNEL_IMG_OFFSET会更好理解一些。一句话总结：PAGE_OFFSET定义了将kernel image安放在虚拟地址空间的哪个位置上。

OK，经过漫长的说明之后，__PHYS_OFFSET实际上就是kernel image的首地址（并不是__PHYS_OFFSET的位置开始就是真实的kernel image，实际上从`__PHYS_OFFSET`开始，首先是TEXT_OFFSET的保留区域，然后才是真正的kernel image）。实际上，`__PHYS_OFFSET`定义的是一个虚拟地址而不是物理地址，这里的PHYS严重影响了该符号的含义，实际上**adrp**这条指令可以将一个虚拟地址转换成物理地址（在没有打开MMU的时候）。而函数`__vet_fdt`主要是对这个bootloader传递给kernel的fdt参数进行验证，看是否OK，主要验证的内容包括：

（1）是否是8字节对齐的

（2）是否在kernel space的前512M内

```
__vet_fdt:
    tst    x21, #0x7－－－－是否是8字节对齐的
    b.ne    1f
    cmp    x21, x24－－－－－是否在小于kernel space的首地址
    b.lt    1f
    mov    x0, #(1 << 29)
    add    x0, x0, x24
    cmp    x21, x0
    b.ge    1f－－－－－－－是否大于kernel space的首地址＋512M
    ret
1:
    mov    x21, #0－－－－－－－－－－传递的fdt地址有误，清零
    ret
ENDPROC(__vet_fdt)
```

