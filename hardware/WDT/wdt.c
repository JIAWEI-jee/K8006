#include "wdt.h"


void wdt_init(u8 time)
{
    WDTC = 0x17;						//允许WDT复位，允许掉电/空闲模式下运行，1024分频
    //WDT使用的是RC44K时钟，WDT_CLOCK_1024是1024分频，初值为0xFF
	//定时时间 	= 1024 * 0xFF / 44000
	//			= 5.93s
	// WDTCCR = 0xFF;						//写入00时，将关闭WDT功能（但不关闭内部低频RC），
										//即相当于禁止WDT。写入非0数据时，将启动WDT。
	WDTCCR = (time)*(43);	//2S   2*44000/1024 = 2*43;
    // WDTCCR = 86;   //2S   2*44000/1024 = 2*43;
}

void clear_wdt(void)
{
	WDTC |= 0x10;                   //清狗									
}





