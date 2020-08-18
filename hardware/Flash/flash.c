#include "flash.h"
#include "uart.h"
//#include "timer.h"

flash_info_t flash_info;
u8 flash_check[4] = {0x20,0x20,0x05,0x01};
u8 device_state = OFF;

u8 get_device_state(void)
{
	return device_state;
}

void set_device_state(u8 state)
{
	device_state = state;
}


/**
  * @说明  	扇区擦除，约消耗5ms的时间
  * @参数  	fui_Address ：被擦除的扇区内的任意一个地址
  * @返回值 无
  * @注		只要操作扇区里面的任意一个地址，就可以擦除此扇区
  */
#pragma disable						//确保擦除时不会进中断导致擦除失败
void Flash_EraseBlock(u16 addr)
{
	IAP_CMD = 0xF00F;				//Flash解锁
	IAP_ADDR = addr;				//写入擦除地址
	IAP_CMD = 0xD22D;				//选择操作方式， 扇区擦除
	IAP_CMD = 0xE11E; 				//触发后IAP_ADDRL&IAP_ADDRH指向0xFF，同时自动锁定
}

/**
  * @说明  	写入一个字节数据到Flash里面
  * @参数  	fui_Address ：FLASH地址
  *	@参数	fucp_SaveData：写入的数据
  * @返回值 无
  * @注		写之前必须先对操作的扇区进行擦除
  */
#pragma disable						//确保写入时不会进中断导致写入失败
void FLASH_WriteData(u8 fuc_SaveData, u16 addr)
{
	IAP_DATA = fuc_SaveData;
	IAP_CMD=0xF00F;					//Flash解锁
	IAP_ADDR = addr;
	IAP_CMD=0xB44B;					//字节编程
	IAP_CMD=0xE11E;					//触发一次操作
}

/**
  * @说明  	写入任意长度的数据到FLASH里面
  * @参数  	fui_Address ：FLASH起始地址
  *	@参数	len ： 写入数据长度
  *			取值范围：0x00-0xFF
  *	@参数	*fucp_SaveArr：写入的数据存放区域的首地址
  * @返回值 无
  * @注		写之前必须先对操作的扇区进行擦除
  */
#pragma disable						//确保写入时不会进中断导致写入失败
void Flash_WriteArr(u16 fui_Address,u8 len,u8 *fucp_SaveArr)
{
	u8 i = 0;

  EA = 0;//关闭中断
	for(i = 0; i < len; i++)
	{
		FLASH_WriteData(*(fucp_SaveArr++), fui_Address++); 
	}
  EA = 1;//开启中断
}

u8 flash_read_addr(u16 address)
{
	u8 val = 0;
	val = *((unsigned char code *)(address));
	return val;
}


/**
  * @说明  	从FLASH里面读取任意长度的数据
  * @参数  	fui_Address ：FLASH起始地址
  *	@参数	fuc_Length ：读取数据长度
  *			取值范围：0x00-0xFF
  *	@参数	*fucp_SaveArr：读取数据存放的区域首地址
  * @返回值 无
  * @注		无
  */
void Flash_ReadArr(u16 fui_Address,u8 fuc_Length,u8 *fucp_SaveArr)
{
	while(fuc_Length--)
	*(fucp_SaveArr++)=*((unsigned char code *)(fui_Address++));//读取数据
}

static u8 check_flash(void)
{
  u8 i = 0,cnt = 0;

  for (i = 0; i < 4; ++i)
  {
    if(flash_read_addr(FLASH_CHECK_ADDRESS+i) == flash_check[i])
    {
      cnt ++;
    }else
      return 0;
  }
  if(cnt >= 4)
    return 1;
  return 0;
}


void flash_init(void)
{
    u8 init_flag = 0;

    // FREQ_CLK = 0x10;          //指明当前系统时钟为16MHz
    FREQ_CLK = 0x04;//4MHZ
    //读出数据
    if(check_flash() == 0)//第一次数据初始化
    {
        init_flag = 1;
    }
    else
    {
        gm_printf("\r\n========================================\r\n");
        Flash_ReadArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
        gm_printf("power on flash gap:%d  timer:%d \r\n",(u16)flash_info.gap,(u16)flash_info.timer);
        gm_printf("========================================\r\n");
        if(flash_info.gap > GAP_9 || flash_info.timer > TIMER_8H)//读出数据不合法
            init_flag = 1;
    }
    if(init_flag == 1)
    {
        gm_printf("=============first init data=========== \r\n");
        flash_info.gap    = GAP_1;    //default 1 gap 
        flash_info.timer  = TIMER_0_5H; //default 0.5H
        Flash_EraseBlock(FLASH_CHECK_ADDRESS);//擦除地址 FLASH_CHECK_ADDRESS 所在扇区
        Flash_WriteArr(FLASH_CHECK_ADDRESS,4,flash_check);
        Flash_WriteArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
        Flash_ReadArr(FLASH_CHECK_ADDRESS,4,flash_check);
        gm_printf("0x%04X  0x%04X  0x%04X  0x%04X \r\n",(u16)flash_check[0],(u16)flash_check[1],(u16)flash_check[2],(u16)flash_check[3]);
        gm_printf("========================================\r\n");
    }
    gm_printf("flash gap:%d  timer:%d \r\n",(u16)flash_info.gap,(u16)flash_info.timer);
}

//flash 需要擦除之后才能写入 这样写入的值才是正确的
void flah_save_data(void)
{
  // gm_printf("========================================\r\n");
  Flash_EraseBlock(FLASH_DATA_ADDRESS);
  Flash_WriteArr(FLASH_CHECK_ADDRESS,4,flash_check);
  Flash_WriteArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
  Flash_ReadArr(FLASH_DATA_ADDRESS,sizeof(flash_info),(u8 *)(&flash_info));
  // gm_printf("read flash gap:%d  timer:%d \r\n",(u16)flash_info.gap,(u16)flash_info.timer);
  // gm_printf("========================================\r\n");
}


