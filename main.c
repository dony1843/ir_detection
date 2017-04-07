/*************************************************

���ڣ�2016-12-10
�汾��Ver1.0
˵������Ƶ�ʣ����ⲿ����Ƶ�ʻ��ڲ�RCʱ��Ƶ�ʣ�11.0592MHz

************************************************/
/************************************************
				IO���ܶ��� 
		P1.2------led_green			//�̵�
		P1.3------led_yellow		//�Ƶ�
		P3.6------brake_switch	//����ƶ����أ��ж�
		P1.0------ADC0					//����ص�ѹ
		P1.1------motor_run			//��������ź�
		P3.7------ir_emit				//���ⷢ�����
		P3.2------ir_transmit		//��������źţ��ж�
		P3.3------key						//��Һ����ѡ���ж�
		
		P1.4------irm3638				//���ƺ�����չ�GND��
		P1.5------motor_stop		//ֹͣ�ź��õ������ͣ��(�̽��ƶ�)
************************************************/

#include	"STC15Fxxxx.H"
#include "user_led.h"
#include "user_adc.h"
#include "user_timer.h"
#include "user_motor.h"
#include "user_exti.h"

#define ir_emit_pin P37
#define irm3638_pin P14
#define liquid_num_flag_pin P33

u16 adc_temp = 0;

u16 i = 0;
u16 timer2_count = 0;
u16 ir_emit_count = 0;
u16 led_count = 0;
u16 led_yellow_count = 0;
u16 falling_edge_time = 0;
u16 hand_count = 0;
u16 hand_temp = 0;
u16 hand_not_leave_count = 0;
u16 wdt_count = 0;
u16 liquid_count = 0;
u16 liquid_time = 0;

u8 power_down_flag = 1;
u8 ir_emit_flag = 0;
u8 ir_emit_flag2 = 0;
u8 falling_edge_flag = 0;
u8 rising_edge_flag = 0;
u8 power_low_flag = 0;
u8 motor_run_flag = 0;
u8 motor_brake_flag = 0;
u8 hand_not_leave_flag = 0;

u16 value_us;
u16 value;

//PCAģ���ʼ������
void PCA_init(void)		//1ms��ʱ
{
	
	AUXR1 = (AUXR1 & 0xcf)|(0<<4);		//CCP�ܽ�ѡ��
	
	CCON = 0x00;		//��ʼ��PCA���ƼĴ���
									//PCA��ʱ��ֹͣ
									//����жϱ�־
	
	CMOD = 0x08;		//����ģʽ��PCA��������������
									//PCAʱ��Ϊsysclk/1
									//��ֹPCA��������ж�

	CL = 0;		//��λPCA�Ĵ���
	CH = 0;
	value_us = 2765;		//11.0592MHz/4 * 1000 us 
	value = value_us;
  CCAP0L = value;		//
  CCAP0H = value >> 8;    //��ʼ��PCAģ��0
	value += value_us;
  CCAPM0 = 0x49; 	//PCAģ��0Ϊ16λ��ʱ��ģʽ
									//ʹ��CCF0�ж�
	
	CR = 1;		//PCA��ʱ����ʼ����
}

/******************** ������ **************************/
void main(void)
{
//	CLK_DIV = 0x01;		//ϵͳʱ��Ϊ��ʱ��2��Ƶ
	CLK_DIV = 0x02;		//ϵͳʱ��Ϊ��ʱ��4��Ƶ

	P1M1 |= 0x01;		//0000 0001b		//PxM1/PxM0 , 00-׼˫��01-���������10-�������룻11-��©
	P1M0 |= 0x3e;		//0011 1110b		//P10-�������룻P11~P15-���������
	P11 = 0;
	P14 = 1;
	P15 = 0;
	
	P3M1 |= 0x00;		//0000 0000b
	P3M0 |= 0x80;		//1000 0000b		//P37���ó��������
	
	ir_emit_pin = 1;		//��ֹ���ⷢ��
	irm3638_pin = 1;		//��ֹ���չܹ���
	
	ADC_init();		//ADC��ʼ��
	Timer0_init();		//Timer0��ʼ��
	Timer2_init();		//Timer2��ʼ��
	Exti_init();		//�ⲿ�жϳ�ʼ��
//	PCA_init();		//PCAģ���ʼ��
	
	EA = 1;		//��ȫ���ж�
	
	//���Ź�����
	WDT_CONTR = 0x04;		//���ÿ��Ź���ʱ����Ƶ��Ϊ32
											//���Ź���ʱ�����ʱ����㹫ʽ��(12 * 32768 * PS) / FOSC
											//���ʱ�䣺(12 * 32768 * 32) / 11059200 = 1.14s
	WDT_CONTR |= 0x20;		//�������Ź�
	
	Led_green_on();		//���̵�
	
	WKTCL = 0x53;                     //���û�������Ϊ550us*(339+1) = 187ms
  WKTCH = 0x81;                   //ʹ�ܵ��绽�Ѷ�ʱ��
    
  while (1)
  {
		if(power_down_flag && (!motor_run_flag) && (!motor_brake_flag) && (!hand_not_leave_flag) && (!power_low_flag))
		{
			PCON |= 0x02;		//�������ģʽ
			_nop_();		//����ģʽ�����Ѻ�ֱ�ӴӴ˴�����ִ��
			_nop_();
			power_down_flag = 0;		//��������־
			
			ir_emit_flag = 1;		//������ⷢ���־
			ir_emit_flag2 = 0;
			ir_emit_count = 0;
			PCA_init();		//PCAģ���ʼ��
		}
			
		if(!power_down_flag || motor_run_flag || motor_brake_flag)
		{
			PCON |= 0x01;		//�������ģʽ
			_nop_();		//���Ѻ�Ӵ˴���ʼִ��
			_nop_();
			_nop_();
			_nop_();
		}
	}
}

//PCAģ�鹤���ڶ�ʱ��ģʽ�жϳ���
void PCA_isr() interrupt 7		//1ms�ж�һ��
{
	CCF0 = 0;    //����жϱ�־
  CCAP0L = value;
  CCAP0H = value >> 8; 	//���±Ƚ�ֵ
	value += value_us;
	
	wdt_count++;
	if(wdt_count == 100)		//0.1s
	{
		wdt_count = 0;
		WDT_CONTR |= 0x10;		//ι��
	}
	
	if(ir_emit_flag)		//�������ʱ��1ms��38KHz�����ź�
	{
		ir_emit_count++;
			if(ir_emit_count == 4)		//187 + 4ms
			{
				irm3638_pin = 0;		//������չܹ���
			}
			if(ir_emit_count == 5)		//187 + 5ms
			{
				IE0 = 0;		//����ж������־
				EX0 = 1;		//��������ж�
			}
			if(ir_emit_count == 6)		//
			{
				ir_emit_flag2 = 1;		//����38kHz�����ź�
				Timer0_on();		//�򿪶�ʱ��0��׼������38kHz�����ź�
			}
			if(ir_emit_count == 11)		//ʱ�䵽ֹͣ����
			{
				ir_emit_flag2 = 0;		//ֹͣ����
				ir_emit_pin = 1;
			}
			if(ir_emit_count == 12)		//
			{
				EX0 = 0;		//��ֹ�����ж�
				Timer0_off();		//�رն�ʱ��0
			}
			if(ir_emit_count == 13)		//187 + 13ms = 200ms
			{
				if((!motor_run_flag) && (!motor_brake_flag))		//��������ʱ��187ms�ӵ���ģʽ�ָ�
				{
					ir_emit_count = 0;
					led_count++;
					if(liquid_num_flag_pin && (liquid_count == 1))		//��Һ���α�־���Ѿ���Һһ��
					{
						liquid_time++;
						if(liquid_time == 2)		//����400ms����ȡҺ
						{
							liquid_time = 0;
							liquid_count = 0;
						}
					}
					irm3638_pin = 1;		//��ֹ���չܹ���
					power_down_flag = 1;		//����������ģʽ
					Timer2_off();		//�رն�ʱ��2
				}	
			}
			if(ir_emit_count == 203)
			{
				if((!motor_run_flag) && motor_brake_flag)		//����ƶ���
				{
					ir_emit_count = 3;	
				}
			}
		}
	
	if((!motor_run_flag) && (!motor_brake_flag) && (!hand_not_leave_flag) && (!power_low_flag))		//����״̬�̵���˸
	{
		if(led_count == 3)		//0.6s
		{
			Led_green_off();		//�̵���
		}
		if(led_count == 25)		//5s
		{
			led_count = 0;
			Led_green_on();		//�̵���
		}
	}
	if(power_low_flag && motor_run_flag && (!motor_brake_flag))		//��ѹ��־
	{
		led_yellow_count++;
		if(led_yellow_count == 100)		//100ms
		{
			led_yellow_count = 0;
			Led_yellow_state_not();		//�ƵƷ�ת
		}
	}
	
	if(falling_edge_flag)		//�½��ؿ�ʼ��ʱ
	{
		falling_edge_time++;		//1ms��1
	}
}

//��ʱ��0�жϷ������
void Timer0_isr() interrupt 1		//13us�ж�һ��
{
	if(ir_emit_flag2)
	{
		ir_emit_pin = !ir_emit_pin;		//����38k�ź�
	}
}

//Timer2�жϷ������
void Timer2_isr() interrupt 12		//
{
	if(motor_run_flag &&(!motor_brake_flag))		//���������
	{
		timer2_count++;		//10ms��1
		if(timer2_count >= 400)	//������г���4��
		{
			timer2_count = 0;
			motor_run_flag = 0;		//���������б�־
			motor_brake_flag = 1;		//��λ����ƶ���־
			
			if(power_low_flag)		//��ѹ��־
			{
				Led_yellow_off();		//�ƶ�ʱ�رջƵ�
				power_low_flag = 0;		//�����ѹ��־
			}
			
			Timer2_off();		//�رն�ʱ��2
	//		Led_yellow_off();
			motor_stop();		//���ֹͣ
			Timer2_10ms_on();		//�򿪶�ʱ��2-100us��ʱ
			
			//������ⷢ��	
			ir_emit_flag = 1;
			ir_emit_flag2 = 0;
			ir_emit_count = 0;
			//������չܹ���
			irm3638_pin = 0;
		}
	}
	
	if(motor_brake_flag && (!motor_run_flag))	//����ƶ�
	{
		if(!hand_not_leave_flag)
		{
			timer2_count++;		//10ms��1
			if(timer2_count == 30)		//400ms
			{
				motor_stop_pin = 0;		//�Ͽ�ֹͣ�ź�
			
				if(liquid_num_flag_pin)		//��Һ2�α�־
				{
					if(liquid_count == 1)		//��Һ1��
					{
						motor_brake_flag = 0;		//����ƶ���־
						timer2_count = 0;
						led_count = 0;
						liquid_time = 0;
						Led_green_on();		//���̵�
						Timer2_off();
						power_down_flag = 1;	//����������ģʽ
					}
					if(liquid_count == 2)		//��Һ2��
					{
						liquid_count = 0;		//�����Һ����
					}
				}
			}	
			if(timer2_count == 100)		//1s
			{	
				timer2_count = 0;
				if(hand_count < 5)		//��⵽��5������
				{
					hand_count = 0;
					motor_brake_flag = 0;		//����ƶ���־
					led_count = 0;
					Led_green_on();		//���̵�
					Timer2_off();
					power_down_flag = 1;		//����������ģʽ
				}
				else
				{
					hand_not_leave_flag = 1;		//��û�뿪��־
					Led_yellow_on();
					hand_not_leave_count = 0;
				}
			}
		}
		else		//��û�뿪
		{
			hand_not_leave_count++;
			if(hand_not_leave_count == 25)		//250ms
			{
				hand_not_leave_count =0;
				Led_yellow_state_not();		//�ƵƷ�ת
			}
			if(hand_temp < hand_count)		//�Ƚ������ּ���ֵ
			{
				hand_temp = hand_count;
				i=0;
			}
			else
			{
				i++;
				if(i >= 60)		//����600msû��⵽��
				{
					ir_emit_flag = 0;		//��ֹ���ⷢ���־
					ir_emit_flag2 = 0;
					ir_emit_pin = 1;
				}
				if(i>= 80)		//800ms
				{
					i = 0;
					hand_temp = 0;
					hand_count = 0;
					hand_not_leave_flag = 0;		//�����û�뿪��־
					Led_yellow_off();
					motor_brake_flag = 0;		//����ƶ���־
					led_count = 0;
					Led_green_on();		//���̵�
					Timer2_off();
					power_down_flag = 1;	//����������ģʽ
				}
			}
		}
	}
}

//INT0�жϷ������
void Exint0_isr() interrupt 0		//�ֿ���
{
	power_down_flag = 0;		//��ֹ�������ģʽ
	if(!INT0)		//�½���
	{
		falling_edge_flag = 1;		//�½��ر�־��λ
		falling_edge_time = 0;		//�½��ؼ�ʱ����		
	}
	else		//�������ж�
	{
		falling_edge_flag = 0;		//�½��ر�־����
			
		if((!motor_brake_flag) && (!motor_run_flag))	//�ǵ���ƶ�,�������
		{
			if(falling_edge_time > 4 && falling_edge_time < 6)			//�յ��ź�ʱ����4ms~6ms֮��Ų���
			{
				Led_green_off();		//���̵�
				
				Timer2_off();		//�رն�ʱ��2
					
				//��ֹ���ⷢ��
				ir_emit_flag = 0;
				ir_emit_flag2 = 0;
				ir_emit_pin = 1;
				//��ֹ���չܹ���
				irm3638_pin = 1;
				
				if(liquid_num_flag_pin)		//������Һ2��
				{
					liquid_count++;			//��Һ�����ۼ�
				}
				
				adc_temp = ADC_convert(ADC_CH0);		//ADת��  ��Ӳ�����Դ��ѹ
				if(adc_temp < 530)		//��Դ��ѹ����5V
				{
					power_low_flag = 1;		//��ѹ��־
					Led_yellow_on();		//���Ƶ�
					led_yellow_count = 0;
				}
	//			Led_yellow_on();
				motor_run();		//�������
				motor_run_flag = 1;		//������б�־
				motor_brake_flag = 0;		//����ƶ���־
				Timer2_10ms_on();		//�򿪶�ʱ��2-10ms��ʱ
			}
		}
		if(motor_brake_flag && (!motor_run_flag))		//����ƶ�
		{
			if(falling_edge_time > 4 && falling_edge_time <= 8)			//�յ��ź�ʱ����4ms~8ms֮��Ų���
			{
				hand_count++;		//��⵽�ֵĴ���
			}
		}
		falling_edge_time = 0;		//�½��ؼ�ʱ����
	}
}

//INT2�жϷ������
void Exint2_isr() interrupt 10		//����ƶ�
{
	if(motor_run_flag && (!motor_brake_flag))		//���������
	{
		if(timer2_count > 40)	//�������ʱ�䳬��400ms��������ɲ��
		{
			motor_brake_flag = 1;		//��λ�ƶ���־
			motor_run_flag = 0;		//���������б�־
			
			Timer2_off();		//�رն�ʱ��2
			if(power_low_flag)		//��ѹ��־
			{
				Led_yellow_off();		//�ƶ�ʱ�رջƵ�
				power_low_flag = 0;		//�����ѹ��־
			}
	//		Led_yellow_off();		//�ػƵ�
			motor_stop();		//���ֹͣ
			timer2_count = 0;
			Timer2_10ms_on();		//�򿪶�ʱ��2-10ms��ʱ
			
			if(!liquid_num_flag_pin || (liquid_num_flag_pin && (liquid_count == 2)))
			{
				//������ⷢ��
				ir_emit_flag = 1;
				ir_emit_flag2 = 0;
				ir_emit_count = 0;
				//������չܹ���
				irm3638_pin = 0;
			}
		}
	}
}

