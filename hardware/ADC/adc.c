#include "adc.h"
#include "delay.h"


void adc_init ( void )
{
	P0M3 = 0x01;		//P03����Ϊģ������
	P0M2 = 0x01;		//P02����Ϊģ������
	P0M7 = 0x01;        //P07����Ϊģ������
    P2M2 = 0xC1;        //P26����Ϊ�������/�˿��������10mA
	ADCC0 = 0x81;		//��ADCת����Դ			
	//00 VDD
//											01 �ڲ�4V
//											10 �ڲ�3V
//											11 �ڲ�2V
//1�� �ڲ��ο���ѹѡ��Ϊ 2V ʱ�� VDD ��ѹ����� 2.7V��
//    �ڲ��ο���ѹѡ��Ϊ 3V ʱ�� VDD ��ѹ����� 3.5V��
//    �ڲ��ο���ѹѡ��Ϊ 4V ʱ�� VDD ��ѹ�����4.5V��
//2�� ϵͳ�������ģʽǰ�����齫 ADC �ο���ѹѡ��� VDD�����Խ�һ������ϵͳ���ġ�
	delay_us ( 80 );						//��ʱ20us��ȷ��ADCϵͳ�ȶ�

//	ADCC1 = 0x08;						//ѡ��ADCͨ��8
	ADCC2 = 0x41;					//ת�����12λ���ݣ������Ҷ��룬ADCʱ��4��Ƶ 4M/4 = 1MHz
}

u16 get_adc_val_P02 ( void )
{
	u16 adc_val = 0;
	ADCC1 = 0x02;						//ѡ��ADCͨ��7
	ADCC0 |= 0x40;					//����ADCת��
	while ( ! ( ADCC0&0x20 ) );			//�ȴ�ADCת������
	ADCC0 &=~ 0x20;					//�����־λ
	adc_val = ADCR;					//��ȡADC��ֵ
//	ADCR = 0;
	return adc_val;
}



u16 get_adc_val_P07 ( void )
{
	u16 adc_val = 0;
	ADCC1 = 0x07;						//ѡ��ADCͨ��7
	ADCC0 |= 0x40;					//����ADCת��
	while ( ! ( ADCC0&0x20 ) );			//�ȴ�ADCת������
	ADCC0 &=~ 0x20;					//�����־λ
	adc_val = ADCR;					//��ȡADC��ֵ
//	ADCR = 0;
	return adc_val;
}

u16 get_adc_val_P03 ( void )
{
	u16 adc_val = 0;
	ADCC1 = 0x03;						//ѡ��ADCͨ��3
	ADCC0 |= 0x40;					//����ADCת��
	while ( ! ( ADCC0&0x20 ) );			//�ȴ�ADCת������
	ADCC0 &=~ 0x20;					//�����־λ
	adc_val = ADCR;					//��ȡADC��ֵ
//	ADCR = 0;
	return adc_val;
}


void get_ADC_val ( u16* PTC_voltage_ch7 ,u16* RTD2_voltage_ch3 )
{
	u32 temp = 0;
    temp = get_adc_val_P07();
	*PTC_voltage_ch7 = temp;
	delay_us ( 20 );
	temp = 0;
	temp = get_adc_val_P03();
	*RTD2_voltage_ch3 = temp;
}


