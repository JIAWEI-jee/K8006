#include "pwm.h"
u8 pwm_period = 0;

void set_pwm(u8 duty)
{
	u32 tep = (u32)duty*pwm_period/100;
	PWM3D = tep;
}

void pwm_init(u16 hz)
{
	P3M5 = 0xC2;                        //P35设置为推挽输出
	PWM3_MAP = 0x35;					//PWM3映射P35口


	//16MHZ
	//周期计算 	= 0xFF / (Fosc / PWM分频系数)	 PWM3C	
	//			= 0xFF /(16000000 / 128)			
	// 			= 1/255 /125000
	// 			= 490HZ
	pwm_period = (31250/hz)-1;			//31250 = 4000000/128
	PWM3P = pwm_period;					//PWM周期  
//	set_pwm(90);
	PWM3C = 0x97; 						//使能PWM3，关闭中断，允许输出，时钟128分频 111
}

