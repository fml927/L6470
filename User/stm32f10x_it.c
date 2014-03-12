/**
******************************************************************************
* @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
* @author  MCD Application Team
* @version V3.5.0
* @date    08-April-2011
* @brief   Main Interrupt Service Routines.
*          This file provides template for all exceptions handler and 
*          peripherals interrupt service routine.
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include"stm32f10x_it.h"
#include"swsbus.h"
#include"main.h"
#include"StepMotor.h" 
#include"flag.h"
#include "hp.h"

u8 CheckSum;

u8 EnterInterruptFlag = 0;   //进入中断标记
u32  AP_PWMPulse_Cnt = 0;    //AP泵发出的PWM脉冲数计数器
u32  BP_PWMPulse_Cnt = 0;    //BP泵发出的PWM脉冲数计数器
/*
u8 TxBuffer[TxBufferSize];
u8 RxBuffer[RxBufferSize];
u8 RxBufferLength = 0;
u16 CRC_Result = 0;
u16 t;
u8 Main_ADD=0x55;
*/

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
* @brief  This function handles NMI exception.
* @param  None
* @retval None
*/
void NMI_Handler(void)
{
}


/**
* @brief  This function handles Hard Fault exception.
* @param  None
* @retval None
*/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
*/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
*/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
*/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
*/
void SVC_Handler(void)
{
}

/**
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
*/
void DebugMon_Handler(void)
{
}

/**
* @brief  This function handles PendSVC exception.
* @param  None
* @retval None
*/
void PendSV_Handler(void)
{
}

/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
void SysTick_Handler(void)  //10ms
{
	
    
  LED_COM_Cnt++;    //------20121224-周友兵增加 - 通信正常灯亮后，关闭通信灯
  StepMotor_ADJ_Cnt++;
  Motor_Time++;
  HP_Time++;
  TYPE_Time++;
//  NF_Cnt[UFP]++;
//  NF_Cnt[BPP]++;
//  NF_Cnt[SBP]++;
//  NF_Cnt[SFP]++;
//  NF_Cnt[HP]++;
  

}
/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
向上溢出中断
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{ 
 
}  
 
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       用于与电导(A,B)通信
*******************************************************************************/
void USART1_IRQHandler(void)
{ 
  USART_IRQ();
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       与上位机通信
*******************************************************************************/
void USART2_IRQHandler(void)
{
  
}
/*******************************************************************************
* Function Name  : TIM2_IRQHandler 
* Description    : 补液泵 This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       AP_PWM
*******************************************************************************/
void TIM2_IRQHandler(void)
{
//   StepMotor_PWM_TIM_IRQ(BPP);
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : 肝素泵 This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       AP_PWM
*******************************************************************************/
void TIM3_IRQHandler(void)
{
//   StepMotor_PWM_TIM_IRQ(SFP);
}
/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : 功能泵 This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       BP_PWM
*******************************************************************************/
void TIM4_IRQHandler(void)
{
//  StepMotor_PWM_TIM_IRQ(SBP);    
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : 超滤泵 This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       BP_PWM
*******************************************************************************/
void TIM5_IRQHandler(void)
{
//  StepMotor_PWM_TIM_IRQ(UFP);
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       USART2(上位机通信)的帧超时
*******************************************************************************/
void TIM6_IRQHandler(void)
{
 TIM_IRQ();
   
}
/*******************************************************************************
* Function Name  : TIM8_IRQHandler
* Description    : 血浆泵 This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*       BP_PWM
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{
//  StepMotor_PWM_TIM_IRQ(HP);
}

/*******************************************************************************
* Function Name  : EXTI9_IRQHandler
* Description    : 补液泵,超滤泵
* Input          : None
* Output         : None
* Return         : None
*       AP-HALL
*******************************************************************************/
void EXTI9_5_IRQHandler(void) 
{ 
  StepMotor_EXTI_IRQ(SBP);     //PC8
  StepMotor_EXTI_IRQ(HP);    // PD5
} 
/*******************************************************************************
* Function Name  : EXTI10_IRQHandler
* Description    : 血浆泵,功能泵,肝素泵
* Input          : None
* Output         : None
* Return         : None
*       AP-HALL
*******************************************************************************/
void EXTI15_10_IRQHandler(void) 
{ 
  StepMotor_EXTI_IRQ(UFP);    //PE11
  StepMotor_EXTI_IRQ(BPP);    // PC10
  StepMotor_EXTI_IRQ(SFP);   //  PA12
  
} 

