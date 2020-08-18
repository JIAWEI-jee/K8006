#include "lcddisplay.h"
#include "flash.h"

display_set lcd_set = {0,0};

unsigned char guc_DisFreshStep = 0;
void set_gap ( u8 gap );
void set_time ( u8 temp );

void LCD_Init ( void )
{
	/***********************************LCD配置初始化**************************************/
	LCDCON |= 0xE8;                     //使能LCD，50K电阻，1/3bias
	COMP1EN |= 0x06;                    //COM使能P11 P12
	COMP2EN |= 0x08;                    //COM使能P23
	COMP3EN |= 0x10;                    //COM使能P34

	SEGP1EN |= 0xF8;                    //SEG口使能P1口，P17 P16 P15 P14 P13 为SEG1 SEG2 SEG3 SEG4 SEG5

	SEG1 = 0;
	SEG2 = 0;
	SEG3 = 0;
	SEG4 = 0;
	SEG5 = 0;
}

void lcd_clear_all ( void )
{
	lcd_set.gap_display = Clear_lcd;
	lcd_set.time_display = Clear_lcd;
}
void lcd_error ( void )
{
	static u8 error_std = 0;
	if ( error_std == 0 )
	{
		lcd_set.gap_display = Clear_lcd;
		lcd_set.time_display = Clear_lcd;
		error_std = 1;
	}
	else if ( error_std == 1 )
	{
		lcd_set.gap_display = Error;
		error_std = 0;
	}

}
void lcd_display_time ( u8 time )
{
	lcd_set.time_display = time;
}

void lcd_display_gap ( u8 gap )
{

	lcd_set.gap_display = gap;

}
void lcd_display_On ( void )
{

	lcd_set.gap_display = all_on;
	lcd_set.time_display = all_on;
}
void LCD_Display ( void )
{

	switch ( guc_DisFreshStep )
	{
		case Frame0_COM0:
			LCDCON &=~ 0x10;   //FRAME 0

			COM0 = 1;
			COM1 = 0;
			COM2 = 0;
			COM3 = 0;

			set_time ( lcd_set.time_display );
			set_gap ( lcd_set.gap_display );
			guc_DisFreshStep = Frame0_COM1;
			break;
		case Frame0_COM1:
			COM0 = 0;
			COM1 = 1;
			COM2 = 0;
			COM3 = 0;
			set_time ( lcd_set.time_display );
			set_gap ( lcd_set.gap_display );
			guc_DisFreshStep = Frame0_COM2;
			break;
		case Frame0_COM2:

			COM0 = 0;
			COM1 = 0;
			COM2 = 1;
			COM3 = 0;

			set_time ( lcd_set.time_display );
			set_gap ( lcd_set.gap_display );
			guc_DisFreshStep = Frame0_COM3;
			break;
		case Frame0_COM3:

			COM0 = 0;
			COM1 = 0;
			COM2 = 0;
			COM3 = 1;
			set_time ( lcd_set.time_display );

			set_gap ( lcd_set.gap_display );
			guc_DisFreshStep = Frame1_COM0;
			break;
		case Frame1_COM0:
			LCDCON |= 0x10;   //FRAME 1

			COM0 = 1;
			COM1 = 0;
			COM2 = 0;
			COM3 = 0;


			set_time ( lcd_set.time_display );

			set_gap ( lcd_set.gap_display );

			guc_DisFreshStep = Frame1_COM1;
			break;
		case Frame1_COM1:

			COM0 = 0;
			COM1 = 1;
			COM2 = 0;
			COM3 = 0;
			set_time ( lcd_set.time_display );

			set_gap ( lcd_set.gap_display );
			guc_DisFreshStep = Frame1_COM2;
			break;
		case Frame1_COM2:

			COM0 = 0;
			COM1 = 0;
			COM2 = 1;
			COM3 = 0;
			set_time ( lcd_set.time_display );
			set_gap ( lcd_set.gap_display );
			guc_DisFreshStep = Frame1_COM3;
			break;
		case Frame1_COM3:

			COM0 = 0;
			COM1 = 0;
			COM2 = 0;
			COM3 = 1;


			set_time ( lcd_set.time_display );

			set_gap ( lcd_set.gap_display );

			guc_DisFreshStep = Frame0_COM0;
			break;
		default:
			guc_DisFreshStep = Frame0_COM0;
			break;
	}
}

//void LCD_Display_seg ( void )
//{
//
//	switch ( guc_DisFreshStep )
//	{
//		case Frame0_SEG1:
//			LCDCON &=~ 0x10;   //FRAME 0
//
//			SEG1 = 0;
//			SEG2 = 0;
//			SEG3 = 0;
//			SEG4 = 0;
//			SEG5 = 1;
//
//			COM0 = 0;
//			COM1 = 0;
//			COM2 = 1;
//			COM3 = 0;
//
//
//			guc_DisFreshStep = Frame0_SEG2;
//			break;
//		case Frame0_SEG2:
//			SEG1 = 0;
//			SEG2 = 0;
//			SEG3 = 0;
//			SEG4 = 1;
//			SEG5 = 0;
//
//			COM0 = 0;
//			COM1 = 1;
//			COM2 = 1;
//			COM3 = 1;
//			guc_DisFreshStep = Frame0_SEG3;
//			break;
//		case Frame0_SEG3:
//			SEG1 = 0;
//			SEG2 = 0;
//			SEG3 = 1;
//			SEG4 = 0;
//			SEG5 = 0;
//
//			COM0 = 1;
//			COM1 = 1;
//			COM2 = 1;
//			COM3 = 1;
//
//			guc_DisFreshStep = Frame0_SEG4;
//			break;
//		case Frame0_SEG4:
//			SEG1 = 0;
//			SEG2 = 1;
//			SEG3 = 0;
//			SEG4 = 0;
//			SEG5 = 0;
//
//
//			COM0 = 0;
//			COM1 = 0;
//			COM2 = 0;
//			COM3 = 0;
//			guc_DisFreshStep = Frame0_SEG5;
//			break;
//		case Frame0_SEG5:
//			SEG1 = 1;
//			SEG2 = 0;
//			SEG3 = 0;
//			SEG4 = 0;
//			SEG5 = 0;
//
//
//			COM0 = 0;
//			COM1 = 0;
//			COM2 = 0;
//			COM3 = 0;
//			guc_DisFreshStep = Frame1_SEG1;
//			break;
//		case Frame1_SEG1:
//			LCDCON |= 0x10;   //FRAME 1
//			SEG1 = 0;
//			SEG2 = 0;
//			SEG3 = 0;
//			SEG4 = 0;
//			SEG5 = 1;
//
//
//
//
//
//			COM0 = 0;
//			COM1 = 0;
//			COM2 = 0;
//			COM3 = 0;
//
//			guc_DisFreshStep = Frame1_SEG2;
//			break;
//		case Frame1_SEG2:
//			SEG1 = 0;
//			SEG2 = 0;
//			SEG3 = 0;
//			SEG4 = 1;
//			SEG5 = 0;
//
//
//			COM0 = 0;
//			COM1 = 1;
//			COM2 = 1;
//			COM3 = 1;
//
//			guc_DisFreshStep = Frame1_SEG3;
//			break;
//		case Frame1_SEG3:
//			SEG1 = 0;
//			SEG2 = 0;
//			SEG3 = 1;
//			SEG4 = 0;
//			SEG5 = 0;
//
//			COM0 = 1;
//			COM1 = 1;
//			COM2 = 1;
//			COM3 = 1;
//
//
//			guc_DisFreshStep = Frame1_SEG4;
//			break;
//		case Frame1_SEG4:
//			SEG1 = 0;
//			SEG2 = 1;
//			SEG3 = 0;
//			SEG4 = 0;
//			SEG5 = 0;
//
//
//
//
//			COM0 = 0;
//			COM1 = 0;
//			COM2 = 0;
//			COM3 = 0;
//
//
//			guc_DisFreshStep = Frame1_SEG5;
//			break;
//		case Frame1_SEG5:
//			SEG1 = 1;
//			SEG2 = 0;
//			SEG3 = 0;
//			SEG4 = 0;
//			SEG5 = 0;
//
//
//
//			COM0 = 0;
//			COM1 = 0;
//			COM2 = 0;
//			COM3 = 0;
//
//
//
//
//			guc_DisFreshStep = Frame0_SEG1;
//			break;
//		default:
//			guc_DisFreshStep = Frame0_SEG1;
//			break;
//	}
//}


//void set_time(void)
//{
//


//}
void set_gap ( u8 number )
{
	switch ( number )
	{
		case GAP_H:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{

				SEG3 = 0;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG3 = 1;
				SEG4 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG3 = 1;
				SEG4 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG3 = 0;
				SEG4 = 1;
			}

			break;
		case GAP_1:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{

				SEG3 = 0;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG3 = 0;
				SEG4 = 0;
			}
			break;

		case GAP_2:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{

				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG3 = 0;
				SEG4 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG3 = 1;
				SEG4 = 1;
			}
			break;

		case GAP_3:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{

				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG3 = 1;
				SEG4 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG3 = 1;
				SEG4 = 0;
			}
			break;
		case Error:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{

				SEG3 = 1;
				SEG4 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG3 = 0;
				SEG4 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG3 = 0;
				SEG4 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG3 = 0;
				SEG4 = 1;
			}
			break;
		case Clear_lcd:
			SEG1 = 0;
			SEG2 = 0;
			SEG3 = 0;
			SEG4 = 0;
			SEG5 = 0;
			break;
		case all_on:
			SEG1 = 1;
			SEG2 = 1;
			SEG3 = 1;
			SEG4 = 1;
			SEG5 = 1;
			break;
	}

}

void set_time ( u8 number )
{
	switch ( number )
	{
		case TIMER_ON:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG1 = 0;
				SEG2 = 1;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}

			break;
		case TIMER_0_5H:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{
				SEG1 = 1;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			break;

		case TIMER_1H:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG1 = 1;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			break;

		case TIMER_2H:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			break;
		case TIMER_4H:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{
				SEG1 = 0;
				SEG2 = 1;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			break;
		case TIMER_8H:
			if ( ( guc_DisFreshStep ==  Frame0_COM0 ) || ( guc_DisFreshStep == Frame1_COM0 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 0;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM1 ) || ( guc_DisFreshStep == Frame1_COM1 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM2 ) || ( guc_DisFreshStep == Frame1_COM2 ) )
			{
				SEG1 = 0;
				SEG2 = 0;
				SEG5 = 1;
			}
			if ( ( guc_DisFreshStep ==  Frame0_COM3 ) || ( guc_DisFreshStep == Frame1_COM3 ) )
			{
				SEG1 = 1;
				SEG2 = 1;
				SEG5 = 1;
			}
			break;
	}

}


