/**
*******************************************************
* @file    flash.h 
  �汾��   v1.2
  ���ܣ�  STM32�ڲ�FLASH��ȡͷ�ļ�
  ˵����  ֧�ֶ�����ݿ�(ÿ�����ݿ��Զ�����CRC)��Ҫ��ÿ�����ݿ�ʹ�ø��Զ�����ҳ(��offsetaddrָ��)

*******************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __user_FLASH_H
#define __user_FLASH_H

/*--------------  �û��ӿ�  -----------------------*/
// ˵��: STM32F103X�ڲ�FLASH��1024�ֽڷ�ҳ,
//       STM32F103C8T6( 48LQFP) flash:  64kbyte
//       STM32F103RBT6( 64LQFP) flash: 128kbyte
//       STM32F103VET6(100LQFP) flash: 512kbyte
#define	FLASH_DATA_BASE_ADDR	(120 * 1024 + 0x08000000)   //FLASH���ݴ�ȡ��ʼ��ַ���� [ֻ���޸ĵ�1������]
// **** �û��ӿں���*******
u8 Flash_Read_Param(u8 offsetaddr, u8 *buf, u16 len);
u8 Flash_Save_Param(u8 offsetaddr, u8 *buf, u16 len);

/*--------------  �ڲ��������� -----------------------*/
void Flash_Read_Byte(u32 addr,u16 len,u8 *xp);
void Flash_Read_HalfWord(u32 addr,u16 len,u16 *xp) ;
u8 Flash_Write_HalfWord(u32 addr,u16 len,u16 *xp) ;

u16 Flash_CRC16(u8 *Frame,u8 Length);         //CRCУ��

#endif /* __user_FLASH_H */

/*******************  *****END OF FILE****/