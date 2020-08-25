#include "pwm.h"
u8 pwm_period = 0;

void set_pwm(u8 duty)
{
	u32 tep = (u32)duty*pwm_period/100;
	PWM3D = tep;
}

void pwm_init(u16 hz)
{
//  P3M5 = 0xC2;                        //P35设置为推挽输出
//  P2M1 = 0xC2;                        //P21设置为推挽输出
//  PWM0_MAP = 0x35;					  //PWM0通道映射P35口
//  PWM01_MAP = 0x21; 				  //PWM01通道映射P21口
//
//  PWM0C = 0x02;						//PWM0高有效，PWM01高有效，时钟32分频 
//  PWMM |= 0x10;						//PWM0工作于独立模式
//
//   PWM0PH = 0x03;						//周期高4位设置为0x03
//   PWM0PL = 0xFF;						//周期低8位设置为0xFF
//
//	PWM0DH = 0x01;						//PWM0高4位占空比0x01
//	PWM0DL = 0x55;						//PWM0低8位占空比0x55
//	PWM0DTH = 0x01;						//PWM01高4位占空比0x01
//	PWM0DTL = 0x55;						//PWM01低8位占空比0x55
//	PWMEN |= 0x11;						//使能PWM1以及PWM01
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

