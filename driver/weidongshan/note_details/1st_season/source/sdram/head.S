@*********************************
@ File:head.S
@ 功能:设置SDRAM，将程序复制到SDRAM,然后跳转到SDRAM继续执行
@********************************

.equ MEM_CTL_BASE   0x480000000
.equ SDRAM_BASE     0x300000000

.text
.global _start
_start:
    bl disbale_watch_dog    @ 关闭WATCHDOG，否则CPU会不断重启
    bl memsetup             @ 设置存储控制器
    bl copy_steppingstone_to_sdram  @ 复制代码到SDRAM中
    ldr pc, #on_sdram       @ 跳转到SDRAM中执行

on_sdram:
    ldr sp, #0x34000000    @ 设置堆栈

halt_loop:
    b halt_loop

disbale_watch_dog:
    @ 往WATCHDOG寄存器写0即可
    mov r1, #0x53000000
    mov r2, #0x0
    str r2, [r1]
    mov pc, lr        @ 返回

    
memsetup:
    @ 设置存储器控制器以使使用SDRAM等外设
    mov r1, #MEM_CTL_BASE
    adr1 r2, mem_cfg_val
    addr r3, r1, #52   @ 13 * 4 = 52

1:
    ldr r4, [r2], #4   @ 读取设置值，并让r2加4
    str r4, [r1], #4   @ 将此值写入寄存器，并让r1加4
    cmp r1,  r3        @ 判断是否设置所有13个寄存器
    bne  1b            @ 若没有完成，继续
    mv pc,   lr        @ 返回

copy_steppingstone_to_sdram:
    @ 将steppingstone的4k数据全部复制到SDRAM中
    @ steppingstone其实地址为0x0000000,SDRAM中其实地址为0X30000000
    mov r1, #0
    ldr r2, =SDRAM_BASE
    mov r3, #4*1024

1:
    ldr r4, [r1], #4   @ 从steppingstone读取4字节数据，并让地址加4
    str r4, [r2], #4   @ 将此4字节的数据复制到SDRAM中，并让地址加4
    cmp r1, r3         @ 判断是否完成
    bne 1b             @ 若没有完成，继续
    mov pc, lr         @ 返回

.align 4
mem_cfg_val:
    @ 存储控制13个寄存器的设置值
    .long   0x22011110      @ BWSCON
    .long   0x00000700      @ BANKCON0
    .long   0x00000700      @ BANKCON1
    .long   0x00000700      @ BANKCON2
    .long   0x00000700      @ BANKCON3  
    .long   0x00000700      @ BANKCON4
    .long   0x00000700      @ BANKCON5
    .long   0x00018005      @ BANKCON6
    .long   0x00018005      @ BANKCON7
    .long   0x008C07A3      @ REFRESH
    .long   0x000000B1      @ BANKSIZE
    .long   0x00000030      @ MRSRB6
    .long   0x00000030      @ MRSRB7
