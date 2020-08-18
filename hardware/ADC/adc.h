#ifndef __ADC_H__
#define __ADC_H__
#include "HC89F3541.H"

#define ADC_EN   P2_2

void adc_init(void);

void get_ADC_val ( u16* PTC_voltage_ch7 ,u16* RTD2_voltage_ch3 );

#endif
