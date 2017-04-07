#include "user_adc.h"

//========================================================================
// ����: void	ADC_init(void)
// ����: ADC��ʼ������
// ����: none
// ����: none.
// �汾: V1.0
//========================================================================
void ADC_init(void)
{
	P1ASF = ADC_P10;		//P1.0��ΪADC�����
	ADC_CONTR = (ADC_CONTR & 0x97) | ADC_360T;		//ADת���ٶ�
	ADC_CONTR &= 0x7F;		//�ر�ADC��Դ
	PCON2 |=  (1<<5);		//10λAD����ĸ�2λ��ADC_RES�ĵ�2λ����8λ��ADC_RESL��
	EADC = 0;		//��ֹADC�ж�
	PADC = 0;		//���ж����ȼ�
}

//========================================================================
// ����: void	ADC_PowerControl(u8 pwr)
// ����: ADC��Դ���Ƴ���.
// ����: pwr: ��Դ����,ENABLE��DISABLE.
// ����: none.
// �汾: V1.0
//========================================================================
void	ADC_PowerControl(u8 pwr)
{
	if(pwr == ENABLE)	ADC_CONTR |= 0x80;
	else				ADC_CONTR &= 0x7f;
}

//========================================================================
// ����: u16	Get_ADC10bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADCͨ��.
// ����: 10λADC���.
// �汾: V1.0
//========================================================================
u16	Get_ADC10bitResult(u8 channel)	//channel = 0~7
{
	u16	adc;
	u8	i;

	if(channel > ADC_CH7)	return	1024;	//����,����1024,���õĳ����ж�	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	NOP(4);			//��ADC_CONTR������Ҫ4T֮����ܷ���

	for(i=0; i<250; i++)		//��ʱ
	{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			if(PCON2 &  (1<<5))		//10λAD����ĸ�2λ��ADC_RES�ĵ�2λ����8λ��ADC_RESL��
			{
				adc = (u16)(ADC_RES & 3);
				adc = (adc << 8) | ADC_RESL;
			}
			else		//10λAD����ĸ�8λ��ADC_RES����2λ��ADC_RESL�ĵ�2λ��
			{
				adc = (u16)ADC_RES;
				adc = (adc << 2) | (ADC_RESL & 3);
			}
			return	adc;
		}
	}
	return	1024;	//����,����1024,���õĳ����ж�
}

//========================================================================
// ����: void	ADC_convert(u8 channel)
// ����: ADCת������.
// ����: channel:ת��ͨ����ADC_CH0 ~ ADC_CH7
// ����: 3��ת����ƽ��ֵ.
// �汾: V1.0
//========================================================================
u16 ADC_convert(u8 channel)
{
	u8 i;
	u16 adc_res[3] = {0};
	u16 adc_aver = 0;
	
	ADC_PowerControl(ENABLE);	//��ADC��Դ
	for(i=0;i<3;i++)		//ת��3�Σ���ƽ��
	{
		adc_res[i]=Get_ADC10bitResult(channel);
	}
	adc_aver = adc_res[0]+adc_res[1]+adc_res[2];
	adc_aver /= 3;
	ADC_PowerControl(DISABLE);	//�ر�ADC��Դ
	return adc_aver;
}



