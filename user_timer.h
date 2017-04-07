#ifndef _USER_TIMER_H
#define _USER_TIMER_H

#include	"STC15Fxxxx.H"


void Timer0_init(void);		//Timer0初始化
void Timer0_on(void);		//打开定时器0
void Timer0_off(void);		//关闭定时器0

void Timer2_init(void);		//Timer2初始化
void Timer2_100us_on(void);		//打开定时器2-100us定时
void Timer2_10ms_on(void);		//打开定时器2-10ms定时
void Timer2_off(void);		//关闭定时器2

#endif

