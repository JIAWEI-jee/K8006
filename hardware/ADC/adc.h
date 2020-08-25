#ifndef __ADC_H__
#define __ADC_H__
#include "HC89F3541.H"



void adc_init(void);

void get_ADC_val ( u16* PTC_voltage_ch7 ,u16* RTD1_voltage_ch2 );
void calc_InputAndInsert_val(u16 * input_voltage_ch1, u16 * insert_voltage_ch3);


#endif
