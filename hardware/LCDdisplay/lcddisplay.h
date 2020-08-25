#ifndef  _LCDDISPLAY_H
#define _LCDDISPLAY_H
#include "HC89F3541.H"

#define COM0    P1_2
#define COM1    P1_1
#define COM2    P2_3
#define COM3    P3_4

#define SEG1    P1_7
#define SEG2    P1_6
#define SEG3    P1_5
#define SEG4    P1_4
#define SEG5    P1_3

#define Error      0xff
#define Clear_lcd  0xce
#define all_on     0xcc
typedef enum lcd_std
{
	Frame0_COM0	= 0x00,
	Frame0_COM1 = 0x01,
	Frame0_COM2 = 0x02,
	Frame0_COM3 = 0x03,
	Frame1_COM0	= 0X04,
	Frame1_COM1	= 0X05,
	Frame1_COM2	= 0X06,
	Frame1_COM3	= 0X07,
};
typedef enum lcd_std_SEG
{
	Frame0_SEG1	= 0x00,
	Frame0_SEG2 = 0x01,
	Frame0_SEG3 = 0x02,
	Frame0_SEG4 = 0x03,
	Frame0_SEG5	= 0X04,
	Frame1_SEG1	= 0X05,
	Frame1_SEG2	= 0X06,
	Frame1_SEG3	= 0X07,
	Frame1_SEG4	= 0X08,
	Frame1_SEG5	= 0X09,
};
typedef struct display_set
{
	u8 gap_display;
	u8 time_display;
} display_set;

extern display_set lcd_set;
extern unsigned char LCD_STD;
extern unsigned char guc_DisFreshStep ;

void LCD_Init ( void );
void LCD_Display ( void );
void LCD_Display_seg ( void );
void lcd_display_time ( u8 time );
void lcd_display_gap ( u8 gap );
void lcd_clear_all ( void );
void lcd_error ( void );
void lcd_off(u8 std);
void lcd_display_On(void);
#endif