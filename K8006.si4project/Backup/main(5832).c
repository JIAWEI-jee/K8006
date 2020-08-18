#define	ALLOCATE_EXTERN
#include "HC89F303.h"
#include "intrins.h"//nop
#include "uart.h"
#include "delay.h"
#include "key.h"
#include "flash.h"
#include "timer.h"
#include "pwm.h"
#include "lcd_display.h"
#include "adc.h"
#include "wdt.h"

#define SKU 8014
#define SOFT_VER "1.00.00"

u16 adc_cnt = 0;
u8  first_heat_std = 0,fault_std = 0;

void Set_Temp ( u16 temp );
void Controll_Heat ( u16 temp_set,u16 temp_now );
void Protect ( void );
void Error ( void );

void device_init ( void )
{
	/************************************系统初始化****************************************/
	// CLKSWR = 0x51;						//选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	CLKSWR = 0x53;  //8分频 32/8 = 4M
	CLKDIV = 0x01;						//Fosc 1分频得到Fcpu，Fcpu=16MHz
	P0M2 = 0x10; 						//P02设置为非施密特数字输入
}


static void key_handle ( void )
{
	u8 key_val = 0;
	if ( fault_std == 0 )
	{
		key_val = key_scan();
	}
	else
	{
		key_val = 0;
	}
	if ( key_val == KEY_1_PRES )
	{
		KEY_printf ( " key_scan\r\n" );

		if ( get_device_state() == ON )
		{
			
			set_device_state ( OFF );
			ht1621_all_clear();
			set_pwm ( 0 );
		}
		else
		{
			set_device_state ( ON );
			set_time_sec();
			if ( flash_info.gap > GAP_3 )
			{
				if ( flash_info.timer == TIMER_ON )
				{
					On_stay = 2;
				}
				else if ( flash_info.timer > TIMER_2H )
				{
					Gap_protect_std = 2;
				}
			}
			first_heat_std = 1;
			set_correct_time ( flash_info.gap );
			lcd_display_gap ( flash_info.gap );
			lcd_display_time ( flash_info.timer );
      
		}
	}
	else if ( get_device_state() == ON )
	{



		if ( key_val == KEY_2_PRES ) //档位
		{

			KEY_printf ( " KEY_2_PRES\r\n" );
			if ( flash_info.gap < GAP_9 )
			{
				flash_info.gap++;

			}
			else
			{
				flash_info.gap = GAP_1;
			}
			if ( flash_info.gap > GAP_3 )
			{
				if ( flash_info.timer == TIMER_ON )
				{
					On_stay = 2;
				}
				else if ( flash_info.timer > TIMER_2H )
				{
					Gap_protect_std = 2;
				}
			}

			first_heat_std = 1;
			set_correct_time ( flash_info.gap );
			lcd_display_gap ( flash_info.gap );
			//set_time_sec();
			flah_save_data();
		}
		else if ( key_val == KEY_3_PRES ) //定时
		{

			if ( flash_info.timer < 0x05 )
			{
				flash_info.timer++;
			}
			else
			{
				flash_info.timer = TIMER_ON;
			}
     if ( flash_info.gap > GAP_3 )
			{
				if ( flash_info.timer == TIMER_ON )
				{
					On_stay = 2;
				}
				else if ( flash_info.timer > TIMER_2H )
				{
					Gap_protect_std = 2;
				}
			}
//			KEY_printf ( "timer:%d \r\n", ( u16 ) flash_info.timer );
			lcd_display_time ( flash_info.timer );
			set_time_sec();
			flah_save_data();
		}
	}
}



u16 temp_calc ( u16 uR510,u16 uRw )
{
	u16 i = 0;
	u16 basi_tmp = 20;
	float u1 = 0;
	
	if ( (uR510 < 50 ) || (uR510 > uRw))
	{
		return 0xff;
	}

 
	u1 = (  (( float ) uRw /1000) - (( float ) uR510/1000) ) / (( ( float ) uR510/1000)/510 );
	if (u1 < 0)  return 0xff;
	gm_printf ( "R = %f  \r\n",u1 );
	if (u1 < 450) return 0;
	if ( u1 > Temperature_Value )
	{
		
     i =  u1 - Temperature_Value;
		i= i/2;
		basi_tmp = basi_tmp + i;
		//gm_printf("basi_up20 = %d \r\n",basi_tmp);
	}
	else
	{
		i =Temperature_Value - u1;
		i= i/2;
		if (i > 20) i = 20;
		basi_tmp = basi_tmp - i;
		
		//gm_printf("basi_unnder20 = %d \r\n",basi_tmp);
	}
	 //gm_printf("basi_tmpF:%d \r\n",basi_tmp);
	return ( basi_tmp-Correct_Value );
}



void temperature_handle ( void )
{
	u16 temp = 0;
	u16 adc_val1 = 0,adc_val3 = 0;
	static u8 error_std = 0;
	adc_cnt++;

	if ( adc_cnt > 10000 )
	{
		adc_cnt = 0;
		get_voltage ( &adc_val1,&adc_val3 );

			KEY_printf ( "adv1 = %d adv3 =%d \r\n",adc_val1,adc_val3 );  //pjw set
		temp = temp_calc ( adc_val1, adc_val3 );
			KEY_printf ( "temp val:%d \r\n",temp );

		if ( adc_val1 >50 )
		{
			if ( get_device_state() == ON )
			{
				if ( first_heat_std == 1 )
				{
					first_heat_std = 0;
					if ( temp > 50 )
					{
						Heat_start_std = 2;
						Open_Heat_Value = corrected_value_GAP_9_temp	;
					}
					else
					{
						Heat_start_std = 2;one_heat = 1;
						Open_Heat_Value = corrected_value_warm_temp	;
					}
				}
        
				lcd_display_time ( flash_info.timer );
				lcd_display_gap ( flash_info.gap );
				Set_Temp ( temp );
			}
			else
			{
				set_pwm ( 0 );
				//ht1621_all_clear();
				ht1621_send_cmd ( LCD_OFF );
			}
			fault_std = 0;
		}
		else
		{
			ht1621_all_clear();
			set_pwm ( 0 );
			error_std = Error_STD;
			lcd_display_gap ( error_std );
			fault_std = 1;
		}

	}


}




void main ( void )
{
	device_init();

	uart_init();
	adc_init();
	EA = 1;//开启总中断
	key_init();
	time0_init();
	flash_init();
	pwm_init ( 200 );
	init_lcd_ht1621b();
	delay_ms ( 800 );
	//ht1621_all_clear(); //消除鬼影
	wdt_init ( 2 );
	set_pwm ( 0 );
	gm_printf ( "\r\n==================================\r\n" );
	gm_printf ( "sku:K%d \r\n", ( u16 ) SKU );
	gm_printf ( "soft version:%s \r\n",SOFT_VER );
	gm_printf ( "gap %d \r\n", ( u16 ) flash_info.gap );      //挡位
	gm_printf ( "timer %d \r\n", ( u16 ) flash_info.timer );  // 时间长度
	gm_printf ( "==================================\r\n" );

	while ( 1 )
	{
		key_handle();
		temperature_handle();
	 // Protect();
		//uart_handle();
		clear_wdt();

	}
}


void Controll_Heat ( u16 temp_set,u16 temp_now )
{
//KEY_printf ( "temp_set val:%d \r\n",temp_set );
	if ( temp_now >  temp_set   )
	{
		//	KEY_printf ( "close_heat \r\n");
		
		set_pwm ( 0 ); // 关闭加热丝
	}
	else if ( temp_now < ( temp_set - Open_Heat_Value ) )
	{
		//KEY_printf ( "open_heat \r\n");
		set_pwm ( 10 ); //打开加热丝
	}
}

void Set_Temp ( u16 temp )
{
	if ( one_heat == 1 )
	{
		//KEY_printf ( "oneheat \r\n");
		Controll_Heat ( One_Heat_Temp,temp );
	}
	else
	{
	//	KEY_printf ( "twoheat \r\n");
		switch ( flash_info.gap )
		{
			case GAP_WARM:
				Controll_Heat ( GAP_WARM_temp,temp );
				break;
			case GAP_1:
				Controll_Heat ( GAP_1_temp,temp );
				break;
			case GAP_2:
				Controll_Heat ( GAP_2_temp,temp );
				break;
			case GAP_3:
				Controll_Heat ( GAP_3_temp,temp );
				break;
			case GAP_4:
				Controll_Heat ( GAP_4_temp,temp );
				break;
			case GAP_5:
				Controll_Heat ( GAP_5_temp,temp );
				break;
			case GAP_6:
				Controll_Heat ( GAP_6_temp,temp );
				break;
			case GAP_7:
				Controll_Heat ( GAP_7_temp,temp );
				break;
			case GAP_8:
				Controll_Heat ( GAP_8_temp,temp );
				break;
			case GAP_9:
				Controll_Heat ( GAP_9_temp,temp );
				break;

		}
	}
}




void Protect ( void )
{
	if ( over_rang_time_std == 1 )
	{
		if ( flash_info.gap > GAP_3 )
		{
			flash_info.gap = GAP_3;
			lcd_display_gap ( flash_info.gap );
			flah_save_data();
			over_rang_time_std = 0;
		}
	}
}


