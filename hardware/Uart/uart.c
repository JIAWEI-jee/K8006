#include "uart.h"
bit guc_Uartflag = 0;					//UART判断标志位
unsigned char guc_Uartbuf = 0;			//用于存放接收收据
//初始化串口9600
void uart_init(void)
{
	#if _DEBUG_PRINTF_
	P2M6 =  0xC1;		//P26设置为推挽输出   0xC1
	P2M5 = 0x60;		//P25设置为上拉输入 0x60
	TXD_MAP = 0x26;		//TXD映射P26
	RXD_MAP = 0x25;		//RXD映射P25	
	
	//波特率计算
	//波特率 = 1/16 * (T4时钟源频率 / 定时器4预分频比) / (65536 - 0xFF98)
	//       = 1/16 * ((16000000 / 1) / 104)
	//		 = 9615.38(误差0.16%)

	//波特率9600
	//反推初值 = (65536 - ((T4时钟源频率 / 定时器4预分频比) * (1 / 16)) / 波特率)
	//		   = (65536 - (16000000 * (1 / 16) / 9600))
	//		   = (65536 - 104.167)
	//         = FF98
	// TH4 = 0xFF;
	// TL4 = 0x98;			//波特率9600
	TH4 = 0xFF;//9600  4M 波特率9600
	TL4 = 0x98;	
	T4CON = 0x06;		//T4工作模式：UART1波特率发生器
	SCON2 = 0x02;		//8位UART，波特率可变 方式一
	SCON = 0x10;		//允许串行接收
	ES1 = 1;			//使能串口中断
	#endif
}


//串口发送一个字符
static void hal_uart_putchar(char Char)
{
	ES1= 0;				//失能UART1中断
	SBUF = Char;
	while(!TI);
	TI = 0;
	ES1= 1;				//UART1接收使能
}

//重写stdio 中的putchar函数  实现printf
char putchar(char c)
{
	hal_uart_putchar(c);
	return c;
}

/***************************************************************************************
  * @说明  	UART1中断服务函数
  *	@参数	无
  * @返回值 无
  * @注		无
***************************************************************************************/
void UART1_Rpt(void) interrupt UART1_VECTOR
{
	if(SCON & 0x01)						//判断接收中断标志位
	{
		guc_Uartbuf = SBUF;				//转存8位串口接收数据
		SCON &=~ 0x10;					//失能UART1接收
		guc_Uartflag = 1;
		SCON &=~ 0x01;					//清除接收中断标志位
	}									
}
