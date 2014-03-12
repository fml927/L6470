/******************************************************************************
----------------�ļ���Ϣ-----------------
���ƣ�SYS.c
����ģ�飺��Һ��
����:ʱ��\�ж�\��ʱ��\��������
����������
�������ڣ�2009.2.13
��ǰ�汾: Vser1.0
�޶���¼:

******************************************************************************/
//#include"stm32f10x_lib.h"
#include "stm32f10x.h"
/******************************************************************************
����ԭ��:void SetupClock (void)
����:	 ϵͳ��ʱ������
��ڲ���:��
���ڲ���:��
******************************************************************************/
void SetupClock (void)
{
  SystemInit();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE); 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  //Enable AFIO clocks    -- ������ӳ����Ҫ��RCC,���û��ʹ�õ�������ӳ�䣬����ע�͵�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   // ****************** USART1,TIM2,3,4������ӳ��************************
  GPIO_PinRemapConfig(GPIO_Remap_USART1|GPIO_FullRemap_TIM2|GPIO_FullRemap_TIM3|GPIO_Remap_TIM4,ENABLE); //��ӳ��TIM3����

   /* Setup SysTick Timer for 100ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))  // 100->10ms, 1000->1ms, 400->2.5ms, /10000->0.1ms
  { 
    /* Capture error */ 
    while (1);
  }
}

/*******************************************************************************
����ԭ��: void SetNVIC(void)
����:	  �������ж�����
��ڲ���: ��
���ڲ���: ��
*******************************************************************************/
void SetNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  //����TIM6�ж�  - ��λ��ͨ��RS485��֡�ж�
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //����UART2�ж� - ��λ��ͨ��RS485��
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);       
  
//  //����TIM3�ж� - UFP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
// 
//  //����TIM4�ж�  BPP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//  //����TIM4�ж�  SBP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//    //����TIM4�ж�  SFP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//    //����TIM4�ж�  HP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/**************************************************************************
����ԭ��:void GPIO_Config(void)
����:	 �˿�����ģʽ����
��ڲ���: ��
���ڲ���: ��
**************************************************************************/
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_Fig;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  
  //ָʾ��: PE1,2,3,4,5,6;   PA0 ϵͳ������˸ָʾ��
  GPIO_Fig.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOE,&GPIO_Fig);
  
  
  GPIO_Fig.GPIO_Pin=GPIO_Pin_0;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_Fig);
 
  //���رü�⣺HS1(PD3),HS2(PD4),HS3(PD5),HS4(PD2): Ϊ��ܴ�С����3����翪�ء�
  //HS5(PD7)���׼�⿪�أ�HS6(PD6):Ϊ��װ״̬���

//  GPIO_Fig.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
//  GPIO_Fig.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOD,&GPIO_Fig);

}






/************************************************************************
����ԭ��:void System_Init(void)
����:ϵͳ��ʼ��
��ڲ���:��
���ڲ���:��
************************************************************************/
void System_Init(void)
{
  SetupClock ();
  GPIO_Config();
  SetNVIC();
}


