.text
.global _start
_start:
    b reset              @ vector 0: reset

    @ 执行到这里之前
    @ 1. 1r_und 保存有被中断模式中的下一条即将被执行的指令地址
    @ 2. SPSR_und 保存素有被中断模式的CPSR
    @ 3. CPSR中的M4-M0 被设置为11011,进入到und 模式
    @ 4. 跳转到0x4的地方执行程序 
    ldr pc, und_addr     @ vector 4: und

    @ 执行到这里之前
    @ 1. 1r_svc 保存有被中断模式中的下一条即将被执行的指令地址
    @ 2. SPSR_svc 保存素有被中断模式的CPSR
    @ 3. CPSR中的M4-M0 被设置为11011,进入到svc模式
    @ 4. 跳转到0x8的地方执行程序 
    ldr pc, swi_addr     @ vector 8: swi
und_addr:
    .word do_und

swi_addr:
    .word do_swi

do_und:
    @ sp_und 未设置，先设置它
    ldr sp, =0x34000000

    @ 在und异常处理函数中可能会修改r0-r12，所以先保存
    @ lr是异常处理后的返回地址,也要保存
    stmdb sp!, {r0-r12, lr}
    
    @ 保存现场，处理und异常
    mrs r0, cpsr
    ldr r1, =und_string
    bl printException

    @ 恢复现场(^会把spsr的值恢复到cpsr里)
    ldmia sp!, {r0-r12, pc}^

und_string:
    .string "undefined instruction exception!"

.align 4

do_swi:
    @ sp_svc未设置，先设置它
    ldr sp, =0x33e00000

    @ 在swi异常处理函数中可能会修改r0-r12，所以先保存
    @ lr是异常处理后的返回地址,也要保存
    stmdb sp!, {r0-r12, lr}
    
    mov r4, lr
    
    @ 保存现场，处理swi异常
    mrs r0, cpsr
    ldr r1, =swi_string
    bl printException

    sub r0, r4, #4
    bl printSWIVal
    @ 恢复现场(^会把spsr的值恢复到cpsr里)
    ldmia sp!, {r0-r12, pc}^

swi_string:
    .string "swi exception"

.align 4
reset:
    /* 关闭看门狗 */
    ldr r0, =0x53000000
    ldr r1, =0
    str r1, [r0]

    @ 设置MPLL，FLCK：HCLK：PCLK = 400M：100M:50M
    @ LOCKTIME(0x4C000000) = 0xFFFFFFFF
    ldr r0, =0x4C000000
    ldr r1, =0xFFFFFFFF
    str r1, [r0]

    @ CLKDIVN(0x4C000014) = 0x5,tFLCK:tHCLK:tPCLK=1:4:8
    ldr r0, =0x4C000014
    ldr r1, =0x5
    str r1, [r0]

    @ 设置CPU工作于异步模式
    mrc p15,0,r0,c1,c0,0
    orr r0,r0,#0xc0000000  @ R1_nF:OR:R1_iA
    mcr p15,0,r0,c1,c0,0

    /* 设置MPLLCON(0x4C000004) = (92<<12)|(1<<4)|(1<<0)
     *   m = MDIV+8 = 92+8=100
     *   p = PDIV+2 = 1+2 = 3
     *   s = SDIV = 1
     *   FCLK = 2*m*Fin/(p*2^s) = 2*100*12/(3*2^1)=400M
     **/
    ldr r0, =0x4C000004
    ldr r1, =(92<<12)|(1<<4)|(1<<0)
    str r1, [r0]

    @ 一旦设置PLL, 就会锁定lock time直到PLL输出稳定
    @ 然后CPU工作于新的频率FCLK
#ifdef LED_PARAMS
    mov r0, #4
    bl led_on
    
    ldr r0, =100000
    bl delay
    
    mov r0, #5
    bl led_on
#endif

    @ 设置内存 sp栈
    /** 分辨nor/nand启动
    写0到0地址，再读出
        得到0，表示地址上内容被修改，对应ram，就是nand启动
        否则就是nor启动
    **/

    mov r1, #0
    ldr r0, [r1] @读出原来的值备份
    str r1, [r1]
    
    ldr r2, [r1] @r2=[0]
    cmp r1, r2   @如果相等nand启动
    ldr sp, =0x40000000 + 4096 @假设nor启动
    
    moveq sp, #4096 @nand启动
    streq r0, [r1]  @恢复原来的值

    bl sdram_init
    // bl sdram_init2    @ 用到有初始值的数组，不是位置无关码

    @ 重定位data段 
    /*mov r1, #0x800
    ldr r0, [r1]
    mov r1, #0x30000000
    str r0, [r1]*/

    @ 重定位整个程序 text, rodata,data
    bl copy2sdram           @ 参数src, dest, len

    @ 清除BSS段
    bl clean_bss

    ldr pc, =sdram
sdram:
    bl uart0_init

    bl print1
    @ 故意加入一条未定义指令
und_code:
    .word 0xdeadc0de    @ 未定义指令
    bl print2


    swi 0x123 @ 执行此命令，触发SWI异常，进入0x8执行
    @ 使用BL命令相对跳转，程序仍然在NOR/SRAM执行
    //bl main

    @ 绝对跳转，跳转到SDRAM
    ldr pc, =main

halt:
    b halt
