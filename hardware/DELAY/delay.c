#include "delay.h"
#include "intrins.h"//nop

void delay_us(u16 us)//16MHz
{
	while(--us)
	{
		_nop_();_nop_();
	}
}

void delay_ms(u16 ms)    
{  
    u16 i;  
    do{  
        i=(MAIN_Fosc/9600)*2;  
        while(--i);     //96T per loop  
    }while(--ms);       //--ms  ms=ms-1  
} 

//void lcd_delay_us(u16 time)
//{
//	delay_us(time);
//}



