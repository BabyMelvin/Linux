
#include "s3c2440_soc.h"


/* 115200,8n1 */
void uart0_init()
{
	/* �����������ڴ��� */
	/* GPH2,3����TxD0, RxD0 */
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |= ((2<<4) | (2<<6));

	GPHUP &= ~((1<<2) | (1<<3));  /* ʹ���ڲ����� */
	

	/* ���ò����� */
	/* UBRDIVn = (int)( UART clock / ( buad rate x 16) ) �C1
	 *  UART clock = 50M
	 *  UBRDIVn = (int)( 50000000 / ( 115200 x 16) ) �C1 = 26
	 */
	UCON0 = 0x00000005; /* PCLK,�ж�/��ѯģʽ */
	UBRDIV0 = 26;

	/* �������ݸ�ʽ */
	ULCON0 = 0x00000003; /* 8n1: 8������λ, �޽���λ, 1��ֹͣλ */

	/*  */

}

int putchar(int c)
{
	/* UTRSTAT0 */
	/* UTXH0 */

	while (!(UTRSTAT0 & (1<<2)));
	UTXH0 = (unsigned char)c;
	return 0;
}

int getchar(void)
{
	while (!(UTRSTAT0 & (1<<0)));
	return URXH0;
}

int puts(const char *s)
{
	while (*s)
	{
		putchar(*s);
		s++;
	}
	return 0;
}

/* 0xABCDEF12 */
void printHex(unsigned int val)
{
	int i;
	unsigned int arr[8];

	/* ��ȡ��ÿһλ��ֵ */
	for (i = 0; i < 8; i++)
	{
		arr[i] = val & 0xf;
		val >>= 4;   /* arr[0] = 2, arr[1] = 1, arr[2] = 0xF */
	}

	/* ��ӡ */
	puts("0x");
	for (i = 7; i >=0; i--)
	{
		if (arr[i] >= 0 && arr[i] <= 9)
			putchar(arr[i] + '0');
		else if(arr[i] >= 0xA && arr[i] <= 0xF)
			putchar(arr[i] - 0xA + 'A');
	}
}

void print1(void)
{
	puts("abc\n\r");
}

void print2(void)
{
	puts("123\n\r");
}

void printSWIVal(unsigned int *pSWI)
{
	puts("SWI val = ");
	printHex(*pSWI & ~0xff000000);
	puts("\n\r");
}

