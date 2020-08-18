#ifndef __UART_H__
#define __UART_H__
#include "HC89F3541.H"
#include "stdio.h"//printf

#define _DEBUG_PRINTF_ 1
#define KEY_DEBUG_PRINT   1


#if _DEBUG_PRINTF_
	#define	gm_printf printf
#else
	#define	gm_printf (...)
#endif


#if KEY_DEBUG_PRINT
	#define	KEY_printf printf
#else
	#define	KEY_printf (...)
#endif

void uart_init(void);




#endif


