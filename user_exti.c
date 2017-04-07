#include "user_exti.h"


void Exti_init(void)
{
	
  IT0 = 0;    //设置INT0中断类型 (1:仅下降沿中断 0:上升沿和下降沿均可中断)	
  EX0 = 0;    //禁止INT0中断
//	PX0 = 1;		//高优先级
	
	AUXR2 |= 0x10;		//使能INT2下降沿中断
	
}

