#include "user_motor.h"

void motor_run(void)		//电机运行
{
	motor_stop_pin = 0;
	motor_run_pin = 1;
}

void motor_stop(void)		//电机制动
{
	motor_run_pin = 0;
	motor_stop_pin = 1;
}


	