#ifndef _USER_TIMER_H
#define _USER_TIMER_H

#include	"STC15Fxxxx.H"


void Timer0_init(void);		//Timer0��ʼ��
void Timer0_on(void);		//�򿪶�ʱ��0
void Timer0_off(void);		//�رն�ʱ��0

void Timer2_init(void);		//Timer2��ʼ��
void Timer2_100us_on(void);		//�򿪶�ʱ��2-100us��ʱ
void Timer2_10ms_on(void);		//�򿪶�ʱ��2-10ms��ʱ
void Timer2_off(void);		//�رն�ʱ��2

#endif

