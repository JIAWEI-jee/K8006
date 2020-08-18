#ifndef __ADC_H__
#define __ADC_H__
#include "HC89F3541.H"

void adc_init(void);
//u16 get_adc_val(void);
void get_voltage ( u16* PTC_voltage_ch7,u16* RTD2_voltage_ch3 ,u16* RTD3_voltage_ch2 );

#endif
