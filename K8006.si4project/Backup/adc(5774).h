#ifndef __ADC_H__
#define __ADC_H__
#include "HC89F3541.H"

void adc_init(void);
//u16 get_adc_val(void);
void get_voltage ( u16* u1_voltage,u16* u3_voltage );

#endif
