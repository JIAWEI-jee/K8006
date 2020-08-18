#include "adc.h"
#include "delay.h"


void adc_init ( void )
{
	P0M3 = 0x01;		//P03设置为模拟输入
	P0M2 = 0x01;		//P02设置为模拟输入
	P0M7 = 0x01;        //P07设置为模拟输入
    P2M2 = 0xC1;        //P26设置为推挽输出/端口输出能力10mA
	ADCC0 = 0x81;		//打开ADC转换电源			
	//00 VDD
//											01 内部4V
//											10 内部3V
//											11 内部2V
//1、 内部参考电压选择为 2V 时， VDD 电压须高于 2.7V；
//    内部参考电压选择为 3V 时， VDD 电压须高于 3.5V；
//    内部参考电压选择为 4V 时， VDD 电压须高于4.5V。
//2、 系统进入掉电模式前，建议将 ADC 参考电压选择非 VDD，可以进一步降低系统功耗。
	delay_us ( 80 );						//延时20us，确保ADC系统稳定

//	ADCC1 = 0x08;						//选择ADC通道8
	ADCC2 = 0x41;					//转换结果12位数据，数据右对齐，ADC时钟4分频 4M/4 = 1MHz
}

u16 get_adc_val_P02 ( void )
{
	u16 adc_val = 0;
	ADCC1 = 0x02;						//选择ADC通道7
	ADCC0 |= 0x40;					//启动ADC转换
	while ( ! ( ADCC0&0x20 ) );			//等待ADC转换结束
	ADCC0 &=~ 0x20;					//清除标志位
	adc_val = ADCR;					//获取ADC的值
//	ADCR = 0;
	return adc_val;
}



u16 get_adc_val_P07 ( void )
{
	u16 adc_val = 0;
	ADCC1 = 0x07;						//选择ADC通道7
	ADCC0 |= 0x40;					//启动ADC转换
	while ( ! ( ADCC0&0x20 ) );			//等待ADC转换结束
	ADCC0 &=~ 0x20;					//清除标志位
	adc_val = ADCR;					//获取ADC的值
//	ADCR = 0;
	return adc_val;
}

u16 get_adc_val_P03 ( void )
{
	u16 adc_val = 0;
	ADCC1 = 0x03;						//选择ADC通道3
	ADCC0 |= 0x40;					//启动ADC转换
	while ( ! ( ADCC0&0x20 ) );			//等待ADC转换结束
	ADCC0 &=~ 0x20;					//清除标志位
	adc_val = ADCR;					//获取ADC的值
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


