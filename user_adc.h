#ifndef _USER_ADC_H
#define _USER_ADC_H

#include	"STC15Fxxxx.H"

#define	ADC_P10		0x01	//IO���� Px.0
#define	ADC_P11		0x02	//IO���� Px.1
#define	ADC_P12		0x04	//IO���� Px.2
#define	ADC_P13		0x08	//IO���� Px.3
#define	ADC_P14		0x10	//IO���� Px.4
#define	ADC_P15		0x20	//IO���� Px.5
#define	ADC_P16		0x40	//IO���� Px.6
#define	ADC_P17		0x80	//IO���� Px.7
#define	ADC_P1_All	0xFF	//IO��������

#define ADC_90T		(3<<5)	//ת��ʱ��90��ʱ��
#define ADC_180T	(2<<5)	//ת��ʱ��180��ʱ��
#define ADC_360T	(1<<5)	//ת��ʱ��360��ʱ��
#define ADC_540T	0				//ת��ʱ��540��ʱ��

#define ADC_FLAG	(1<<4)	//ADC��ɱ�־����Ҫ�����0
#define ADC_START	(1<<3)	//ADC��ʼ��ʼ���ƣ��Զ���0

#define ADC_CH0		0		//ADCͨ��0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7		//ADCͨ��7

void ADC_init(void);	//ADC��ʼ��
void	ADC_PowerControl(u8 pwr);		//ADC��Դ����
u16	Get_ADC10bitResult(u8 channel);		//��һ��ADC���
u16 ADC_convert(u8 channel);	//ADCת��


#endif
