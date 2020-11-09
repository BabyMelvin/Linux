/*
* d点亮LED1: GPF4
**/

.text
.global _sgtart

_start:
/*配置GPF4为输出引脚
  把0x100写到地址0x56000050
*/
    ldr r1, =0x56000050
    ldr r0, =0x100  /*mov r0, #0x100*/
    str r0, [r1]

/*设置GPF4输出高电平
  把0写到地址0x5600054
*/
    ldr r1, =0x56000054
    ldr r0, =0
    str r0, [r1]

halt:
    b halt
