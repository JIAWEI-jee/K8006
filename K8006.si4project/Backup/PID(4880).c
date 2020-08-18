#include"PID.h"
PID_ValueStr spid; //定义一个结构体 存储PID各种算法
u8 PID_RunFlag = 0;
/***************************************************/
/*
函数名称；PID_Operation()
函数功能：PID运算
入口参数：null
出口参数：null
函数说明；U(k)+KP*[E(K)- E(K-1)]+KI*E(k)+KD*[E(k)-2E(k-1)+E(k-2)]
*/
/********************************************************/
void PID_Operation ( void )
{
	u32 temp[3]= {0}; //存储中间临时变量
	u32 PostSum = 0; //正数和
	u32 NegSum = 0;  //负数和
	if ( spid.iSetVal > spid.iCurVal )
	{
		if ( spid.iSetVal - spid.iCurVal > 10 )
		{
			spid.iPriVal = 100;//偏差大于10为上限幅值(全速加热)
		}
		else
		{
			temp[0]=spid.iSetVal - spid.iCurVal;//计算偏差值
			spid.uEkFlag[1] = 0;//E(k)为正数，因为设定值大于实际值
			/*数值进行移位，注意顺序，否则会覆盖掉前面数值*/
			spid.liEkVal[2] = spid.liEkVal[1];
			spid.liEkVal[1] = spid.liEkVal[0];
			spid.liEkVal[0] = temp[0];
			/*=================================================================*/
			if ( spid.liEkVal[0] > spid.liEkVal[1] )
			{
				temp[0]=spid.liEkVal[0] - spid.liEkVal[1]; //E(k)>E(k-1)
				spid.uEkFlag[0] = 0; //E(k)-E(k-1)为正数
			}
			else
			{
				temp[0] = spid.liEkVal[1] - spid.liEkVal[0];
				spid.uEkFlag[0] = 1;
			}
			/*==================================================================*/
			temp[2]=spid.liEkVal[1]*2;
			if ( ( spid.liEkVal[0]+spid.liEkVal[2] ) >temp[2] ) //E(k-2)+E(k)>2E(k-1)
			{
				temp[2] = ( spid.liEkVal[0] + spid.liEkVal[2] )- temp[2];
				spid.uEkFlag[2] = 0;  //E(k-2)+E(k)-2E(K-1)为正数
			}
			else                        //E(k-2)+E(k)<2E(k-1)
			{
				temp[2]= temp[2] - ( spid.liEkVal[0]+spid.liEkVal[2] );
				spid.uEkFlag[2]=1;     //E(k-2)+E(k)-2E(K-1)为负数
			}
			/*==================================================================*/
			temp[0] = ( u32 ) spid.uKP_Coe*temp[0]; //KP*[E(k)-E(k-1)]
			temp[1] = ( u32 ) spid.uKI_Coe*spid.liEkVal[0]; //KI*E(k)
			temp[2] = ( u32 ) spid.uKD_Coe*temp[2]; //KD*[E(k-1)+E(k)-2E(k-1)]
			/*一下部分代码是讲所有正数项叠加，负数项叠加*/
			/*======================计算KP[E(k)-E(k-1)]=================*/
			if ( spid.uEkFlag[0]==0 )
			{
				PostSum +=temp[0];   //正数和
			}
			else
			{
				NegSum +=temp[0];   //负数和
			}
			/*=======================计算KI*E(k)===================================*/
			if ( spid.uEkFlag[1]==0 )
			{
				PostSum +=temp[0];   //正数和
			}
			//是在spid.iSetVal > spid.iCurVal情况下不会为负数
			/*====================计算KD*[E(k-2)+E(k)-2E(k-1)]的值===============*/
			if ( spid.uEkFlag[2]==0 )
			{
				PostSum+=temp[2];   //正数和
			}
			else
			{
				NegSum+=temp[2];   //负数和
			}
			/*=====================计算U(k)的值=================================*/
			PostSum += ( u32 ) spid.iPriVal;
			if ( PostSum>NegSum ) //控制量是否为正数
			{
				temp[0]=PostSum-NegSum;
				if ( temp[0]<100 )
				{
					spid.iPriVal = ( u16 ) temp[0];   //小于上限幅值为计算值输出
				}
				else
				{
					spid.iPriVal = 100;    //大于上限幅值为上限值输出
				}
			}
			else //控制量为负数，则控制输出为0
			{
				spid.iPriVal =0;
			}
		}

	}
	else
	{
		spid.iPriVal = 0;    //spid.iSetVal < spid.iCurVal 控制输出值直接为0 停止加热
	}
}


