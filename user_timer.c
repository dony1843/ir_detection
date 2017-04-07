#include "user_timer.h"

void Timer0_init(void)		//13us@11.0592MHz/4
{
	TR0 = 0;		////打开定时器0
	AUXR |= 0x80;		//定时器0时钟1T模式
	TMOD &= 0xF0;		//定时器0模式-16位自动重装
	TL0 = 0xdc;		//定时器0初值
	TH0 = 0xff;		//定时器0初值
	TF0 = 0;		//清除TF0溢出中断标志
	TR0 = 0;		//关闭定时器0
	
	ET0 = 1;		//使能定时器0中断
	PT0 = 1;		//高优先级中断
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
	AUXR |= 0x04;		//定时器2时钟1T模式

	IE2 |= 0x04;	//使能定时器2中断
	AUXR &= 0xef;		//关闭定时器2
}
/*
void Timer2_100us_on(void)		//100us@11.0592MHz/4
{
	T2L = 0xec;		//低8位初值
	T2H = 0xfe;		//高8位初值
	AUXR |= 0x10;
}
*/
void Timer2_10ms_on(void)		//10ms@11.0592MHz/4
{
	T2L = 0x00;		//低8位初值
	T2H = 0x94;		//高8位初值
	AUXR |= 0x10;
}

void Timer2_off(void)
{
	AUXR &= 0xef;
}
	
	



