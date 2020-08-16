// 电亮LED1： GPF4

.text
.global _start

_start:
    /* 配置GPF4为输出引脚
     * 吧0X100写到地址0x5600050
     */
    ldr r1, =0x56000050
    ldr r0, =0x100 /*mov r0, #0x100*/
    str r0, [r1]

    /*
        设置GPF4输出为高电平
        把0写到地址0x56000054
    */ 
    ldr r1, =0x56000054
    ldr r0, =0
    str r0, [r1]

/*死循环，避免跑到后面未知程序*/
halt:
    b halt
