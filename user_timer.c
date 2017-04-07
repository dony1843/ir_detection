#include "user_timer.h"

void Timer0_init(void)		//13us@11.0592MHz/4
{
	TR0 = 0;		////�򿪶�ʱ��0
	AUXR |= 0x80;		//��ʱ��0ʱ��1Tģʽ
	TMOD &= 0xF0;		//��ʱ��0ģʽ-16λ�Զ���װ
	TL0 = 0xdc;		//��ʱ��0��ֵ
	TH0 = 0xff;		//��ʱ��0��ֵ
	TF0 = 0;		//���TF0����жϱ�־
	TR0 = 0;		//�رն�ʱ��0
	
	ET0 = 1;		//ʹ�ܶ�ʱ��0�ж�
	PT0 = 1;		//�����ȼ��ж�
}

void Timer0_on(void)
{
	TR0 = 1;
}

void Timer0_off(void)
{
	TR0 = 0;
}

void Timer2_init(void)		//@11.0592MHz/4
{
	AUXR |= 0x04;		//��ʱ��2ʱ��1Tģʽ

	IE2 |= 0x04;	//ʹ�ܶ�ʱ��2�ж�
	AUXR &= 0xef;		//�رն�ʱ��2
}
/*
void Timer2_100us_on(void)		//100us@11.0592MHz/4
{
	T2L = 0xec;		//��8λ��ֵ
	T2H = 0xfe;		//��8λ��ֵ
	AUXR |= 0x10;
}
*/
void Timer2_10ms_on(void)		//10ms@11.0592MHz/4
{
	T2L = 0x00;		//��8λ��ֵ
	T2H = 0x94;		//��8λ��ֵ
	AUXR |= 0x10;
}

void Timer2_off(void)
{
	AUXR &= 0xef;
}
	
	



