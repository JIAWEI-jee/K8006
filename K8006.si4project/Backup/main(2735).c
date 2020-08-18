/**
*	************************************************************************************
*	@Examle Version		K8104
*	@Demo	Version		V1.00.00
*	@Date				2020.6??23
*	************************************************************************************
*
**/

#define	ALLOCATE_EXTERN
#include "HC89F3541.H"
#include "delay.h"
#include "adc.h"
#include "flash.h"
#include "key.h"
#include "uart.h"
#include "pwm.h"
#include "wdt.h"
#include "timer.h"
#include "lcddisplay.h"
#include "PID.h"

#define SKU 8104
#define SOFT_VER "1.00.00"


u16 adc_cnt = 0;
u8  first_heat_std = 0;
u8  error_std = 0;

void Clock ( void );
void Set_Temp ( u16 temp );
void Controll_Heat ( u16 temp_set,u16 temp_now );
void Protect ( void );





static void key_handle ( void )
{
	u8 key_val = 0;

	if ( error_std == 0 )

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

			set_pwm ( 0 );
		}
		else
		{
			set_device_state ( ON );
			set_time_sec();
			if ( flash_info.gap > GAP_1 )
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

		}
	}
	else if ( get_device_state() == ON )
	{



		if ( key_val == KEY_2_PRES ) //??
		{

			KEY_printf ( " KEY_2_PRES\r\n" );
			if ( flash_info.gap < GAP_H )
			{
				flash_info.gap++;

			}
			else
			{
				flash_info.gap = GAP_1;
			}
			if ( flash_info.gap > GAP_1 )
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

			lcd_display_gap ( flash_info.gap );
			//set_time_sec();
			flah_save_data();
		}
		else if ( key_val == KEY_3_PRES ) //??
		{

			if ( flash_info.timer < 0x05 )
			{
				flash_info.timer++;
			}
			else
			{
				flash_info.timer = TIMER_ON;
			}

			KEY_printf ( "timer:%d \r\n", ( u16 ) flash_info.timer );
			lcd_display_time ( flash_info.timer );
			set_time_sec();
			flah_save_data();
		}
	}
}



u16 temp_calc ( u16 uRt,u16 uRw )
{
	u8 i = 0;
	u8 basi_tmp = 25;
	u8 k = 10;
	float u1 = 0;
	float u3 = 0;
	if ( uRt < 50 )
	{
		return 0;
	}

	u1 = ( float ) uRt/1000;
	u3 = 	( float ) uRw /1000;
	u1 = (  u3 - u1 ) / u1 * 510;
	//gm_printf ( "R = %f  \r\n",u1 );
	if ( u1 <= 0 )
	{
		return 0;
	}
	
	u1 = u1 / Length;
	 gm_printf("average R = %f \r\n",u1);
	
	
	if ( u1 > 1 )
	{
		while ( 1 )
		{
			u1 = u1 / ( 1 + 0.03 );
			i++;
			if ( u1 <= 1 )
			{
				break;
			}

		}
        //gm_printf("under basi_tmp i = %d \r\n",i);
		basi_tmp = basi_tmp + i;
	}
	else
	{
		while ( 1 )
		{
			u1 = u1 / ( 1 - 0.03 );
			i++;
			if ( u1 >= 1 )
			{
				break;
			}


		}
        // gm_printf("over basi_tmp i = %d \r\n", i );
		basi_tmp = basi_tmp - i;
	}
	 gm_printf("temp:%d \r\n",basi_tmp);
	return ( basi_tmp-Correct_Value );
}




/**
  * @brief  temperature_handle
   
  * @param  temp_set
  * @param  temp_now
  * @retval None
  */
void temperature_handle ( void )
{
	u16 temp = 0;
	u16 adc_val1 = 0,adc_val2 = 0,adc_val3 = 0;

	// if(get_device_state() == ON)
	//{
	adc_cnt++;

	if ( adc_cnt >= 50000 )
	{
		adc_cnt = 0;
		ADC_EN = 0;  //open ADC 
		delay_us ( 800 );
		get_voltage ( &adc_val1,&adc_val2 ,&adc_val3);

		KEY_printf ( "PTC = %d RTD1 =%d RTD2 =%d\r\n",adc_val1,adc_val2,adc_val3 );  //pjw set
		temp = temp_calc ( adc_val3, adc_val2 );
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
						Heat_start_std = 1;
						Open_Heat_Value = corrected_value_GAP_4_temp	;
					}
					else
					{
						Heat_start_std = 1;
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
				lcd_clear_all();
			}
			error_std = 0;
		}
		else
		{
			set_pwm ( 0 );

			lcd_error (  );
            error_std = 1;
		}

    //ADC_EN = 1;  //close ADC 
	}


}








void main()
{
	Clock();
	uart_init();
	adc_init();
	EA = 1;
	time0_init ();						
	key_init();
	flash_init();
	pwm_init ( 200 );
	wdt_init ( 2 );
	LCD_Init();
	lcd_display_On();
	delay_ms ( 1200 );
	lcd_clear_all ();
	gm_printf ( "\r\n==================================\r\n" );
	gm_printf ( "sku:K%d \r\n", ( u16 ) SKU );
	gm_printf ( "soft version:%s \r\n",SOFT_VER );
	gm_printf ( "gap %d \r\n", ( u16 ) flash_info.gap );      
	gm_printf ( "timer %d \r\n", ( u16 ) flash_info.timer );  
	gm_printf ( "==================================\r\n" );

	while ( 1 )
	{


		temperature_handle();
		key_handle ();
		Protect ();
		clear_wdt();

	}
}



void Clock ( void )
{
	/************************************??????????****************************************/
	CLKSWR = 0x51;						//Fosc=16MHz
	CLKDIV = 0x01;						//Fosc/1 = Fcpu=16MHz
}


/**
  * @brief  controll_PWM on/off heat
  * @param  temp_set
  * @param  temp_now
  * @retval None
  */

void Controll_Heat ( u16 temp_set,u16 temp_now )
{

	if ( temp_now >=  temp_set   )
	{
		//KEY_printf ( "close_heat \r\n");
		set_pwm ( 0 ); 
	}
	else if ( temp_now <= ( temp_set - Open_Heat_Value ) )
	{
		//KEY_printf ( "open_heat \r\n");
		set_pwm ( 10 ); 
	}
}
/**
  * @brief  set temperature from now_temper and GAP
  * @param  now_temper
  * @param  null
  * @retval None
  */

void Set_Temp ( u16 temp )
{


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
		case GAP_H:
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

/**
  * @brief  protect -> Over  2H exchange GAP1
  * @param  null
  * @param  null
  * @retval None
  */
void Protect ( void )
{
	if ( over_rang_time_std == 1 )
	{
		if ( flash_info.gap > GAP_1 )
		{
			flash_info.gap = GAP_1;
			//lcd_display_gap ( flash_info.gap );
			flah_save_data();
			over_rang_time_std = 0;
		}
	}
}

