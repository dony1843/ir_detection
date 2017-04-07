#include "user_adc.h"

//========================================================================
// 函数: void	ADC_init(void)
// 描述: ADC初始化函数
// 参数: none
// 返回: none.
// 版本: V1.0
//========================================================================
void ADC_init(void)
{
	P1ASF = ADC_P10;		//P1.0作为ADC输入口
	ADC_CONTR = (ADC_CONTR & 0x97) | ADC_360T;		//AD转换速度
	ADC_CONTR &= 0x7F;		//关闭ADC电源
	PCON2 |=  (1<<5);		//10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
	EADC = 0;		//禁止ADC中断
	PADC = 0;		//低中断优先级
}

//========================================================================
// 函数: void	ADC_PowerControl(u8 pwr)
// 描述: ADC电源控制程序.
// 参数: pwr: 电源控制,ENABLE或DISABLE.
// 返回: none.
// 版本: V1.0
//========================================================================
void	ADC_PowerControl(u8 pwr)
{
	if(pwr == ENABLE)	ADC_CONTR |= 0x80;
	else				ADC_CONTR &= 0x7f;
}

//========================================================================
// 函数: u16	Get_ADC10bitResult(u8 channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC通道.
// 返回: 10位ADC结果.
// 版本: V1.0
//========================================================================
u16	Get_ADC10bitResult(u8 channel)	//channel = 0~7
{
	u16	adc;
	u8	i;

	if(channel > ADC_CH7)	return	1024;	//错误,返回1024,调用的程序判断	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	NOP(4);			//对ADC_CONTR操作后要4T之后才能访问

	for(i=0; i<250; i++)		//超时
	{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			if(PCON2 &  (1<<5))		//10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
			{
				adc = (u16)(ADC_RES & 3);
				adc = (adc << 8) | ADC_RESL;
			}
			else		//10位AD结果的高8位放ADC_RES，低2位在ADC_RESL的低2位。
			{
				adc = (u16)ADC_RES;
				adc = (adc << 2) | (ADC_RESL & 3);
			}
			return	adc;
		}
	}
	return	1024;	//错误,返回1024,调用的程序判断
}

//========================================================================
// 函数: void	ADC_convert(u8 channel)
// 描述: ADC转换程序.
// 参数: channel:转换通道，ADC_CH0 ~ ADC_CH7
// 返回: 3次转换的平均值.
// 版本: V1.0
//========================================================================
u16 ADC_convert(u8 channel)
{
	u8 i;
	u16 adc_res[3] = {0};
	u16 adc_aver = 0;
	
	ADC_PowerControl(ENABLE);	//打开ADC电源
	for(i=0;i<3;i++)		//转换3次，求平均
	{
		adc_res[i]=Get_ADC10bitResult(channel);
	}
	adc_aver = adc_res[0]+adc_res[1]+adc_res[2];
	adc_aver /= 3;
	ADC_PowerControl(DISABLE);	//关闭ADC电源
	return adc_aver;
}



