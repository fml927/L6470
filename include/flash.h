/**
*******************************************************
* @file    flash.h 
  版本：   v1.2
  功能：  STM32内部FLASH存取头文件
  说明：  支持多个数据块(每个数据块自动增加CRC)，要求每个数据块使用各自独立的页(由offsetaddr指定)

*******************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __user_FLASH_H
#define __user_FLASH_H

/*--------------  用户接口  -----------------------*/
// 说明: STM32F103X内部FLASH以1024字节分页,
//       STM32F103C8T6( 48LQFP) flash:  64kbyte
//       STM32F103RBT6( 64LQFP) flash: 128kbyte
//       STM32F103VET6(100LQFP) flash: 512kbyte
#define	FLASH_DATA_BASE_ADDR	(120 * 1024 + 0x08000000)   //FLASH数据存取起始地址定义 [只能修改第1个数字]
// **** 用户接口函数*******
u8 Flash_Read_Param(u8 offsetaddr, u8 *buf, u16 len);
u8 Flash_Save_Param(u8 offsetaddr, u8 *buf, u16 len);

/*--------------  内部函数声明 -----------------------*/
void Flash_Read_Byte(u32 addr,u16 len,u8 *xp);
void Flash_Read_HalfWord(u32 addr,u16 len,u16 *xp) ;
u8 Flash_Write_HalfWord(u32 addr,u16 len,u16 *xp) ;

u16 Flash_CRC16(u8 *Frame,u8 Length);         //CRC校验

#endif /* __user_FLASH_H */

/*******************  *****END OF FILE****/