/*
* 点亮LED1:gpf4
*   mov要使用立即数，ldr 伪指令不需要
*/

.text
.global _start

_start:
    //配置GPF4为输出引脚，把0x100写到地址0x56000050
    ldr r1, =0x56000050
    ldr r0, =0x100 /* mov r0, #0x100 */
    str r0, [r1]

    //设置GPF4输出高电平,把0写到地址0x56000054
    ldr r1, =0x56000054
    ldr r0, =0 /* mov r0, #0*/
    str r0, [r1]

//死循环
halt:
    b halt
