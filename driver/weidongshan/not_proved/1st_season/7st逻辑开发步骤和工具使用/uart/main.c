#include "serial.h"

int main(void)
{
    unsigned char c;
    uart0_init(); // 波特率115200, 8IN1(8个数据位， 无校验码， 1个停止位)

    while(1) {
        // 从串口接收数据后，判断其是否数字或字母，若是则加1后输出
        c = getc();
        if (is_digit(c) || is_letter(c))
            putc(c+1);
    }

    return 0;
}
