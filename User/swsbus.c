/************************ (C) COPYRIGHT 2008 SWS *******************************
* File Name          : swsbus.c
* Author             : 童锦
* Version            : V2.0
* Date               : 2012.11.30
* Description        : SWSBUS协议程序

V1.1:修改RD0-RD7读命令过程中数据传递问题
     修改添加CRC码时进行两次CRC检验问题
     修改485使能延时过长问题
V1.2:删除4组写命令和读命令
     将帧与帧之间延时改为0.5ms
     CRC校验错误后不返回信息
V2.0:增加4组写命令和读命令
     修改串口配置函数适应STM32的V3.5.0库文件
     修改头文件，所以硬件配置端口均在头文件里设置
********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "swsbus.h"
#include "main.h"
#include "string.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8  TxBuffer[TxBufferSize];    //发送数据缓存
u8  RxBuffer[RxBufferSize];    //接受数据缓存
u8  TxBufferLength;            //发送数据长度
u8  RxBufferLength;            //接受数据长度
struct COM_Array BUS_Buffer;
/*******************************************************************************
** 函数名称 ：RS485_Init
** 函数功能 ：RS485端口及串口配置
** 入口参数 ：无
** 出口参数 ：无
*******************************************************************************/
void RS485_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RS485_TIM_RCC, ENABLE);    //TIM6-判断485数据的帧
  RCC_APB2PeriphClockCmd(RS485_USART_RCC, ENABLE);
  
/************************  RS485端口及串口配置   ******************************/  
  /* 配置485通信TX端口 */
  GPIO_InitStructure.GPIO_Pin = USART_TX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
  /* 配置485通信RX端口 */
  GPIO_InitStructure.GPIO_Pin = USART_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
  /* 配置485通信发送接收使能端口*/
  GPIO_InitStructure.GPIO_Pin = USART_RD;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
  
  /* 配置485通信串口*/
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(RS485_USART, &USART_InitStructure);
  USART_ITConfig(RS485_USART, USART_IT_RXNE, ENABLE);
  USART_Cmd(RS485_USART, ENABLE);
  RS485_RE;      //485接收使能
  
/************************  RS485通信定时器配置   ******************************/ 
  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(RS485_TIM, &TIM_TimeBaseStructure);
  

}

/*******************************************************************************
** 函数名称 ：BUS_DataResponse
** 函数功能 ：串口通信中接受到一帧数据后判断是什么命令并作出相应的响应
** 入口参数 ：无
** 出口参数 ：无
*******************************************************************************/
void BUS_DataResponse(void)
{
  int i,CRC_CODE=0;
  u16 CRC_Result;                //CRC校验结果
  CRC_Result = CRC16(RxBuffer,RxBufferLength);
  if(CRC_Result == 0)   //CRC校验正确结果为0
  {
    LED_485_ON;        //接收正确一帧数据
     
    switch(RxBuffer[1]) //根据功能码执行相应功能
    {
    case BUS_INI:       //初始化
      break;

    case BUS_ASK:                      //握手命令
      TxBuffer[0] = LocalAddress;      //本机地址
      TxBuffer[1] = BUS_ASK;           //功能码
     CRC_CODE = CRC16(TxBuffer,2);    //计算CRC码
      TxBuffer[2] = (CRC_CODE & 0x00FF);        //CRC校验码的低字节
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF); //CRC校验码的高字节
      RS485_SendData(MIN_BufferSize);           //向上位机发送响应数据
      break;

    case BUS_WR0:                      //向丛机写数据0
      BUS_Buffer.WR0[0] = 1;           //表示有新的数据更新
      for(i=2;i<RxBufferLength-2;i++)  //去除地址，功能和CRC码
      {
        BUS_Buffer.WR0[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;      //本机地址
      TxBuffer[1] = BUS_WR0;           //功能码
      CRC_CODE = CRC16(TxBuffer,2);    //计算CRC码
      TxBuffer[2] = (CRC_CODE & 0x00FF);        //CRC校验码的低字节
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF); //CRC校验码的高字节
      RS485_SendData(MIN_BufferSize);           //向上位机发送响应数据
      break;

   case BUS_WR1:
      BUS_Buffer.WR1[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR1[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR1;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR2:
      BUS_Buffer.WR2[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR2[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR2;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR3:
      BUS_Buffer.WR3[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR3[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR3;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;
      
    case BUS_WR4:
      BUS_Buffer.WR4[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR4[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR4;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR5:
      BUS_Buffer.WR5[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR5[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR5;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;
      
    case BUS_WR6:
      BUS_Buffer.WR6[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR6[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR6;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR7:
      BUS_Buffer.WR7[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR7[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR7;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;   
      
    case BUS_WR8:
      BUS_Buffer.WR8[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR8[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR8;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;      
      
    case BUS_WR9:
      BUS_Buffer.WR9[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR9[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR9;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;      

    case BUS_RD0:                        //从丛机读数据
      TxBuffer[0] = LocalAddress;        //本机地址
      TxBuffer[1] = BUS_RD0;             //功能码
      for(i=2;i<RD0_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD0[i-2];    //将丛机数据传送给串口发送缓存区
      }
      CRC_CODE = CRC16(TxBuffer,RD0_BufferSize + 2);//计算CRC码
      TxBuffer[i] = (CRC_CODE & 0x00FF);         //CRC校验码的低字节
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);//CRC校验码的高字节
      RS485_SendData(RD0_BufferSize + 4);       //向上位机发送响应数据
      break;

    case BUS_RD1:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD1;
      for(i=2;i<RD1_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD1[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD1_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD1_BufferSize + 4);
      break;

    case BUS_RD2:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD2;
      for(i=2;i<RD2_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD2[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD2_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD2_BufferSize + 4);
      break;

    case BUS_RD3:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD3;
      for(i=2;i<RD3_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD3[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD3_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD3_BufferSize + 4);
      break;
      
    case BUS_RD4:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD4;
      for(i=2;i<RD4_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD4[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD4_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD4_BufferSize + 4);
      break;   
      
    case BUS_RD5:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD5;
      for(i=2;i<RD5_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD5[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD5_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD5_BufferSize + 4);
      break; 

    case BUS_RD6:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD6;
      for(i=2;i<RD6_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD6[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD6_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD6_BufferSize + 4);
      break;

    case BUS_RD7:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD7;
      for(i=2;i<RD7_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD7[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD7_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD7_BufferSize + 4);
      break;   
      
    case BUS_RD8:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD8;
      for(i=2;i<RD8_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD8[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD8_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD8_BufferSize + 4);
      break;
      
    case BUS_RD9:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD9;
      for(i=2;i<RD9_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD9[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD9_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD9_BufferSize + 4);
      break;
    }
  }

  /* 清零接收缓冲数组的长度*/
  RxBufferLength= 0;
//  memset(RxBuffer,0,sizeof(RxBuffer));
}

/*******************************************************************************
** 函数名称 ：CRC16()
** 函数功能 ：CRC校验码计算及检测（当输入的值包含了以完成的CRC校验值时）
** 入口参数 ：接收到的一帧数据的数组首地址及其长度
** 出口参数 ：CRC校验码（当输入的值包含了以完成的CRC校验值时返回零）
********************************************************************************/
static u8 aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static u8 aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

u16 CRC16(u8 *Frame,u8 Length)     // 传入要校验的数组名及其长度
{
  u8 CRCHi = 0xFF;
  u8 CRCLo = 0xFF;
  int iIndex;
  while( Length-- )
  {
    iIndex = CRCLo ^ *( Frame++ );
    CRCLo = ( u8 )( CRCHi ^ aucCRCHi[iIndex] );
    CRCHi = aucCRCLo[iIndex];
  }
  return ( u16 )( CRCHi << 8 | CRCLo );// CRC校验返回值
}

/*******************************************************************************
** 函数名称 ：RS485_SendData
** 函数功能 ：485发送数据
** 入口参数 ：SendDataLength
** 出口参数 ：无
*******************************************************************************/
void RS485_SendData(u8 SendDataLength)
{
  u8 i;
  RS485_SE;   // RS485发送使能
   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  for(i=0;i<SendDataLength;i++)
  {
    USART_SendData(RS485_USART, TxBuffer[i]);//发送响应
    while(USART_GetFlagStatus(RS485_USART, USART_FLAG_TC) == RESET);
  }
//  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  RS485_RE;    // RS485接收使能
}
/********************************************************************************
** 函数名称 ：TIM_IRQ
** 函数功能 ：TIM中断服务程序
** 入口参数 ：无
** 出口参数 ：无
********************************************************************************/
void TIM_IRQ(void)
{
  TIM_ClearITPendingBit(RS485_TIM, TIM_IT_Update);//定时中断标志清零
  TIM_ITConfig(RS485_TIM, TIM_IT_Update, DISABLE);//中断关闭
  TIM_Cmd(RS485_TIM, DISABLE);
  if((RxBuffer[0] == LocalAddress) || (RxBuffer[0] == BroadcastAddress))
  {
    BUS_DataResponse();
    
  }
  
  RxBufferLength= 0;
  
}
/********************************************************************************
** 函数名称 ：USART_IRQ
** 函数功能 ：USART中断服务程序
** 入口参数 ：无
** 出口参数 ：无
********************************************************************************/
void USART_IRQ(void)
{  
  u8 usartsr;
 if(USART_GetITStatus(RS485_USART, USART_IT_RXNE) != RESET)
  {  
    USART_ClearITPendingBit(RS485_USART, USART_IT_RXNE);
    RxBuffer[RxBufferLength++] = USART_ReceiveData(RS485_USART);
    if(RxBufferLength >= RxBufferSize) 
      RxBufferLength = RxBufferSize-1;
    
    /*每接收完一个字节数据判断一次间隔时间*/
    TIM_ClearITPendingBit(RS485_TIM, TIM_IT_Update); //定时中断标志清零
    TIM_ITConfig(RS485_TIM, TIM_IT_Update, ENABLE);  //中断使能
    TIM_SetCounter(RS485_TIM,0);                  //定时器清零
    TIM_Cmd(RS485_TIM, ENABLE);                   //定时器使能
  }
  else if(USART_GetFlagStatus(RS485_USART, USART_FLAG_ORE) != RESET)
  {   //清除ORE(溢出中断)标志
   //USART_ClearFlag(RS485_USART,USART_FLAG_ORE); //读SR其实就是清除标志
   usartsr = RS485_USART->SR;
   usartsr += USART_ReceiveData(RS485_USART);
  }
}



/******************* (C) COPYRIGHT SWS *****END OF FILE*******************/
