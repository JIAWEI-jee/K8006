#ifndef __TIMER_H__
#define __TIMER_H__
#include "HC89F3541.H"

#define overrang_time   (60*60*2)
#define Heat_Value      (30*9)

#define Length          10
#define Temperature_Value         133.0000


extern u8 Heat_start_std;
extern u8  Open_Heat_Value;

void time0_init(void);
void set_time_sec_val(u16 sec);
void set_time_sec(void);
#endif

