#include "pwm.h"
u8 pwm_period = 0;

void set_pwm(u8 duty)
{
	u32 tep = (u32)duty*pwm_period/100;
	PWM3D = tep;
}

void pwm_init(u16 hz)
{
//  P3M5 = 0xC2;                        //P35����Ϊ�������
//  P2M1 = 0xC2;                        //P21����Ϊ�������
//  PWM0_MAP = 0x35;					  //PWM0ͨ��ӳ��P35��
//  PWM01_MAP = 0x21; 				  //PWM01ͨ��ӳ��P21��
//
//  PWM0C = 0x02;						//PWM0����Ч��PWM01����Ч��ʱ��32��Ƶ 
//  PWMM |= 0x10;						//PWM0�����ڶ���ģʽ
//
//   PWM0PH = 0x03;						//���ڸ�4λ����Ϊ0x03
//   PWM0PL = 0xFF;						//���ڵ�8λ����Ϊ0xFF
//
//	PWM0DH = 0x01;						//PWM0��4λռ�ձ�0x01
//	PWM0DL = 0x55;						//PWM0��8λռ�ձ�0x55
//	PWM0DTH = 0x01;						//PWM01��4λռ�ձ�0x01
//	PWM0DTL = 0x55;						//PWM01��8λռ�ձ�0x55
//	PWMEN |= 0x11;						//ʹ��PWM1�Լ�PWM01
	P3M5 = 0xC2;                        //P35����Ϊ�������
	
	PWM3_MAP = 0x35;					//PWM3ӳ��P35��

	//16MHZ
	//���ڼ��� 	= 0xFF / (Fosc / PWM��Ƶϵ��)	 PWM3C	
	//			= 0xFF /(16000000 / 128)			
	// 			= 1/255 /125000
	// 			= 490HZ
	pwm_period = (31250/hz)-1;			//31250 = 4000000/128
	PWM3P = pwm_period;					//PWM����  
//	set_pwm(90);
	PWM3C = 0x97; 						//ʹ��PWM3���ر��жϣ����������ʱ��128��Ƶ 111
}

