
#include "s3c2440_soc.h"
#define TIMER_NUM  32
#define NULL  ((void *)0)

typedef void(*timer_func)(void);
typedef struct timer_desc {
	char *name;
	timer_func fp;
}timer_desc, *p_timer_desc;

timer_desc timer_array[TIMER_NUM];

int register_timer(char *name, timer_func fp)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++) {
		if (!timer_array[i].fp){
			timer_array[i].name = name;
			timer_array[i].fp = fp;
			return 0;
		}
	}

	return -1;
}

void unregister_timer(char *name)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++) {
		if (!strcmp(timer_array[i].name, name)){
			timer_array[i].name = NULL;
			timer_array[i].fp = NULL;
		}
	}
}
void timer_irq(void)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++) {
		if (timer_array[i].fp) {
			timer_array[i].fp();
		}
	}
}

void timer_init(void)
{
	/* ����TIMER0��ʱ�� */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	             = 50000000/(99+1)/16
	             = 31250
	 */
	TCFG0 = 49;  /* Prescaler 0 = 49, ����timer0,1 */
	TCFG1 &= ~0xf;
	TCFG1 |= 3;  /* MUX0 : 1/16 */

	/* ����TIMER0�ĳ�ֵ */
	TCNTB0 = 15625;  /* 0.5s�ж�һ�� */

	/* ���س�ֵ, ����timer0 */
	TCON |= (1<<1);   /* Update from TCNTB0 & TCMPB0 */

	/* ����Ϊ�Զ����ز����� */
	TCON &= ~(1<<1);
	TCON |= (1<<0) | (1<<3);  /* bit0: start, bit3: auto reload */

	/* �����ж� */
	register_irq(10, timer_irq);
}


