
#include "s3c2440_soc.h"

void delay(volatile int d)
{
	while (d--);
}

/* 每10ms该函数被调用一次 
 * 每500ms操作一下LED实现计数
 */
void led_timer_irq(void)
{
	/* 点灯计数 */
	static int timer_num = 0;
	static int cnt = 0;
	int tmp;

	timer_num++;
	if (timer_num < 50)
		return;
	timer_num = 0;

	cnt++;

	tmp = ~cnt;
	tmp &= 7;
	GPFDAT &= ~(7<<4);
	GPFDAT |= (tmp<<4);
}

int led_init(void)
{
	/* ����GPFCON��GPF4/5/6����Ϊ������� */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));

	register_timer("led", led_timer_irq);
}


int led_test(void)
{
	int val = 0;  /* val: 0b000, 0b111 */
	int tmp;

	/* ����GPFCON��GPF4/5/6����Ϊ������� */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));

	/* ѭ������ */
	while (1)
	{
		tmp = ~val;
		tmp &= 7;
		GPFDAT &= ~(7<<4);
		GPFDAT |= (tmp<<4);
		delay(100000);
		val++;
		if (val == 8)
			val =0;
		
	}

	return 0;
}



