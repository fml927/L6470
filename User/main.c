#include "stm32f10x.h"
#include "main.h"
#include "swsbus.h"
#include "flag.h"
#include "hp.h"
#include "dspin.h"
#include "StepMotor.h"
#include "ad7799.h"

/*-----------------ȫ�ֱ���------------------*/
u16   LED_COM_Cnt = 0;

/**************************************************************************
����ԭ�ͣ�void delay(u16 uS)
���ܣ�	  ��ʱ
��ڲ�������
���ڲ�������
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
  System_Init();	         //ϵͳ��ʼ��
  RS485_Init();
  StepMotor_Init();
  dSPIN_SPI_Init();
  dSPIN_Regs_Init();
  ADC7799_Init();
  HP_CheckInfo();                   //��ȡ���ر�У����Ϣ
  
  while(1)
  {
    ADC_Auto_Conversion();
    Syringe_Type();                  //��ȡADֵ��ע�����ͺż��
    Down_flag();                     //������־λ
    check_lockrotor();               //ϵͳ���������ָʾ��
    Motor_Run_Mode();  
//    dSPIN_Work();                  //���THB6064AH��Ӧ�ú���
    HP_FlowACC();                      //���ر������ۼ�
    Up_flag();                       //������Ϣ    
  }
 
}  
 

void check_lockrotor(void)
{
  if(LED_COM_Cnt < 100)  //ϵͳ����ָʾ��
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
  
  

