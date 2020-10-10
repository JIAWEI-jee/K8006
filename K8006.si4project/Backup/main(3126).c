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
#include "math.h"
#include "stdio.h"
#include "string.h"

#define SKU 8006
#define SOFT_VER "1.00.00"

const u16 NTC_ADC_Table[30] =
{
	//160, 148, 136, 124, 110, 102, 94, 86, 78,  70,  // 61℃ ~70℃
	65,  60,  55,  50,   45,  42, 39, 36, 33,  30,  // 71℃ ~80℃
	28,  26,  24,  22,   19,  18, 17, 16, 15,  13,  // 81℃ ~90℃
	12,  11,  10,   9,    8,   7,  6,  5,  4,   3,   // 91℃ ~100℃
};
u16 ADC_val[20] = {0};
u16 ADC_val_PTC[20] = {0};
u16 ADC_val_AC[20] = {0};
u16 ADC_val_insert[20] = {0};

u16 adc_cnt = 0;
u8  first_heat_std = 0;
u8  error_std = 0;
u8  adc_test_std = 1;
u8  Insert_std = 0;
u8  Heat_std = 0;
void Clock ( void );
void Set_Temp ( u16 temp );
void Controll_Heat ( u16 temp_set,u16 temp_now );
u8 look_up_table ( const u16* a,u8 ArrayLong,u16 Data );
//u8 calcuTem(u16 RW,const u16 * tempRes_buf);
void Sort ( u16* arr, int n );




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

u16 temp_calc_PTC ( u16* adc_pdata )
{
	float temp1 = 0;
	float temp2 = 0;
	u16 i = 0;
	u16  bais_temp = 25;

	Sort ( adc_pdata, 20 );

//	for ( i=0; i<20; i++ )
//	{
//		gm_printf ( " %d  ",adc_pdata[i] );
//	}
//	if ( adc_pdata[19] < 10 )
//	{
//		return 0xff;
//	}
//	gm_printf ( "adc_max = %d  \r\n",adc_pdata[19] );
	temp1 = ( float ) adc_pdata[19]/4095;

//	gm_printf ( "u1 = %f  \r\n",temp1 );
	temp1 = temp1*4.2;
//	gm_printf ( "temp1 = %f  \r\n",temp1 );
	temp1 = temp1/1.414;
//	gm_printf ( "AC_real = %f  \r\n",temp1 );
	temp2 = 120 - temp1;
//	gm_printf ( "AC_PTC_real = %f  \r\n",temp2 );

	temp2 =  temp2/ temp1*0.43;
	gm_printf ( "PTC_R = %f  \r\n",temp2 );
	if ( temp2 < 122.93 )
	{
		return 0;
	}

	if ( temp2 >Temperature_Value )
	{
		temp1 = temp2 - Temperature_Value;
		i = ( u16 ) ( temp1 / 0.57 );
		bais_temp = bais_temp + i;
		if ( bais_temp > 100 )
		{
			return 0xff;
		}
	}
	else  if ( temp2 < Temperature_Value )
	{
		temp1 = Temperature_Value - temp2;
		i = ( u16 ) ( temp1 / 0.57 );
		if ( i > 25 )
		{
			i = 25;
		}

		bais_temp = bais_temp - i;
	}
	gm_printf ( "PTC_temper = %d  \r\n", bais_temp );

	return bais_temp;
}

u16 temp_calc ( u16* adc_NTC_pdata )
{
	float u1 = 0;
	float u3 = 0;
	int i = 0;
	u32 adc_max =0;

//	if ( PTC_VAL < 50 )
//	{
//		return 0xff;
//	}
//	gm_printf ( " adc_NTC_pdata:\r\n" );
//	for ( i=0; i<20; i++ )
//	{
//		gm_printf ( " %d  ",adc_NTC_pdata[i] );
//	}
//	gm_printf ( "\r\n" );
	Sort ( adc_NTC_pdata, 20 );
//	gm_printf ( "sort:\r\n" );
//	for ( i=0; i<20; i++ )
//	{
//		gm_printf ( " %d  ",adc_NTC_pdata[i] );
//	}
//	gm_printf ( "\r\n" );
//	gm_printf ( "\r\n");

	//u1 = PTC_adc_val /1000;

//   NTC_adc_val = sqrt ( NTC_adc_val/50 );
//	gm_printf ( "u1 = %d  \r\n",adc_NTC_pdata[19] );


	u1 = ( float ) adc_NTC_pdata[19]/4095;
//	gm_printf ( "u1 = %f  \r\n",u1 );
	u1 = u1*4;
//	gm_printf ( "u1 = %f  \r\n",u1 );
	u1 = u1/1.414;
//	gm_printf ( "u1 = %f  \r\n",u1 );

	u3 = 120.00 - u1;
//	gm_printf ( "u3 = %f  \r\n",u3 );

	u3 = u3/u1*510;
	gm_printf ( "NTC_R = %f  \r\n",u3 );
	adc_max = u3/1000;
	// gm_printf ( "adc_max = %d  \r\n",(u16)adc_max );
	i = look_up_table ( NTC_ADC_Table,30, ( u16 ) adc_max );
	gm_printf ( "NTC_temp = %d  \r\n",i );
	return ( i );
}

void  temp_calc_AC_Insert ( u16* pdata_ac,u16* pdata_insert )
{
	float u1 = 0;
	float u3 = 0;
	int i = 0;
	static u32 adc_max =0;

	Sort ( pdata_ac, 20 );

//	gm_printf ( "AC:\r\n");
//	for ( i=0; i<20; i++ )
//	{
//		gm_printf ( " %d  ",pdata_ac[i] );
//	}
//	gm_printf ( "\r\n" );
	gm_printf ( "pdata_ac_max = %d\r\n",pdata_ac[19]);
   for(i = 0;i < 10 ;i++)
   	{
   	adc_max = adc_max + pdata_ac[19];
   	}
   gm_printf ( "pdata_ac_max = %d \r\n",adc_max); 
   u1 = (float)(adc_max / 10);
   gm_printf ( "u1 = %f \r\n",u1); 
   
   

//	gm_printf ( "\r\n");
//	gm_printf ( "\r\n");
//
//	gm_printf ( "Insert:\r\n");
	Sort ( pdata_insert, 20 );
//	for ( i=0; i<20; i++ )
//	{
//		gm_printf ( " %d  ",pdata_insert[i] );
//	}
//	gm_printf ( "\r\n" );
	gm_printf ( "pdata_insert_max = %d\r\n",pdata_insert[19] );
//	gm_printf ( "\r\n" );
//	gm_printf ( "\r\n" );
	if ( pdata_insert[19] > 150 )
	{
		Insert_std = 1;
		KEY_printf ( " Insert_blanket \r\n" );
	}
	else
	{
		Insert_std = 0;
		KEY_printf ( " Uninsert_blanket \r\n" );
	}

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
	static u16 adc_val1 = 0,adc_val3 = 0;
	static u16 adc_val_input = 0,adc_val_insert = 0;
	static u16 adc_test = 0;
	static u8 adc_times = 0;
	adc_cnt++;
	if ( adc_test_std == 1 )
	{
		set_pwm ( 10 );

		if ( ++adc_test >= 50 )
		{



			adc_test = 0;
			get_ADC_val ( &adc_val1, &adc_val3 );
			calc_InputAndInsert_val ( &adc_val_input, &adc_val_insert );
			ADC_val[adc_times] = adc_val3;
			ADC_val_PTC[adc_times] = adc_val1;
			ADC_val_AC[adc_times] = adc_val_input;
			ADC_val_insert[adc_times] = adc_val_insert;
			adc_times++;
			//		PTC_adc_val = PTC_adc_val + adc_val1;

			//	NTC_adc_val = NTC_adc_val + ( adc_val3*adc_val1 );

		}
		if ( adc_times > 20 )
		{
			adc_times = 0;
			adc_test_std = 2;
			if ( Heat_std == open )
			{
				set_pwm ( 10 );
			}
			else
			{
				set_pwm ( 0 );
			}
		}

	}
	if ( adc_cnt >= 50000 )
	{
		adc_cnt = 0;

		if ( adc_test_std == 2 )
		{
//
//           if (Heat_std == open)
//           	{
//             set_pwm ( 10 );
//		    }
//		   else
//		   	{
//             set_pwm ( 0 );
//		    }
//			gm_printf ( " adc_test_std = 2\r\n" );
//			gm_printf ( "\r\n" );
			temp_calc ( ADC_val );
			temp = temp_calc_PTC ( ADC_val_PTC );
			temp_calc_AC_Insert ( ADC_val_AC,ADC_val_insert );
			//	KEY_printf ( "PTC = %d  NTC =%d\r\n",PTC_adc_val,NTC_adc_val );  //pjw set

			adc_test_std = 1;

			//	KEY_printf ( "temp val:%d \r\n",temp );
		}
		if ( Insert_std == 1 )  //adc_val1 >50
		{
			if ( get_device_state() == ON ) //get_device_state() == OFF
			{
				if ( first_heat_std == 1 )
				{
					first_heat_std = 0;
					if ( temp > 50 )
					{
						Heat_start_std = 1;
						Open_Heat_Value = corrected_value_GAP_4_temp;
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
				//set_pwm ( 10 );

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

//    ADC_EN = 1;  //close ADC
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
		Heat_std = CLOSE;
		set_pwm ( 0 );
	}
	else if ( temp_now <= ( temp_set - Open_Heat_Value ) )
	{
		//KEY_printf ( "open_heat \r\n");
		Heat_std = open;
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
//void Protect ( void )
//{
//	if ( over_rang_time_std == 1 )
//	{
//		if ( flash_info.gap > GAP_1 )
//		{
//			flash_info.gap = GAP_1;
//
//			flah_save_data();
//			over_rang_time_std = 0;
//		}
//	}
//}

u8 look_up_table ( const u16* a,u8 ArrayLong,u16 Data )
{
	u16 begin,end,middle ;
	u8 i ;
	begin = 0 ;
	end = ArrayLong-1 ;
	i = 0  ;
	if ( Data >= a[begin] )
	{

		return begin ;
	}
	else if ( Data <= a[end] )
	{
		return 100 ;
	}
	while ( begin < end )
	{
		middle = ( begin+end ) /2 ;
		if ( Data == a[middle] )
		{
			break ;
		}
		if ( Data < a[middle] && Data > a[middle+1] )
		{
			break ;
		}
		if ( Data > a[middle] )
		{
			end = middle ;
		}
		else
		{
			begin = middle ;
		}
		if ( i++ > ArrayLong )
		{
			break ;
		}
	}
	if ( begin > end )
	{
		return 0 ;
	}
	return middle+70 ;
}

//u8 calcuTem(u16 RW,const u16 * tempRes_buf)
//{
//  u8 tempValue= 108;
//	u8 i = 0;
//  if ((RW < 4000)&&(RW > 6))
//  {
//    for ( i = 0 ; i < 181 ; i++)
//    {
//      if (RW >= tempRes_buf[i])
//      {
//        tempValue = i+20;
//        break;
//      }
//    }
//  }
//
//  return tempValue;
//}
void Sort ( u16* arr, int n )
{
	int m, i, j;
	for ( i = 0; i < n - 1; i++ )
		for ( j = 0; j < n - 1 - i; j++ )
			if ( arr[j] > arr[j + 1] )
			{
				m = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = m;
			}
}




