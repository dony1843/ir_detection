#include "user_led.h"

#define led_green_pin P12
#define led_yellow_pin P13

void Led_green_on(void)
{
	led_green_pin = 0;
}

void Led_green_off(void)
{
	led_green_pin = 1;
}

void Led_yellow_on(void)
{
	led_yellow_pin = 0;
}

void Led_yellow_off(void)
{
	led_yellow_pin = 1;
}
/*
void Led_green_state_not(void)
{
	led_green_pin = ~led_green_pin;
}
*/
void Led_yellow_state_not(void)
{
	led_yellow_pin = ~led_yellow_pin;
}

