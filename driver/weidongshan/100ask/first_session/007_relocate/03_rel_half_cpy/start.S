@ 循环点亮LED 4 5 6
.text
.global _start

_start:

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

    @ 重定位data段 
    /*mov r1, #0x800
    ldr r0, [r1]
    mov r1, #0x30000000
    str r0, [r1]*/

    @ 重定位data段
    ldr r1, =data_load_addr @ data段在bin文件中的地址(加载地址)
    ldr r2, =data_start     @ data段在重定位地址在运行地址)
    ldr r3, =data_end       @ data段结束地址

cpy:
    ldrb r4, [r1]
    strb r4, [r2]
    add  r1, r1, #1
    add  r2, r2, #1
    cmp  r2, r3
    bne cpy

    @ 清除BSS段
    ldr r1, =bss_start
    ldr r2, =bss_end
    mov r3, #0
clean:
    str r3, [r1]
    add r1, r1, #4
    cmp r1, r2
    ble clean

    bl main

halt:
    b halt
