/*************************************************

日期：2016-12-10
版本：Ver1.0
说明：主频率（即外部晶振频率或内部RC时钟频率）11.0592MHz

************************************************/
/************************************************
				IO功能定义 
		P1.2------led_green			//绿灯
		P1.3------led_yellow		//黄灯
		P3.6------brake_switch	//电机制动开关，中断
		P1.0------ADC0					//检测电池电压
		P1.1------motor_run			//电机控制信号
		P3.7------ir_emit				//红外发射控制
		P3.2------ir_transmit		//红外接收信号，中断
		P3.3------key						//出液次数选择，中断
		
		P1.4------irm3638				//控制红外接收管GND脚
		P1.5------motor_stop		//停止信号让电机快速停下(短接制动)
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

//PCA模块初始化函数
void PCA_init(void)		//1ms定时
{
	
	AUXR1 = (AUXR1 & 0xcf)|(0<<4);		//CCP管脚选择
	
	CCON = 0x00;		//初始化PCA控制寄存器
									//PCA定时器停止
									//清除中断标志
	
	CMOD = 0x08;		//空闲模式下PCA计数器继续工作
									//PCA时钟为sysclk/1
									//禁止PCA计数溢出中断

	CL = 0;		//复位PCA寄存器
	CH = 0;
	value_us = 2765;		//11.0592MHz/4 * 1000 us 
	value = value_us;
  CCAP0L = value;		//
  CCAP0H = value >> 8;    //初始化PCA模块0
	value += value_us;
  CCAPM0 = 0x49; 	//PCA模块0为16位定时器模式
									//使能CCF0中断
	
	CR = 1;		//PCA定时器开始工作
}

/******************** 主函数 **************************/
void main(void)
{
//	CLK_DIV = 0x01;		//系统时钟为主时钟2分频
	CLK_DIV = 0x02;		//系统时钟为主时钟4分频

	P1M1 |= 0x01;		//0000 0001b		//PxM1/PxM0 , 00-准双向；01-推挽输出；10-高阻输入；11-开漏
	P1M0 |= 0x3e;		//0011 1110b		//P10-高阻输入；P11~P15-推挽输出；
	P11 = 0;
	P14 = 1;
	P15 = 0;
	
	P3M1 |= 0x00;		//0000 0000b
	P3M0 |= 0x80;		//1000 0000b		//P37设置成推挽输出
	
	ir_emit_pin = 1;		//禁止红外发射
	irm3638_pin = 1;		//禁止接收管工作
	
	ADC_init();		//ADC初始化
	Timer0_init();		//Timer0初始化
	Timer2_init();		//Timer2初始化
	Exti_init();		//外部中断初始化
//	PCA_init();		//PCA模块初始化
	
	EA = 1;		//开全局中断
	
	//看门狗设置
	WDT_CONTR = 0x04;		//设置看门狗定时器分频数为32
											//看门狗定时器溢出时间计算公式：(12 * 32768 * PS) / FOSC
											//溢出时间：(12 * 32768 * 32) / 11059200 = 1.14s
	WDT_CONTR |= 0x20;		//启动看门狗
	
	Led_green_on();		//开绿灯
	
	WKTCL = 0x53;                     //设置唤醒周期为550us*(339+1) = 187ms
  WKTCH = 0x81;                   //使能掉电唤醒定时器
    
  while (1)
  {
		if(power_down_flag && (!motor_run_flag) && (!motor_brake_flag) && (!hand_not_leave_flag) && (!power_low_flag))
		{
			PCON |= 0x02;		//进入掉电模式
			_nop_();		//掉电模式被唤醒后直接从此处向下执行
			_nop_();
			power_down_flag = 0;		//清除掉电标志
			
			ir_emit_flag = 1;		//允许红外发射标志
			ir_emit_flag2 = 0;
			ir_emit_count = 0;
			PCA_init();		//PCA模块初始化
		}
			
		if(!power_down_flag || motor_run_flag || motor_brake_flag)
		{
			PCON |= 0x01;		//进入空闲模式
			_nop_();		//唤醒后从此处开始执行
			_nop_();
			_nop_();
			_nop_();
		}
	}
}

//PCA模块工作于定时器模式中断程序
void PCA_isr() interrupt 7		//1ms中断一次
{
	CCF0 = 0;    //清除中断标志
  CCAP0L = value;
  CCAP0H = value >> 8; 	//更新比较值
	value += value_us;
	
	wdt_count++;
	if(wdt_count == 100)		//0.1s
	{
		wdt_count = 0;
		WDT_CONTR |= 0x10;		//喂狗
	}
	
	if(ir_emit_flag)		//发射持续时间1ms的38KHz红外信号
	{
		ir_emit_count++;
			if(ir_emit_count == 4)		//187 + 4ms
			{
				irm3638_pin = 0;		//允许接收管工作
			}
			if(ir_emit_count == 5)		//187 + 5ms
			{
				IE0 = 0;		//清除中断请求标志
				EX0 = 1;		//允许接收中断
			}
			if(ir_emit_count == 6)		//
			{
				ir_emit_flag2 = 1;		//发射38kHz红外信号
				Timer0_on();		//打开定时器0，准备发射38kHz红外信号
			}
			if(ir_emit_count == 11)		//时间到停止发射
			{
				ir_emit_flag2 = 0;		//停止发射
				ir_emit_pin = 1;
			}
			if(ir_emit_count == 12)		//
			{
				EX0 = 0;		//禁止接收中断
				Timer0_off();		//关闭定时器0
			}
			if(ir_emit_count == 13)		//187 + 13ms = 200ms
			{
				if((!motor_run_flag) && (!motor_brake_flag))		//正常待机时，187ms从掉电模式恢复
				{
					ir_emit_count = 0;
					led_count++;
					if(liquid_num_flag_pin && (liquid_count == 1))		//出液两次标志且已经出液一次
					{
						liquid_time++;
						if(liquid_time == 2)		//超过400ms不再取液
						{
							liquid_time = 0;
							liquid_count = 0;
						}
					}
					irm3638_pin = 1;		//禁止接收管工作
					power_down_flag = 1;		//允许进入掉电模式
					Timer2_off();		//关闭定时器2
				}	
			}
			if(ir_emit_count == 203)
			{
				if((!motor_run_flag) && motor_brake_flag)		//电机制动后
				{
					ir_emit_count = 3;	
				}
			}
		}
	
	if((!motor_run_flag) && (!motor_brake_flag) && (!hand_not_leave_flag) && (!power_low_flag))		//正常状态绿灯闪烁
	{
		if(led_count == 3)		//0.6s
		{
			Led_green_off();		//绿灯灭
		}
		if(led_count == 25)		//5s
		{
			led_count = 0;
			Led_green_on();		//绿灯亮
		}
	}
	if(power_low_flag && motor_run_flag && (!motor_brake_flag))		//低压标志
	{
		led_yellow_count++;
		if(led_yellow_count == 100)		//100ms
		{
			led_yellow_count = 0;
			Led_yellow_state_not();		//黄灯反转
		}
	}
	
	if(falling_edge_flag)		//下降沿开始计时
	{
		falling_edge_time++;		//1ms加1
	}
}

//定时器0中断服务程序
void Timer0_isr() interrupt 1		//13us中断一次
{
	if(ir_emit_flag2)
	{
		ir_emit_pin = !ir_emit_pin;		//发射38k信号
	}
}

//Timer2中断服务程序
void Timer2_isr() interrupt 12		//
{
	if(motor_run_flag &&(!motor_brake_flag))		//电机在运行
	{
		timer2_count++;		//10ms加1
		if(timer2_count >= 400)	//电机运行超过4秒
		{
			timer2_count = 0;
			motor_run_flag = 0;		//清除电机运行标志
			motor_brake_flag = 1;		//置位电机制动标志
			
			if(power_low_flag)		//低压标志
			{
				Led_yellow_off();		//制动时关闭黄灯
				power_low_flag = 0;		//清除低压标志
			}
			
			Timer2_off();		//关闭定时器2
	//		Led_yellow_off();
			motor_stop();		//电机停止
			Timer2_10ms_on();		//打开定时器2-100us定时
			
			//允许红外发射	
			ir_emit_flag = 1;
			ir_emit_flag2 = 0;
			ir_emit_count = 0;
			//允许接收管工作
			irm3638_pin = 0;
		}
	}
	
	if(motor_brake_flag && (!motor_run_flag))	//电机制动
	{
		if(!hand_not_leave_flag)
		{
			timer2_count++;		//10ms加1
			if(timer2_count == 30)		//400ms
			{
				motor_stop_pin = 0;		//断开停止信号
			
				if(liquid_num_flag_pin)		//出液2次标志
				{
					if(liquid_count == 1)		//出液1次
					{
						motor_brake_flag = 0;		//清除制动标志
						timer2_count = 0;
						led_count = 0;
						liquid_time = 0;
						Led_green_on();		//开绿灯
						Timer2_off();
						power_down_flag = 1;	//允许进入掉电模式
					}
					if(liquid_count == 2)		//出液2次
					{
						liquid_count = 0;		//清零出液计数
					}
				}
			}	
			if(timer2_count == 100)		//1s
			{	
				timer2_count = 0;
				if(hand_count < 5)		//检测到手5次以下
				{
					hand_count = 0;
					motor_brake_flag = 0;		//清除制动标志
					led_count = 0;
					Led_green_on();		//开绿灯
					Timer2_off();
					power_down_flag = 1;		//允许进入掉电模式
				}
				else
				{
					hand_not_leave_flag = 1;		//手没离开标志
					Led_yellow_on();
					hand_not_leave_count = 0;
				}
			}
		}
		else		//手没离开
		{
			hand_not_leave_count++;
			if(hand_not_leave_count == 25)		//250ms
			{
				hand_not_leave_count =0;
				Led_yellow_state_not();		//黄灯反转
			}
			if(hand_temp < hand_count)		//比较两次手计数值
			{
				hand_temp = hand_count;
				i=0;
			}
			else
			{
				i++;
				if(i >= 60)		//超过600ms没检测到手
				{
					ir_emit_flag = 0;		//禁止红外发射标志
					ir_emit_flag2 = 0;
					ir_emit_pin = 1;
				}
				if(i>= 80)		//800ms
				{
					i = 0;
					hand_temp = 0;
					hand_count = 0;
					hand_not_leave_flag = 0;		//清除手没离开标志
					Led_yellow_off();
					motor_brake_flag = 0;		//清除制动标志
					led_count = 0;
					Led_green_on();		//开绿灯
					Timer2_off();
					power_down_flag = 1;	//允许进入掉电模式
				}
			}
		}
	}
}

//INT0中断服务程序
void Exint0_isr() interrupt 0		//手靠近
{
	power_down_flag = 0;		//禁止进入掉电模式
	if(!INT0)		//下降沿
	{
		falling_edge_flag = 1;		//下降沿标志置位
		falling_edge_time = 0;		//下降沿计时清零		
	}
	else		//上升沿中断
	{
		falling_edge_flag = 0;		//下降沿标志清零
			
		if((!motor_brake_flag) && (!motor_run_flag))	//非电机制动,启动电机
		{
			if(falling_edge_time > 4 && falling_edge_time < 6)			//收到信号时间在4ms~6ms之间才操作
			{
				Led_green_off();		//关绿灯
				
				Timer2_off();		//关闭定时器2
					
				//禁止红外发射
				ir_emit_flag = 0;
				ir_emit_flag2 = 0;
				ir_emit_pin = 1;
				//禁止接收管工作
				irm3638_pin = 1;
				
				if(liquid_num_flag_pin)		//连续出液2次
				{
					liquid_count++;			//出液次数累加
				}
				
				adc_temp = ADC_convert(ADC_CH0);		//AD转换  间接测量电源电压
				if(adc_temp < 530)		//电源电压低于5V
				{
					power_low_flag = 1;		//低压标志
					Led_yellow_on();		//开黄灯
					led_yellow_count = 0;
				}
	//			Led_yellow_on();
				motor_run();		//电机运行
				motor_run_flag = 1;		//电机运行标志
				motor_brake_flag = 0;		//清除制动标志
				Timer2_10ms_on();		//打开定时器2-10ms定时
			}
		}
		if(motor_brake_flag && (!motor_run_flag))		//电机制动
		{
			if(falling_edge_time > 4 && falling_edge_time <= 8)			//收到信号时间在4ms~8ms之间才操作
			{
				hand_count++;		//检测到手的次数
			}
		}
		falling_edge_time = 0;		//下降沿计时清零
	}
}

//INT2中断服务程序
void Exint2_isr() interrupt 10		//电机制动
{
	if(motor_run_flag && (!motor_brake_flag))		//电机在运行
	{
		if(timer2_count > 40)	//电机运行时间超过400ms，才允许刹车
		{
			motor_brake_flag = 1;		//置位制动标志
			motor_run_flag = 0;		//清除电机运行标志
			
			Timer2_off();		//关闭定时器2
			if(power_low_flag)		//低压标志
			{
				Led_yellow_off();		//制动时关闭黄灯
				power_low_flag = 0;		//清除低压标志
			}
	//		Led_yellow_off();		//关黄灯
			motor_stop();		//电机停止
			timer2_count = 0;
			Timer2_10ms_on();		//打开定时器2-10ms定时
			
			if(!liquid_num_flag_pin || (liquid_num_flag_pin && (liquid_count == 2)))
			{
				//允许红外发射
				ir_emit_flag = 1;
				ir_emit_flag2 = 0;
				ir_emit_count = 0;
				//允许接收管工作
				irm3638_pin = 0;
			}
		}
	}
}

