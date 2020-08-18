#ifndef __WDT_H__
#define __WDT_H__
#include "HC89F3541.H"

void wdt_init(u8 time);
void clear_wdt(void);
#endif