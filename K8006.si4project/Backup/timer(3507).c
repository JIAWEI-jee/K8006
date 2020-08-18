#include "timer.h"
#include "flash.h"
#include "uart.h"
#include "lcd_display.h"

//----------------time---------------
u8 time_cnt = 0;
u16 time_sec = 0;
u16 cnt_heat_time = 0;
u8 over_rang_time_std = 0;
u8 Gap_protect_std = 0 ;
u8 On_stay = 0;
u8  Open_Heat_Value = 0;
u8 Heat_start_std = 0;
u16 time_heat = 0;
u16 temp_time = 0 ;
u16 correct_time = 0;
u8 one_heat = 0;

void set_time_sec_val ( u16 sec )
{
	time_sec = sec;
	time_cnt = 0;
	gm_printf ( "set time second:%d\r\n",sec );
}
void set_correct_time(u8 gap)
{
  
switch ( gap )
	{
		case GAP_WARM:
			correct_time = corrected_time_warm_temp;
			break;
		case GAP_1:
			correct_time = corrected_time_GAP_1_temp;
			break;
		case GAP_2:
			correct_time = corrected_time_GAP_2_temp;
			break;
		case GAP_3:
			correct_time = corrected_time_GAP_3_temp;
			break;
		case GAP_4:
			correct_time = corrected_time_GAP_4_temp;
			break;
		case GAP_5:
			correct_time = corrected_time_GAP_5_temp;
			break;
		case GAP_6:
			correct_time = corrected_time_GAP_6_temp;
			break;
		case GAP_7:
			correct_time = corrected_time_GAP_7_temp;
			break;
		case GAP_8:
			correct_time = corrected_time_GAP_8_temp;
			break;
		case GAP_9:
			correct_time = corrected_time_GAP_9_temp;
			break;

}
	}
 static void set_correct_Value ( void )
{
	switch ( flash_info.gap )
	{
		case GAP_WARM:
			Open_Heat_Value = corrected_value_warm_temp;
			break;
		case GAP_1:
			Open_Heat_Value = corrected_value_GAP_1_temp;
			break;
		case GAP_2:
			Open_Heat_Value = corrected_value_GAP_2_temp;
			break;
		case GAP_3:
			Open_Heat_Value = corrected_value_GAP_3_temp;
			break;
		case GAP_4:
			Open_Heat_Value = corrected_value_GAP_4_temp;
			break;
		case GAP_5:
			Open_Heat_Value = corrected_value_GAP_5_temp;
			break;
		case GAP_6:
			Open_Heat_Value = corrected_value_GAP_6_temp;
			break;
		case GAP_7:
			Open_Heat_Value = corrected_value_GAP_7_temp;
			break;
		case GAP_8:
			Open_Heat_Value = corrected_value_GAP_8_temp;
			break;
		case GAP_9:
			Open_Heat_Value = corrected_value_GAP_9_temp;
			break;


	}

}
static void set_heat_val ( void )
{
	if ( Heat_start_std == 2)
	{

	    
		if ( ++time_heat > correct_time )
		{
			KEY_printf ( "exchange_heat_value \r\n" );
			Heat_start_std = 0;
			time_heat = 0;
			one_heat = 0;

			set_correct_Value (  );
		}
	}
//	if (Heat_start_std == 2)
//		{
//		   one_heat = 1;
//           if ( ++time_heat > Heat_Value)
//           	{
//           	   one_heat = 0;
//              Heat_start_std = 1;
//		    }
//      
//	    }
}

void set_time_sec ( void )
{
	time_cnt = 0;
	if ( flash_info.timer == TIMER_ON )
	{
		time_sec = 0;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_0_5H )
	{
		time_sec = 30*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_1H )
	{
		time_sec = 60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_2H )
	{
		time_sec = 2*60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_4H )
	{
		time_sec = 4*60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	else if ( flash_info.timer == TIMER_8H )
	{
		time_sec = 8*60*60;
		cnt_heat_time = 0;
		temp_time = 0;
	}
	gm_printf ( "time second:%d \r\n",time_sec );
}


void time0_init ( void )
{
	/**********************************TIM0配置初始化**************************************/
	TCON1 = 0x00;						//Tx0定时器时钟为Fosc/12
	TMOD = 0x00;						//16位重装载定时器/计数器
	//定时10ms
	//反推初值 	= 65536 - ((10/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((10/1000) / (1/(16000000 / 12)))
	//			= 65536 - 13333
	//			= 0xCBEC
	// TH0 = 0xCB;
	// TL0 = 0xEB;							//T0定时时间10ms  16MHZ
	TH0 = 0xF2;
	TL0 = 0xFB;							//T0定时时间10ms  4MHZ
	TF0 = 0;//清除中断标志
	ET0 = 1;//打开T0中断
	TR0 = 1;//使能T0
	EA = 1;								//打开总中断
}



//10ms
void TIMER0_Rpt ( void ) interrupt TIMER0_VECTOR
{
	if ( get_device_state() == ON ) //flash_info.timer != TIMER_ON &&
	{
		time_cnt++;
		if ( time_cnt >= 100 )
		{
			set_heat_val();

			if ( On_stay == 2 )  // stay on 模式下进行保护
			{
				cnt_heat_time++;
				if ( cnt_heat_time > overrang_time )
				{
					On_stay = 0;
					cnt_heat_time = 0;
					over_rang_time_std = 1;
					//gm_printf ( "On_stay_overrang \r\n" );
				}

			}
			if ( time_sec > 0 )
			{

				time_sec--;
				if ( Gap_protect_std == 2 )
				{

					cnt_heat_time++;


					if ( cnt_heat_time > overrang_time )
					{
						over_rang_time_std = 1;
						Gap_protect_std = 0;
						cnt_heat_time = 0;
						gm_printf ( "protect \r\n" );
					}
				}
			}
			else
			{
				if ( flash_info.timer != TIMER_ON )
				{
					set_device_state ( OFF );
					time_sec = 0;
					cnt_heat_time = 0;
					Gap_protect_std = 0;
					On_stay = 0;
					ht1621_all_clear(); //定时时间到
					gm_printf ( "time off \r\n" );
				}
			}
			//gm_printf("time_sec=%d \r\n",time_sec);
			time_cnt = 0;
		}
	}
}

void time_Print ( void )
{
	switch ( ++temp_time )
	{
		case H_0_5:
			gm_printf ( "time_to_0.5H  time_sec=%d \r\n",time_sec );
			temp_time = 0;
			break;
		case H_1_0:
			gm_printf ( "time_to_1H  time_sec=%d \r\n",time_sec );
			temp_time = 0;
			break;
		case H_2_0:
			gm_printf ( "time_to_2H  time_sec=%d \r\n",time_sec );
			temp_time = 0;
			break;
		case H_4_0:
			gm_printf ( "time_to_4H  time_sec=%d \r\n",time_sec );
			temp_time = 0;
			break;
		case H_8_0:
			gm_printf ( "time_to_8H  time_sec=%d \r\n",time_sec );
			temp_time = 0;
			break;
	}
}
