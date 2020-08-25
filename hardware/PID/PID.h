#ifndef _PID_H
#define _PID_H

#include "HC89F3541.H"



typedef struct PID_Value
{
  ui32 liEkVal[3]; //差值保存，给定和反馈的差值
  u8   uEkFlag[3]; //符号，1则对应的为负数，0对应为正数
  u8   uKP_Coe;    //比例系数
  u8   uKI_Coe;    //积分系数
  u8   uKD_Coe;    //微分常数
  u16  iPriVal;    //上一时刻
  u16  iSetVal;    //设定值
  u16  iCurVal;    //实际值
}PID_ValueStr;

extern PID_ValueStr spid;

void PID_Operation ( void );
void PID_Init(void);


#endif
