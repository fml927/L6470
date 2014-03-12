/******************************************************************************
----------------文件信息-----------------
名称：SYS.c
所属模块：配液板
功能:时钟\中断\定时器\串口配置
创建：刘旭东
创建日期：2009.2.13
当前版本: Vser1.0
修订记录:

******************************************************************************/
//#include"stm32f10x_lib.h"
#include "stm32f10x.h"
/******************************************************************************
函数原形:void SetupClock (void)
功能:	 系统各时钟配置
入口参数:无
出口参数:无
******************************************************************************/
void SetupClock (void)
{
  SystemInit();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE); 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  //Enable AFIO clocks    -- 引脚重映射需要的RCC,如果没有使用到引脚重映射，可以注释掉
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   // ****************** USART1,TIM2,3,4引脚重映射************************
  GPIO_PinRemapConfig(GPIO_Remap_USART1|GPIO_FullRemap_TIM2|GPIO_FullRemap_TIM3|GPIO_Remap_TIM4,ENABLE); //重映射TIM3引脚

   /* Setup SysTick Timer for 100ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))  // 100->10ms, 1000->1ms, 400->2.5ms, /10000->0.1ms
  { 
    /* Capture error */ 
    while (1);
  }
}

/*******************************************************************************
函数原形: void SetNVIC(void)
功能:	  非向量中断配置
入口参数: 无
出口参数: 无
*******************************************************************************/
void SetNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  //配置TIM6中断  - 上位机通信RS485，帧判断
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //配置UART2中断 - 上位机通信RS485，
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);       
  
//  //配置TIM3中断 - UFP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
// 
//  //配置TIM4中断  BPP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//  //配置TIM4中断  SBP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//    //配置TIM4中断  SFP_PWM
//  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//    //配置TIM4中断  HP_PWM
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
函数原形:void GPIO_Config(void)
功能:	 端口引脚模式配置
入口参数: 无
出口参数: 无
**************************************************************************/
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_Fig;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  
  //指示灯: PE1,2,3,4,5,6;   PA0 系统运行闪烁指示灯
  GPIO_Fig.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOE,&GPIO_Fig);
  
  
  GPIO_Fig.GPIO_Pin=GPIO_Pin_0;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_Fig);
 
  //肝素泵检测：HS1(PD3),HS2(PD4),HS3(PD5),HS4(PD2): 为针管大小检测的3个光电开关。
  //HS5(PD7)到底检测开关，HS6(PD6):为安装状态检测

//  GPIO_Fig.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
//  GPIO_Fig.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOD,&GPIO_Fig);

}






/************************************************************************
函数原形:void System_Init(void)
功能:系统初始化
入口参数:无
出口参数:无
************************************************************************/
void System_Init(void)
{
  SetupClock ();
  GPIO_Config();
  SetNVIC();
}


