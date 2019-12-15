#include <stdio.h>

int main()
{
    SPI_init();
    OLED_init();
    OLED_print(0,0,"www.100ask.net,100ask.taobao.com");
    return 0;
}
