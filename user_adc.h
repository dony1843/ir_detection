#ifndef _USER_ADC_H
#define _USER_ADC_H

#include	"STC15Fxxxx.H"

#define	ADC_P10		0x01	//IO引脚 Px.0
#define	ADC_P11		0x02	//IO引脚 Px.1
#define	ADC_P12		0x04	//IO引脚 Px.2
#define	ADC_P13		0x08	//IO引脚 Px.3
#define	ADC_P14		0x10	//IO引脚 Px.4
#define	ADC_P15		0x20	//IO引脚 Px.5
#define	ADC_P16		0x40	//IO引脚 Px.6
#define	ADC_P17		0x80	//IO引脚 Px.7
#define	ADC_P1_All	0xFF	//IO所有引脚

#define ADC_90T		(3<<5)	//转换时间90个时钟
#define ADC_180T	(2<<5)	//转换时间180个时钟
#define ADC_360T	(1<<5)	//转换时间360个时钟
#define ADC_540T	0				//转换时间540个时钟

#define ADC_FLAG	(1<<4)	//ADC完成标志，需要软件清0
#define ADC_START	(1<<3)	//ADC开始开始控制，自动清0

#define ADC_CH0		0		//ADC通道0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7		//ADC通道7

void ADC_init(void);	//ADC初始化
void	ADC_PowerControl(u8 pwr);		//ADC电源控制
u16	Get_ADC10bitResult(u8 channel);		//读一次ADC结果
u16 ADC_convert(u8 channel);	//ADC转换


#endif
