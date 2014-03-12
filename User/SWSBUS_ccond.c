/************************ (C) COPYRIGHT 2008 SWS *******************************
* File Name          : swsbus.c
* Author             : Tong Jin
* Version            : V1.1
* Date               : 2008.10.7
* Description        : SWSBUS协议

V1.1:修改RD0-RD7读命令过程中数据传递问题
修改添加CRC码时进行两次CRC检验问题
修改485使能延时过长问题
********************************************************************************/

//#include"stm32f10x_lib.h"
#include"stm32f10x.h"
#include"swsbus_ccond.h"
#include"swsbus.h"
struct COM_Array_Ccond BUS_Buffer_Ccond;

u8 TxBuffer_Ccond[TxBufferSize_Ccond];
u8 RxBuffer_Ccond[RxBufferSize_Ccond];
u8 RxBufferLength_Ccond = 0;
//u16 CRC_Result = 0;
//u8 Main_ADD=0x55;

/*******************************************************************************
** 函数名称 ：RS485_Init
** 函数功能 ：RS485端口及串口配置
** 入口参数 ：无
** 出口参数 ：无
*******************************************************************************/
void RS485_Init_Ccond(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  //TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  //TIM_OCInitTypeDef  TIM_OCInitStructure;
  
/************************  RS485端口及串口配置   ******************************/  
  /* 配置485通信TX端口 */
  GPIO_InitStructure.GPIO_Pin = USART_TX_Ccond;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_USART_Ccond, &GPIO_InitStructure);
  /* 配置485通信RX端口 */
  GPIO_InitStructure.GPIO_Pin = USART_RX_Ccond;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USART_Ccond, &GPIO_InitStructure);
  /* 配置485通信发送接收使能端口*/
  GPIO_InitStructure.GPIO_Pin = USART_RD_Ccond;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_USART_Ccond, &GPIO_InitStructure);
  
  /* 配置485通信串口*/
  USART_InitStructure.USART_BaudRate = BaudRate_Ccond;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(RS485_USART_Ccond, &USART_InitStructure);
  USART_ITConfig(RS485_USART_Ccond, USART_IT_RXNE, ENABLE);
  USART_Cmd(RS485_USART_Ccond, ENABLE);
  RS485_RE_Ccond;      //485接收使能
  
/************************  RS485通信定时器配置   ******************************/ 
  //使用TIM1,在system_config.c中进行配置
  /*
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(RS485_TIM_Ccond, &TIM_TimeBaseStructure);
  
  //Prescaler configuration :
  TIM_InternalClockConfig(RS485_TIM_Ccond);
  TIM_PrescalerConfig(RS485_TIM_Ccond, 0x0E0F, TIM_PSCReloadMode_Immediate);//clock = 10 KHz

  // Output Compare Active Mode configuration: Channel1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 5;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(RS485_TIM_Ccond, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(RS485_TIM_Ccond, TIM_OCPreload_Disable);
  TIM_Cmd(RS485_TIM_Ccond, DISABLE);
  */
}
/*******************************************************************************
* Function Name  : BUS_DataResponse
* Description    : 串口通信中接受到一帧数据后判断是什么命令并作出相应的响应
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BUS_DataResponse_Ccond(void)
{
  u8 i;
  u16 CRC_CODE;
  u16 CRC_Result;
  CRC_Result = CRC16(RxBuffer_Ccond,RxBufferLength_Ccond); 
  if(CRC_Result==0)   //CRC校验正确结果为0
  {  //从机读
    switch(RxBuffer_Ccond[1]) //根据功能码执行相应功能              
    {
    case BUS_INI:       //初始化                     
      break;
      
    case BUS_ASK:       //握手命令
      TxBuffer_Ccond[0] = LocalAddress_Ccond;      //本机地址
      TxBuffer_Ccond[1] = BUS_ASK;           //功能码
      CRC_CODE = CRC16(TxBuffer_Ccond,2);    //计算CRC码
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        //CRC校验码的低字节
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); //CRC校验码的高字节  
      USART1_SendData(MIN_BufferSize_Ccond);          //向上位机发送响应数据
      break;
      
    case BUS_WR0:                      //向丛机写数据0
      BUS_Buffer_Ccond.WR0[0] = 1;           //表示有新的数据更新
      for(i=2;i<RxBufferLength_Ccond-2;i++)  //去除地址，功能和CRC码
      {
        BUS_Buffer_Ccond.WR0[i-1] = RxBuffer_Ccond[i];
      }   
      TxBuffer_Ccond[0] = LocalAddress_Ccond;      //本机地址
      TxBuffer_Ccond[1] = BUS_WR0;           //功能码
      CRC_CODE = CRC16(TxBuffer_Ccond,2);    //计算CRC码
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        //CRC校验码的低字节
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); //CRC校验码的高字节  
      USART1_SendData(MIN_BufferSize_Ccond);          //向上位机发送响应数据
      break;
      
    case BUS_WR1:
      BUS_Buffer_Ccond.WR1[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++)
      {
        BUS_Buffer_Ccond.WR1[i-1] = RxBuffer_Ccond[i];
      }
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR1;
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); 
      USART1_SendData(MIN_BufferSize_Ccond);
      break;
      
    case BUS_WR2: 
      BUS_Buffer_Ccond.WR2[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++)  
      {
        BUS_Buffer_Ccond.WR2[i-1] = RxBuffer_Ccond[i];
      }  
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR2;
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); 
      USART1_SendData(MIN_BufferSize_Ccond);
      break;
      
    case BUS_WR3:
      BUS_Buffer_Ccond.WR3[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++)
      {
        BUS_Buffer_Ccond.WR3[i-1] = RxBuffer_Ccond[i];
      }    
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR3;
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); 
      USART1_SendData(MIN_BufferSize_Ccond);
      break;
      
    case BUS_WR4:
      BUS_Buffer_Ccond.WR4[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++)
      {
        BUS_Buffer_Ccond.WR4[i-1] = RxBuffer_Ccond[i];
      }   
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR4;
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); 
      USART1_SendData(MIN_BufferSize_Ccond);
      break;
      
    case BUS_WR5:
      BUS_Buffer_Ccond.WR5[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++)
      {
        BUS_Buffer_Ccond.WR5[i-1] = RxBuffer_Ccond[i];
      }    
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR5;
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF);        
      USART1_SendData(MIN_BufferSize_Ccond);
      break;
      
    case BUS_WR6:
      BUS_Buffer_Ccond.WR6[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++)
      {
        BUS_Buffer_Ccond.WR6[i-1] = RxBuffer_Ccond[i];
      }     
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR6;
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF);            
      USART1_SendData(MIN_BufferSize_Ccond);
      break;
      
    case BUS_WR7:  
      BUS_Buffer_Ccond.WR7[0] = 1;
      for(i=2;i<RxBufferLength_Ccond-2;i++) 
      {
        BUS_Buffer_Ccond.WR7[i-1] = RxBuffer_Ccond[i];
      }     
      TxBuffer_Ccond[0] = LocalAddress_Ccond;
      TxBuffer_Ccond[1] = BUS_WR7;           
      CRC_CODE = CRC16(TxBuffer_Ccond,2);
      TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        
      TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF);             
      USART1_SendData(MIN_BufferSize_Ccond); 
      break;
      //主机读
    case BUS_RD0: 
      BUS_Buffer_Ccond.RD0[0]=1;                      //数据更新标志
      for(i=2;i<RD0_BufferSize_Ccond+2;i++)
      {
        BUS_Buffer_Ccond.RD0[i-1]=RxBuffer_Ccond[i];        //接收从机数据(去除地址、功能码）
      }
      //  CRC_CODE = CRC16(TxBuffer_Ccond,RD0_BufferSize_Ccond + 2);//计算CRC码
      //   TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);            //CRC校验码的低字节
      //  TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);   //CRC校验码的高字节
      //   USART1_SendData(RD0_BufferSize_Ccond + 4);          //向上位机发送响应数据
      break;
      
    case BUS_RD1:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD1;          
      for(i=2;i<RD1_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD1[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD1_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD1_BufferSize_Ccond + 4);
      break;
      
    case BUS_RD2:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD2;          
      for(i=2;i<RD2_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD2[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD2_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD2_BufferSize_Ccond + 4);
      break;
      
    case BUS_RD3:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD3;          
      for(i=2;i<RD3_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD3[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD3_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD3_BufferSize_Ccond + 4);
      break;
      
    case BUS_RD4:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD4;          
      for(i=2;i<RD4_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD4[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD4_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD4_BufferSize_Ccond + 4);
      break;
      
    case BUS_RD5:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD5;          
      for(i=2;i<RD5_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD5[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD5_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD5_BufferSize_Ccond + 4);
      break;
      
    case BUS_RD6:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD6;          
      for(i=2;i<RD6_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD6[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD6_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD6_BufferSize_Ccond + 4);
      break;
      
    case BUS_RD7:                       
      TxBuffer_Ccond[0] = LocalAddress_Ccond; 
      TxBuffer_Ccond[1] = BUS_RD7;          
      for(i=2;i<RD7_BufferSize_Ccond+2;i++)
      {
        TxBuffer_Ccond[i] = BUS_Buffer_Ccond.RD7[i-2];
      }
      CRC_CODE = CRC16(TxBuffer_Ccond,RD7_BufferSize_Ccond + 2);
      TxBuffer_Ccond[i] = (CRC_CODE & 0x00FF);         
      TxBuffer_Ccond[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      USART1_SendData(RD7_BufferSize_Ccond + 4);
      break;
      
    }
  }
  else                              //CRC校验错误
  {    
    TxBuffer_Ccond[0] = LocalAddress_Ccond;   //本机地址
    TxBuffer_Ccond[1] = CRC_ERR;        //功能码（CRC校验错误）
    CRC_CODE = CRC16(TxBuffer_Ccond,2); //计算CRC码
    TxBuffer_Ccond[2] = (CRC_CODE & 0x00FF);        //CRC校验码的低字节
    TxBuffer_Ccond[3] = ((CRC_CODE >> 8) & 0x00FF); //CRC校验码的高字节   
    /*向上位机发送响应数据*/
    USART1_SendData(MIN_BufferSize_Ccond);
  }
  
  /* 清零接收缓冲数组的长度*/
  RxBufferLength_Ccond= 0;    
}



/*******************************************************************************
* Function Name  : USART2_SendData
* Description    : 
* Input          : SendDataLength
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_SendData(u8 SendDataLength)
{
  u8 i;
  u8 TxBuffer_CcondLength = 0;
  RS485_Send_Ccond(ENABLE);    // RS485使能发送
  Delay_485_1(1);
  for(i=0;i<SendDataLength;i++)
  {
    USART_SendData(USART1, TxBuffer_Ccond[TxBuffer_CcondLength++]);//发送响应
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  }
  
  RS485_Receive_Ccond(ENABLE);  // RS485使能接收
  Delay_485_1(1);
}

/********************************************************************************
** 函数名称 ：Delay()
** 函数功能 ：
** 入口参数 ：
** 出口参数 ：
********************************************************************************/
void Delay_485_1(u32 dly)
{
  u8 i;
  while(dly--)
  {
    i = 8;
    while(--i);
  }
}

/******************* (C) COPYRIGHT 2008 SWS *****END OF FILE*******************/
