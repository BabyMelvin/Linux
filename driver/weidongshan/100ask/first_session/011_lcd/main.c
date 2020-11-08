
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
char g_Char3 = 'a';
const char g_Char2 = 'B';
int g_A = 0;
int g_B;

int main(void)
{
	led_init();
	//interrupt_init();  /* ��ʼ���жϿ����� */
	key_eint_init();   /* ��ʼ������, ��Ϊ�ж�Դ */
	//timer_init();
	
	puts("\n\rg_A = ");
	printHex(g_A);
	puts("\n\r");

	nor_flash_test();
	
	return 0;
}



