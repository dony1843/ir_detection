#include "user_exti.h"


void Exti_init(void)
{
	
  IT0 = 0;    //����INT0�ж����� (1:���½����ж� 0:�����غ��½��ؾ����ж�)	
  EX0 = 0;    //��ֹINT0�ж�
//	PX0 = 1;		//�����ȼ�
	
	AUXR2 |= 0x10;		//ʹ��INT2�½����ж�
	
}

