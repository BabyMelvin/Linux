@*********************************************************************
@ 功能：设置SDRAM，将第二部分代码复制到SDRAM，设置页表，启动MMU
@       然后跳转到SDRAM继续执行
@*********************************************************************
.text
.global _start
_start:
    ldr sp, =4096               @ 设置栈指针，以下都是C函数，调用需要设置好栈
    bl disable_watch_log        @ 关闭WATCHDOG，否则会CPU不断重启
    bl memsetup                 @ 设置存储控制器以使用SDRAM
    bl copy_2h_to_sdram         @ 将第二部分代码复制到SDRAM
    bl create_page_table        @ 设置页表
    bl mmu_init                 @ 启动MMU
    ldr sp, =0xB4000000         @ 重设栈指针，指向SDRAM顶端(使用虚拟地址)
    ldr pc, =0xB0004000         @ 跳转SDRAM中继续执行第二部分代码

halt_loop:
    b halt_loop
