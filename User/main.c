#include "stm32f10x.h"
#include "main.h"
#include "swsbus.h"
#include "flag.h"
#include "hp.h"
#include "dspin.h"
#include "StepMotor.h"
#include "ad7799.h"

/*-----------------全局变量------------------*/
u16   LED_COM_Cnt = 0;

/**************************************************************************
函数原型：void delay(u16 uS)
功能：	  延时
入口参数：无
出口参数：无
***************************************************************************/
void Delay(u16 uS)
{
  u8 i;
  while(uS--)
  {
    i=7;
    while(i--);
  }
}
int main(void)
{ 
  System_Init();	         //系统初始化
  RS485_Init();
  StepMotor_Init();
  dSPIN_SPI_Init();
  dSPIN_Regs_Init();
  ADC7799_Init();
  HP_CheckInfo();                   //读取肝素泵校正信息
  
  while(1)
  {
    ADC_Auto_Conversion();
    Syringe_Type();                  //读取AD值，注射器型号检测
    Down_flag();                     //解析标志位
    check_lockrotor();               //系统，电机运行指示灯
    Motor_Run_Mode();  
//    dSPIN_Work();                  //针对THB6064AH的应用函数
    HP_FlowACC();                      //肝素泵流量累计
    Up_flag();                       //上行消息    
  }
 
}  
 

void check_lockrotor(void)
{
  if(LED_COM_Cnt < 100)  //系统运行指示灯
  { 
    LED_RUN_OFF; 
    LED_485_OFF;
    
  }
  else if(LED_COM_Cnt < 200)
  {
    LED_RUN_ON; 
    
  }
  else if(200 <LED_COM_Cnt)
  {
    LED_COM_Cnt = 0;
  }
}
  
  

