/***************************************************************************
-----------文件信息-----------
名称:SYS.h
所属模块:SWSII型步进电机驱动器
创建:刘旭东
创建日期:2008.8.30
修订记录:Vser1.0
***************************************************************************/
#ifndef __SWSBUS_CCOND_H
#define __SWSBUS_CCOND_H

//用户可配置，可根据需要更改
#define   RS485_TIM_Ccond       TIM1                 //RS485使用TIM
#define   RS485_USART_Ccond     USART1               //RS485使用串口
#define   GPIO_USART_Ccond      GPIOA                //RS485使用GPIO(要求RD,TX,RX使用同一个GPIO)
#define   USART_RD_Ccond        GPIO_Pin_8           //发送接收使能端口
#define   USART_TX_Ccond        GPIO_Pin_9          //串口发送信号端口
#define   USART_RX_Ccond        GPIO_Pin_10          //串口接收信号端口
#define   RS485_RE_Ccond        GPIO_ResetBits(GPIO_USART_Ccond,USART_RD_Ccond)//485接收使能
#define   RS485_SE_Ccond        GPIO_SetBits(GPIO_USART_Ccond,USART_RD_Ccond)  //485发送使能
#define   BaudRate_Ccond        115200                             //设置波特率

/////////RS-485通信///////////////
#define   RS485_Receive_Ccond(ENABLE)  GPIO_ResetBits(GPIO_USART_Ccond, USART_RD_Ccond)
#define   RS485_Send_Ccond(ENABLE)     GPIO_SetBits(GPIO_USART_Ccond, USART_RD_Ccond)

#define   LocalAddress_Ccond       0x25      // 本机地址  
#define   BroadcastAddress_Ccond   0xAA      // 广播地址    
#define   Main_ADD_Ccond           0x55

//用户可配置，可根据需要更改数组长度
#define   TxBufferSize_Ccond     20          //发送缓冲区数组长度 
#define   RxBufferSize_Ccond     20          //接收缓冲区数组长度

#define   WR0_BufferSize_Ccond   0x0F        // 写命令0的接口数组长度 
#define   WR1_BufferSize_Ccond   0x0A        // 写命令1的接口数组长度
#define   WR2_BufferSize_Ccond   0x0A        // 写命令2的接口数组长度 
#define   WR3_BufferSize_Ccond   0x0A        // 写命令3的接口数组长度
#define   WR4_BufferSize_Ccond   0x0A        // 写命令4的接口数组长度 
#define   WR5_BufferSize_Ccond   0x0A        // 写命令5的接口数组长度
#define   WR6_BufferSize_Ccond   0x0A        // 写命令6的接口数组长度
#define   WR7_BufferSize_Ccond   0x0A        // 写命令7的接口数组长度

#define   RD0_BufferSize_Ccond   0x0F        // 读命令0的接口数组长度
#define   RD1_BufferSize_Ccond   0x0A        // 读命令1的接口数组长度 
#define   RD2_BufferSize_Ccond   0x0A        // 读命令2的接口数组长度 
#define   RD3_BufferSize_Ccond   0x0A        // 读命令3的接口数组长度 
#define   RD4_BufferSize_Ccond   0x0A        // 读命令4的接口数组长度 
#define   RD5_BufferSize_Ccond   0x0A        // 读命令5的接口数组长度 
#define   RD6_BufferSize_Ccond   0x0A        // 读命令6的接口数组长度 
#define   RD7_BufferSize_Ccond   0x0c        // 读命令7的接口数组长度 

#define   MIN_BufferSize_Ccond   0x04        // 所有读、写命令的接口数组的最小长度


//extern  u8  EnterInterruptFlag;        //进入中断标志
extern  u8  TxBuffer_Ccond[TxBufferSize_Ccond];    //发送数据缓存
extern  u8  RxBuffer_Ccond[RxBufferSize_Ccond];    //接受数据缓存
extern  u8  TxBufferLength_Ccond;            //发送数据长度
extern  u8  RxBufferLength_Ccond;            //接受数据长度
//extern  u16 CRC_Result;                //CRC校验结果

extern struct COM_Array_Ccond BUS_Buffer_Ccond;

struct COM_Array_Ccond
{
  u8 WR0[WR0_BufferSize_Ccond];    // 写命令0的接口数组
  u8 WR1[WR1_BufferSize_Ccond];    // 写命令1的接口数组
  u8 WR2[WR2_BufferSize_Ccond];    // 写命令2的接口数组
  u8 WR3[WR3_BufferSize_Ccond];    // 写命令3的接口数组
  u8 WR4[WR4_BufferSize_Ccond];    // 写命令4的接口数组
  u8 WR5[WR5_BufferSize_Ccond];    // 写命令5的接口数组
  u8 WR6[WR6_BufferSize_Ccond];    // 写命令6的接口数组
  u8 WR7[WR7_BufferSize_Ccond];    // 写命令7的接口数组
  
  u8 RD0[RD0_BufferSize_Ccond];    // 读命令0的接口数组
  u8 RD1[RD1_BufferSize_Ccond];    // 读命令1的接口数组
  u8 RD2[RD2_BufferSize_Ccond];    // 读命令2的接口数组
  u8 RD3[RD3_BufferSize_Ccond];    // 读命令3的接口数组
  u8 RD4[RD4_BufferSize_Ccond];    // 读命令4的接口数组
  u8 RD5[RD5_BufferSize_Ccond];    // 读命令5的接口数组
  u8 RD6[RD6_BufferSize_Ccond];    // 读命令6的接口数组
  u8 RD7[RD7_BufferSize_Ccond];    // 读命令7的接口数组 
};
/*    //以下这些功能码在swsbus.h中定义 - 可以注释掉
//功能码定义，用户不可更改
#define   BUS_INI     0x20     // 通信初始化状态           
#define   BUS_ASK     0x21     // 向从机发出握手信息 
#define   CRC_ERR     0x22     // CRC校验错误 

#define   BUS_WR0     0x30     // 向从机写0 
#define   BUS_WR1     0x31     // 向从机写1
#define   BUS_WR2     0x32     // 向从机写2
#define   BUS_WR3     0x33     // 向从机写3
#define   BUS_WR4     0x34     // 向从机写4
#define   BUS_WR5     0x35     // 向从机写5
#define   BUS_WR6     0x36     // 向从机写6
#define   BUS_WR7     0x37     // 向从机写7

#define   BUS_RD0     0x40     // 对从机读0
#define   BUS_RD1     0x41     // 对从机读1
#define   BUS_RD2     0x42     // 对从机读2
#define   BUS_RD3     0x43     // 对从机读3
#define   BUS_RD4     0x44     // 对从机读4
#define   BUS_RD5     0x45     // 对从机读5
#define   BUS_RD6     0x46     // 对从机读6
#define   BUS_RD7     0x47     // 对从机读7
*/
extern void BUS_DataResponse_Ccond(void);            //总线数据解析
extern u16 CRC16(u8 *Frame,u8 Length);         //CRC校验
extern void USART1_SendData(u8 SendDataLength);//发送数据   
extern u8 CSTX_Mode;                //醋酸盐模式标志位,lym20100322
void Delay_485_1(u32 dly);
void RS485_Init_Ccond(void);


#endif