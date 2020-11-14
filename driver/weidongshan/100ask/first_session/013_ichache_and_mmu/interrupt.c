#include "s3c2440_soc.h"

typedef void(*irq_func)(int);
irq_func irq_array[32];


/* SRCPND ������ʾ�ĸ��жϲ�����, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK ���������ж�, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND ������ʾ��ǰ���ȼ���ߵġ����ڷ������ж�, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTOFFSET : ������ʾINTPND����һλ������Ϊ1
 */

/* ��ʼ���жϿ����� */
void interrupt_init(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
	INTMSK &= ~(1<<10);  /* enable timer0 int */
}

/* ��EINTPEND�ֱ����ĸ�EINT����(eint4~23)
 * ����ж�ʱ, дEINTPEND����Ӧλ
 */


void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;
	
	if (irq == 0) /* eint0 : s2 ���� D12 */
	{
		if (val1 & (1<<0)) /* s2 --> gpf6 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<6);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<6);
		}
		
	}
	else if (irq == 2) /* eint2 : s3 ���� D11 */
	{
		if (val1 & (1<<2)) /* s3 --> gpf5 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<5);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<5);
		}
		
	}
	else if (irq == 5) /* eint8_23, eint11--s4 ���� D10, eint19---s5 ��������LED */
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (val2 & (1<<3)) /* s4 --> gpf4 */
			{
				/* �ɿ� */
				GPFDAT |= (1<<4);
			}
			else
			{
				/* ���� */
				GPFDAT &= ~(1<<4);
			}
		}
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* �ɿ� */
				/* Ϩ������LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* ����: ��������LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}

	EINTPEND = val;
}


void handle_irq_c(void)
{
	/* �ֱ��ж�Դ */
	int bit = INTOFFSET;

	/* ���ö�Ӧ�Ĵ����� */
	irq_array[bit](bit);
	
	/* ���ж� : ��Դͷ��ʼ�� */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);	
}

void register_irq(int irq, irq_func fp)
{
	irq_array[irq] = fp;

	INTMSK &= ~(1<<irq);
}


/* ��ʼ������, ��Ϊ�ж�Դ */
void key_eint_init(void)
{
	/* ����GPIOΪ�ж����� */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3������Ϊ�ж����� */

	GPGCON &= ~((3<<6) | (3<<22));
	GPGCON |= ((2<<6) | (2<<22));   /* S4,S5������Ϊ�ж����� */
	

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* ����EINTMASKʹ��eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));

	register_irq(0, key_eint_irq);
	register_irq(2, key_eint_irq);
	register_irq(5, key_eint_irq);
}

