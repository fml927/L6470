/**
  ******************************************************************************
  * @file    dspin.c 
  * @author  IMMCC Prague FA - MH
  * @version V1.0.0
  * @date    27-May-2011
  * @brief   dSPIN (L6470) product related routines
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */ 


/* Includes ------------------------------------------------------------------*/
#include "dspin.h"
#include "stm32f10x_spi.h"
#include "flag.h"
#include "StepMotor.h"
dSPIN_RegsStruct_TypeDef dSPIN_RegsStruct[5];
void dSPIN_SPI_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  RCC_APB1PeriphClockCmd(dSPIN_PERIPHERAL_CLKs_APB1, ENABLE);
  RCC_APB2PeriphClockCmd(dSPIN_PERIPHERAL_CLKs_APB2, ENABLE);
  
     /* Configure SPI pin: SCK --------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = dSPIN_SCK_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(dSPIN_SCK_Port, &GPIO_InitStructure);

  /* Configure SPI pin: MOSI -------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = dSPIN_MOSI_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(dSPIN_MOSI_Port, &GPIO_InitStructure);
  
    /* Configure SPI pin: MIS0 -------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = dSPIN_MISO_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(dSPIN_MISO_Port, &GPIO_InitStructure);
 				
  /* SPI configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(dSPIN_SPI, &SPI_InitStructure);
  
  /* Enable SPI */
  SPI_Cmd(dSPIN_SPI, ENABLE);
}

/**
  * @brief  Fills-in dSPIN configuration structure with default values.
  * @param  Structure address (pointer to struct)
  * @retval None
  */
void dSPIN_Regs_Init()
{
  u8 i; 
  for(i = 0; i < MOTOR_TOTAL; i++) //配置GPIO
  {  
   dSPIN_CS_Configuration((StepMotor_TypeDef)i);
   dSPIN_FLAG_Configuration((StepMotor_TypeDef)i);
   dSPIN_PC_Configuration((StepMotor_TypeDef)i);
   dSPIN_PWM_Configuration((StepMotor_TypeDef)i);
  } 
  
  for(i = 0; i < MOTOR_TOTAL; i++)  
  {
    Type = i;         //电机SPI操作片选
    if(HP == i)
    {
      dSPIN_RegsStruct[HP].ABS_POS = 0;
      dSPIN_RegsStruct[HP].EL_POS = 0;
      dSPIN_RegsStruct[HP].MARK = 0;
      dSPIN_RegsStruct[HP].SPEED = 0;
      dSPIN_RegsStruct[HP].ACC = AccDec_Steps_to_Par(46);//加速率
      dSPIN_RegsStruct[HP].DEC = AccDec_Steps_to_Par(46); //减速率
      dSPIN_RegsStruct[HP].MAX_SPEED = MaxSpd_Steps_to_Par(488);
      dSPIN_RegsStruct[HP].MIN_SPEED = MinSpd_Steps_to_Par(0);
      dSPIN_RegsStruct[HP].FS_SPD = FSSpd_Steps_to_Par(500);//全步速设定
      dSPIN_RegsStruct[HP].KVAL_HOLD = Kval_Perc_to_Par(0);  //保持占空比
      dSPIN_RegsStruct[HP].KVAL_RUN = Kval_Perc_to_Par(25);
      dSPIN_RegsStruct[HP].KVAL_ACC = Kval_Perc_to_Par(25);  //加速占空比
      dSPIN_RegsStruct[HP].KVAL_DEC = Kval_Perc_to_Par(25);
      dSPIN_RegsStruct[HP].INT_SPD = IntSpd_Steps_to_Par(300);	
      dSPIN_RegsStruct[HP].ST_SLP = BEMF_Slope_Perc_to_Par(0.038);
      dSPIN_RegsStruct[HP].FN_SLP_ACC = BEMF_Slope_Perc_to_Par(0.063);
      dSPIN_RegsStruct[HP].FN_SLP_DEC = BEMF_Slope_Perc_to_Par(0.063);
      dSPIN_RegsStruct[HP].K_THERM = KTherm_to_Par(1);//热补偿参数设置
      dSPIN_RegsStruct[HP].OCD_TH = dSPIN_OCD_TH_6000mA;//过流阈值设置
      dSPIN_RegsStruct[HP].STALL_TH = StallTh_to_Par(3000); //1000mA的失速门槛设置
      dSPIN_RegsStruct[HP].STEP_MODE = dSPIN_STEP_SEL_1_128;
      dSPIN_RegsStruct[HP].ALARM_EN = dSPIN_ALARM_EN_OVERCURRENT | dSPIN_ALARM_EN_THERMAL_SHUTDOWN
        | dSPIN_ALARM_EN_THERMAL_WARNING | dSPIN_ALARM_EN_UNDER_VOLTAGE | dSPIN_ALARM_EN_STALL_DET_A
          | dSPIN_ALARM_EN_STALL_DET_B | dSPIN_ALARM_EN_SW_TURN_ON | dSPIN_ALARM_EN_WRONG_NPERF_CMD;
      dSPIN_RegsStruct[HP].CONFIG = dSPIN_CONFIG_INT_16MHZ_OSCOUT_2MHZ | dSPIN_CONFIG_SW_HARD_STOP
        | dSPIN_CONFIG_VS_COMP_DISABLE | dSPIN_CONFIG_OC_SD_ENABLE | dSPIN_CONFIG_SR_290V_us
          | dSPIN_CONFIG_PWM_DIV_2 | dSPIN_CONFIG_PWM_MUL_1;	
      dSPIN_Reset_Pos();
      dSPIN_Registers_Set(HP);      
    }
    else
    {
      dSPIN_RegsStruct[i].ABS_POS = 0;
      dSPIN_RegsStruct[i].EL_POS = 0;
      dSPIN_RegsStruct[i].MARK = 0;
      dSPIN_RegsStruct[i].SPEED = 0;
      dSPIN_RegsStruct[i].ACC = AccDec_Steps_to_Par(46);//加速率  //466
      dSPIN_RegsStruct[i].DEC = AccDec_Steps_to_Par(46); //减速率 //466
      dSPIN_RegsStruct[i].MAX_SPEED = MaxSpd_Steps_to_Par(500);  //最大150 R/M
      dSPIN_RegsStruct[i].MIN_SPEED = MinSpd_Steps_to_Par(0);
      dSPIN_RegsStruct[i].FS_SPD = FSSpd_Steps_to_Par(500);//全步速设定   //252
      dSPIN_RegsStruct[i].KVAL_HOLD = Kval_Perc_to_Par(0);  //保持占空比 3.5A 24 3A,21 2.5A,18
      dSPIN_RegsStruct[i].KVAL_RUN = Kval_Perc_to_Par(21);
      dSPIN_RegsStruct[i].KVAL_ACC = Kval_Perc_to_Par(21);  //加速占空比
      dSPIN_RegsStruct[i].KVAL_DEC = Kval_Perc_to_Par(21);
      dSPIN_RegsStruct[i].INT_SPD = IntSpd_Steps_to_Par(300);	       //763      德国电机713
      dSPIN_RegsStruct[i].ST_SLP = BEMF_Slope_Perc_to_Par(0.038);       //0.042    德国电机0.038
      dSPIN_RegsStruct[i].FN_SLP_ACC = BEMF_Slope_Perc_to_Par(0.1);   //0.119    德国电机0.132
      dSPIN_RegsStruct[i].FN_SLP_DEC = BEMF_Slope_Perc_to_Par(0.1);  //
      dSPIN_RegsStruct[i].OCD_TH = dSPIN_OCD_TH_6000mA;//过流阈值设置
      dSPIN_RegsStruct[i].STALL_TH = StallTh_to_Par(3000); //1000mA的失速门槛设置
      dSPIN_RegsStruct[i].STEP_MODE = dSPIN_STEP_SEL_1_128;   //dSPIN_ALARM_EN_OVERCURRENT |
      dSPIN_RegsStruct[i].ALARM_EN = dSPIN_ALARM_EN_OVERCURRENT |dSPIN_ALARM_EN_THERMAL_SHUTDOWN 
        |dSPIN_ALARM_EN_THERMAL_WARNING |dSPIN_ALARM_EN_UNDER_VOLTAGE | dSPIN_ALARM_EN_STALL_DET_A
          | dSPIN_ALARM_EN_STALL_DET_B | dSPIN_ALARM_EN_SW_TURN_ON | dSPIN_ALARM_EN_WRONG_NPERF_CMD;
      dSPIN_RegsStruct[i].CONFIG = dSPIN_CONFIG_INT_16MHZ_OSCOUT_2MHZ | dSPIN_CONFIG_SW_HARD_STOP
        | dSPIN_CONFIG_VS_COMP_ENABLE | dSPIN_CONFIG_OC_SD_ENABLE | dSPIN_CONFIG_SR_290V_us
          | dSPIN_CONFIG_PWM_DIV_2 | dSPIN_CONFIG_PWM_MUL_1;	
      dSPIN_Reset_Pos();
      dSPIN_Registers_Set(i);
    }  
  }

        

}
 
/**
  * @brief  Configures dSPIN internal registers with values in the config structure.
  * @param  Configuration structure address (pointer to configuration structure)
  * @retval None
  */
void dSPIN_Registers_Set(u8 i)
{
	dSPIN_Set_Param(dSPIN_ABS_POS, dSPIN_RegsStruct[i].ABS_POS);
	dSPIN_Set_Param(dSPIN_EL_POS, dSPIN_RegsStruct[i].EL_POS);
	dSPIN_Set_Param(dSPIN_MARK, dSPIN_RegsStruct[i].MARK);
	dSPIN_Set_Param(dSPIN_SPEED, dSPIN_RegsStruct[i].SPEED);
	dSPIN_Set_Param(dSPIN_ACC, dSPIN_RegsStruct[i].ACC);
	dSPIN_Set_Param(dSPIN_DEC, dSPIN_RegsStruct[i].DEC);
	dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[i].MAX_SPEED);
	dSPIN_Set_Param(dSPIN_MIN_SPEED, dSPIN_RegsStruct[i].MIN_SPEED);
	dSPIN_Set_Param(dSPIN_FS_SPD, dSPIN_RegsStruct[i].FS_SPD);
	dSPIN_Set_Param(dSPIN_KVAL_HOLD, dSPIN_RegsStruct[i].KVAL_HOLD);
	dSPIN_Set_Param(dSPIN_KVAL_RUN, dSPIN_RegsStruct[i].KVAL_RUN);
	dSPIN_Set_Param(dSPIN_KVAL_ACC, dSPIN_RegsStruct[i].KVAL_ACC);
	dSPIN_Set_Param(dSPIN_KVAL_DEC, dSPIN_RegsStruct[i].KVAL_DEC);
	dSPIN_Set_Param(dSPIN_INT_SPD, dSPIN_RegsStruct[i].INT_SPD);
	dSPIN_Set_Param(dSPIN_ST_SLP, dSPIN_RegsStruct[i].ST_SLP);
	dSPIN_Set_Param(dSPIN_FN_SLP_ACC, dSPIN_RegsStruct[i].FN_SLP_ACC);
	dSPIN_Set_Param(dSPIN_FN_SLP_DEC, dSPIN_RegsStruct[i].FN_SLP_DEC);
	dSPIN_Set_Param(dSPIN_K_THERM, dSPIN_RegsStruct[i].K_THERM);
	dSPIN_Set_Param(dSPIN_OCD_TH, dSPIN_RegsStruct[i].OCD_TH);
	dSPIN_Set_Param(dSPIN_STALL_TH, dSPIN_RegsStruct[i].STALL_TH);
	dSPIN_Set_Param(dSPIN_STEP_MODE, dSPIN_RegsStruct[i].STEP_MODE);
	dSPIN_Set_Param(dSPIN_ALARM_EN, dSPIN_RegsStruct[i].ALARM_EN);
	dSPIN_Set_Param(dSPIN_CONFIG, dSPIN_RegsStruct[i].CONFIG);
}

/**
  * @brief  Issues dSPIN NOP command.
  * @param  None
  * @retval None
  */
void dSPIN_Nop(void)
{
	/* Send NOP operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_NOP);
}

/**
  * @brief  Issues dSPIN Set Param command.
  * @param  dSPIN register address, value to be set
  * @retval None
  */
void dSPIN_Set_Param(dSPIN_Registers_TypeDef param, uint32_t value)
{
	/* Send SetParam operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_SET_PARAM | param);   //强制类型转换避免警告
	switch (param)
	{
		case dSPIN_ABS_POS: ;
		case dSPIN_MARK: ;
		case dSPIN_SPEED:
			/* Send parameter - byte 2 to dSPIN */
			dSPIN_Write_Byte((uint8_t)(value >> 16));
		case dSPIN_ACC: ;
		case dSPIN_DEC: ;
		case dSPIN_MAX_SPEED: ;
		case dSPIN_MIN_SPEED: ;
		case dSPIN_FS_SPD: ;
		case dSPIN_INT_SPD: ;
		case dSPIN_CONFIG: ;
		case dSPIN_STATUS:
			/* Send parameter - byte 1 to dSPIN */
		   	dSPIN_Write_Byte((uint8_t)(value >> 8));
		default:
			/* Send parameter - byte 0 to dSPIN */
		   	dSPIN_Write_Byte((uint8_t)(value));
	}
}

/**
  * @brief  Issues dSPIN Get Param command.
  * @param  dSPIN register address
  * @retval Register value - 1 to 3 bytes (depends on register)
  */
uint32_t dSPIN_Get_Param(dSPIN_Registers_TypeDef param)
{
	uint32_t temp = 0;
	uint32_t rx = 0;

	/* Send GetParam operation code to dSPIN */
	temp = dSPIN_Write_Byte(dSPIN_GET_PARAM | param);
	/* MSB which should be 0 */
	temp = temp << 24;
	rx |= temp;
	switch (param)
	{
		case dSPIN_ABS_POS: ;
		case dSPIN_MARK: ;
		case dSPIN_SPEED:
		   	temp = dSPIN_Write_Byte((uint8_t)(0x00));
			temp = temp << 16;
			rx |= temp;
		case dSPIN_ACC: ;
		case dSPIN_DEC: ;
		case dSPIN_MAX_SPEED: ;
		case dSPIN_MIN_SPEED: ;
		case dSPIN_FS_SPD: ;
		case dSPIN_INT_SPD: ;
		case dSPIN_CONFIG: ;
		case dSPIN_STATUS:
		   	temp = dSPIN_Write_Byte((uint8_t)(0x00));
			temp = temp << 8;
			rx |= temp;
		default:
		   	temp = dSPIN_Write_Byte((uint8_t)(0x00));
			rx |= temp;
	}
	return rx;
}

/**
  * @brief  Issues dSPIN Run command.
  * @param  Movement direction (FWD, REV), Speed - 3 bytes
  * @retval None
  */
void dSPIN_Run(u8 direction, uint32_t speed)
{
	/* Send RUN operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_RUN | direction);
	/* Send speed - byte 2 data dSPIN */
	dSPIN_Write_Byte((uint8_t)(speed >> 16));
	/* Send speed - byte 1 data dSPIN */
	dSPIN_Write_Byte((uint8_t)(speed >> 8));
	/* Send speed - byte 0 data dSPIN */
	dSPIN_Write_Byte((uint8_t)(speed));
}

/**
  * @brief  Issues dSPIN Step Clock command.
  * @param  Movement direction (FWD, REV)
  * @retval None
  */
void dSPIN_Step_Clock(dSPIN_Direction_TypeDef direction)
{
	/* Send StepClock operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_STEP_CLOCK | direction);
}

/**
  * @brief  Issues dSPIN Move command.
  * @param  Movement direction, Number of steps
  * @retval None
  */
void dSPIN_Move(u8 direction, uint32_t n_step)
{
	/* Send Move operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_MOVE | direction);
	/* Send n_step - byte 2 data dSPIN */
	dSPIN_Write_Byte((uint8_t)(n_step >> 16));
	/* Send n_step - byte 1 data dSPIN */
	dSPIN_Write_Byte((uint8_t)(n_step >> 8));
	/* Send n_step - byte 0 data dSPIN */
	dSPIN_Write_Byte((uint8_t)(n_step));
}

/**
  * @brief  Issues dSPIN Go To command.
  * @param  Absolute position where requested to move
  * @retval None
  */
void dSPIN_Go_To(uint32_t abs_pos)
{
	/* Send GoTo operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_GO_TO);
	/* Send absolute position parameter - byte 2 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(abs_pos >> 16));
	/* Send absolute position parameter - byte 1 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(abs_pos >> 8));
	/* Send absolute position parameter - byte 0 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(abs_pos));
}

/**
  * @brief  Issues dSPIN Go To Dir command.
  * @param  Movement direction, Absolute position where requested to move
  * @retval None
  */
void dSPIN_Go_To_Dir(u8 direction, uint32_t abs_pos)
{
	/* Send GoTo_DIR operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_GO_TO_DIR | direction);
	/* Send absolute position parameter - byte 2 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(abs_pos >> 16));
	/* Send absolute position parameter - byte 1 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(abs_pos >> 8));
	/* Send absolute position parameter - byte 0 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(abs_pos));
}

//void dSPIN_Go_To_Dir(dSPIN_Direction_TypeDef direction, uint32_t abs_pos)
//{
//	/* Send GoTo_DIR operation code to dSPIN */
//	dSPIN_Write_Byte(dSPIN_GO_TO_DIR | direction);
//	/* Send absolute position parameter - byte 2 data to dSPIN */
//	dSPIN_Write_Byte((uint8_t)(abs_pos >> 16));
//	/* Send absolute position parameter - byte 1 data to dSPIN */
//	dSPIN_Write_Byte((uint8_t)(abs_pos >> 8));
//	/* Send absolute position parameter - byte 0 data to dSPIN */
//	dSPIN_Write_Byte((uint8_t)(abs_pos));
//}

/**
  * @brief  Issues dSPIN Go Until command.
  * @param  Action, Movement direction, Speed
  * @retval None
  */
void dSPIN_Go_Until(dSPIN_Action_TypeDef action, dSPIN_Direction_TypeDef direction, uint32_t speed)
{
	/* Send GoUntil operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_GO_UNTIL | action | direction);
	/* Send speed parameter - byte 2 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(speed >> 16));
	/* Send speed parameter - byte 1 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(speed >> 8));
	/* Send speed parameter - byte 0 data to dSPIN */
	dSPIN_Write_Byte((uint8_t)(speed));
}

/**
  * @brief  Issues dSPIN Release SW command.
  * @param  Action, Movement direction
  * @retval None
  */
void dSPIN_Release_SW(dSPIN_Action_TypeDef action, dSPIN_Direction_TypeDef direction)
{
	/* Send ReleaseSW operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_RELEASE_SW | action | direction);
}

/**
  * @brief  Issues dSPIN Go Home command. (Shorted path to zero position)
  * @param  None
  * @retval None
  */
void dSPIN_Go_Home(void)
{
	/* Send GoHome operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_GO_HOME);
}

/**
  * @brief  Issues dSPIN Go Mark command.
  * @param  None
  * @retval None
  */
void dSPIN_Go_Mark(void)
{
	/* Send GoMark operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_GO_MARK);
}

/**
  * @brief  Issues dSPIN Reset Pos command.
  * @param  None
  * @retval None
  */
void dSPIN_Reset_Pos(void)
{
	/* Send ResetPos operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_RESET_POS);
}

/**
  * @brief  Issues dSPIN Reset Device command.
  * @param  None
  * @retval None
  */
void dSPIN_Reset_Device(void)
{
	/* Send ResetDevice operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_RESET_DEVICE);
}

/**
  * @brief  Issues dSPIN Soft Stop command.
  * @param  None
  * @retval None
  */
void dSPIN_Soft_Stop(void)
{
	/* Send SoftStop operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_SOFT_STOP);
}

/**
  * @brief  Issues dSPIN Hard Stop command.
  * @param  None
  * @retval None
  */
void dSPIN_Hard_Stop(void)
{
	/* Send HardStop operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_HARD_STOP);
}

/**
  * @brief  Issues dSPIN Soft HiZ command.
  * @param  None
  * @retval None
  */
void dSPIN_Soft_HiZ(void)
{
	/* Send SoftHiZ operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_SOFT_HIZ);   //
}

/**
  * @brief  Issues dSPIN Hard HiZ command.
  * @param  None
  * @retval None
  */
void dSPIN_Hard_HiZ(void)
{
	/* Send HardHiZ operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_HARD_HIZ);
}

/**
  * @brief  Issues dSPIN Get Status command.
  * @param  None
  * @retval Status Register content
  */
uint16_t dSPIN_Get_Status(void)
{
	uint16_t temp = 0;
	uint16_t rx = 0;

	/* Send GetStatus operation code to dSPIN */
	dSPIN_Write_Byte(dSPIN_GET_STATUS);
	/* Send zero byte / receive MSByte from dSPIN */
	temp = dSPIN_Write_Byte((uint8_t)(0x00));
	temp = temp << 8;
	rx |= temp;
	/* Send zero byte / receive LSByte from dSPIN */
	temp = dSPIN_Write_Byte((uint8_t)(0x00));
	rx |= temp;
	return rx;
}

/**
  * @brief  Checks if the dSPIN is Busy by hardware - active Busy signal.
  * @param  None
  * @retval one if chip is busy, otherwise zero
  */
uint8_t dSPIN_Busy_HW(void)
{
uint8_t Value = 0;
    switch(Type)
    {
    case 0:
      Value = (GPIO_ReadInputDataBit(dSPIN_UFP_BUSY_Port, dSPIN_UFP_BUSY_Pin));
      break;
    case 1:
      Value = (GPIO_ReadInputDataBit(dSPIN_BPP_BUSY_Port, dSPIN_BPP_BUSY_Pin));
      break;
    case 2:
      Value = (GPIO_ReadInputDataBit(dSPIN_SBP_BUSY_Port, dSPIN_SBP_BUSY_Pin));
      break;
    case 3:
      Value = (GPIO_ReadInputDataBit(dSPIN_SFP_BUSY_Port, dSPIN_SFP_BUSY_Pin));
      break;
    case 4:
      Value = (GPIO_ReadInputDataBit(dSPIN_HP_BUSY_Port, dSPIN_HP_BUSY_Pin));
      break;    
    default:
      break;
    }   
    if(!Value) 
      return 0x01;
    else 
      return 0x00;
}

/**
  * @brief  Checks if the dSPIN is Busy by SPI - Busy flag bit in Status Register.
  * @param  None
  * @retval one if chip is busy, otherwise zero
  */
uint8_t dSPIN_Busy_SW(void)
{
	if(!(dSPIN_Get_Status() & dSPIN_STATUS_BUSY)) return 0x01;
	else return 0x00;
}

/**
  * @brief  Checks dSPIN Flag signal.
  * @param  None
  * @retval one if Flag signal is active, otherwise zero
  */
uint8_t dSPIN_Flag(void)
{
	if(!(GPIO_ReadInputDataBit(dSPIN_UFP_FLAG_Port, dSPIN_UFP_FLAG_Pin))) return 0x01;
	else return 0x00;
}

/**
  * @brief  Transmits/Receives one byte to/from dSPIN over SPI.
  * @param  Transmited byte
  * @retval Received byte
  */
uint8_t dSPIN_Write_Byte(uint8_t byte)
{
        /* nSS signal activation - low */
        switch(Type)
        {
        case 0:
          GPIO_ResetBits(dSPIN_UFP_CS_Port, dSPIN_UFP_CS_Pin);
          break;
        case 1:
          GPIO_ResetBits(dSPIN_BPP_CS_Port, dSPIN_BPP_CS_Pin);
          break;
        case 2:
          GPIO_ResetBits(dSPIN_SBP_CS_Port, dSPIN_SBP_CS_Pin);
          break;
        case 3:
          GPIO_ResetBits(dSPIN_SFP_CS_Port, dSPIN_SFP_CS_Pin);
          break;
        case 4:
          GPIO_ResetBits(dSPIN_HP_CS_Port, dSPIN_HP_CS_Pin);
          break;
        default:
          break;
        }
    
  
	/* SPI byte send */
        SPI_I2S_SendData(dSPIN_SPI, byte);
	/* Wait for SPIx Busy flag */
	while (SPI_I2S_GetFlagStatus(dSPIN_SPI, SPI_I2S_FLAG_BSY) != RESET);
	/* nSS signal deactivation - high */
        switch(Type)
        {
        case 0:
          GPIO_SetBits(dSPIN_UFP_CS_Port, dSPIN_UFP_CS_Pin);
          break;
        case 1:
          GPIO_SetBits(dSPIN_BPP_CS_Port, dSPIN_BPP_CS_Pin);
          break;
        case 2:
          GPIO_SetBits(dSPIN_SBP_CS_Port, dSPIN_SBP_CS_Pin);
          break;
        case 3:
          GPIO_SetBits(dSPIN_SFP_CS_Port, dSPIN_SFP_CS_Pin);
          break;
        case 4:
          GPIO_SetBits(dSPIN_HP_CS_Port, dSPIN_HP_CS_Pin);
          break;
        default:
          break;
        }
	return (uint8_t)(SPI_I2S_ReceiveData(dSPIN_SPI));
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
