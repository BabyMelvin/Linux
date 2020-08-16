@ 循环点亮LED 4 5 6
.text
.global _start

_start:

    /* 关闭看门狗 */
    ldr r0, =0x53000000
    ldr r1, =0
    str r1, [r0]

#ifdef LED_PARAMS
    mov r0, #4
    bl led_on
    
    ldr r0, =100000
    bl delay
    
    mov r0, #5
    bl led_on
#endif
    // 设置内存 sp栈
    /*
    * 分辨nor/nand启动
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

    bl main

halt:
    b halt
