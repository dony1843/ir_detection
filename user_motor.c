#include "user_motor.h"

void motor_run(void)		//�������
{
	motor_stop_pin = 0;
	motor_run_pin = 1;
}

void motor_stop(void)		//����ƶ�
{
	motor_run_pin = 0;
	motor_stop_pin = 1;
}


	