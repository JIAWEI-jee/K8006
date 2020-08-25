#ifndef __KEY_H__
#define __KEY_H__
#include "HC89F3541.H"

#define KEY_1 	   P0_1
#define KEY_2 	   P2_0
#define KEY_3 	   P0_0
#define EN_BT134   P2_1

#define Insert_Test P0_2

#define KEY_1_PRES 	1	//KEY1按下
#define KEY_2_PRES	2	//KEY2按下
#define KEY_3_PRES	3	//KEY3按下

#define KEY_STATE_0         0       // 按键状态
#define KEY_STATE_1         1
#define KEY_STATE_2         2
#define KEY_STATE_3         3
 
#define LONG_KEY_TIME       200    //长按时间
#define SINGLE_KEY_TIME     3      // 短按时间
 
#define N_KEY         0             // 没有案件按下

#define KEY_1_SHORT  4
#define KEY_1_LONG   5


void key_init(void);
u8 key_scan(void);

#endif

