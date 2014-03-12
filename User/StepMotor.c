#include "stm32f10x.h"
#include "StepMotor.h"
#include "dspin.h"
#include "flag.h"
u32 StepMotor_ADJ_Cnt = 0;   //调节PWM频率时间间隔计数
u32 TEMP_Circle[5] = {0};   //判断堵转时临时圈数
u32 TEMP_Pulse[5] = {0};    //判断堵转时临时脉冲
StepMotor_WorkInfo_Def StepMotor_Data[5];


/*******************************************************************************
* Function Name  : StepMotor_REAL_Pin_EXTI_Configuration
* Description    : 配置堵转检测的GPIO及其EXTI中断
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_REAL_Pin_EXTI_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_REAL_PORT,MOTOR2_REAL_PORT,MOTOR3_REAL_PORT,MOTOR4_REAL_PORT,MOTOR5_REAL_PORT,MOTOR6_REAL_PORT};   //端口
  uint16_t    Motor_REAL_PINx[6] =  {MOTOR1_REAL_PIN,MOTOR2_REAL_PIN,MOTOR3_REAL_PIN,MOTOR4_REAL_PIN,MOTOR5_REAL_PIN,MOTOR6_REAL_PIN};    //Pin
  uint16_t    Source_PORT[6] =     {MOTOR1_REAL_SourcePORT,MOTOR2_REAL_SourcePORT,MOTOR3_REAL_SourcePORT,MOTOR4_REAL_SourcePORT,MOTOR5_REAL_SourcePORT,MOTOR6_REAL_SourcePORT};   //端口
  uint16_t    Source_PINx[6] =     {MOTOR1_REAL_SourcePIN,MOTOR2_REAL_SourcePIN,MOTOR3_REAL_SourcePIN,MOTOR4_REAL_SourcePIN,MOTOR5_REAL_SourcePIN,MOTOR6_REAL_SourcePIN};    //Pin
  uint32_t    EXTI_Linex[6]  =     {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin
 
   // 配置REAL脚：浮空输入  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_REAL_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
    
    GPIO_EXTILineConfig(Source_PORT[Motorx], Source_PINx[Motorx]);
  
    /* Configure EXTI Linex to generate an interrupt on  edge */  
    EXTI_InitStructure.EXTI_Line = EXTI_Linex[Motorx];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = MOTOR_REAL_EXTI_TRIGGER;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
} 
void StepMotor_Init(void)
{
  u8 i;
  for(i = 0; i < MOTOR_TOTAL; i++)  //清空工作状态
  {
    StepMotor_Data[i].Enable = DISABLE;
    StepMotor_Data[i].Status = DISABLE;  //正常应初始化为DISABLE，这里初始化为ENABLE的目的是让处理程序释放电机
    StepMotor_Data[i].SetDir = 0;
    StepMotor_Data[i].SetSpeed = 0;
    StepMotor_Data[i].CurrSpeed = 0;
    StepMotor_Data[i].ToBottom = 0;
    StepMotor_Data[i].ToTop = 0;
    StepMotor_Data[i].Alert = 0;
    StepMotor_Data[i].NF = 0;
    StepMotor_Data[i].Braking = 0;
    StepMotor_Data[i].RunSta_Brak = 0;
    StepMotor_Data[i].LockRotor = 0;
    StepMotor_Data[i].Flow = 0;
    StepMotor_Data[i].FlowErr = 0;
    StepMotor_Data[i].LockRotorSpeed = 0;
    StepMotor_Data[i].FlowLockRotor = 0;
    StepMotor_Data[i].RatioSwitch = 0; 
    StepMotor_Data[i].Down_Mode = 0; 
    StepMotor_Data[i].Ratio = 0; 
    StepMotor_Data[i].SetFlow = 0;
    StepMotor_Data[i].Step = 0;
    StepMotor_Data[i].Temp_Circle = 0;
    StepMotor_Data[i].Circle = 0;
    StepMotor_Data[i].ReadSpeed = 0;
    StepMotor_Data[i].CMD = 0;
    StepMotor_REAL_Pin_EXTI_Configuration((StepMotor_TypeDef)i);
  }
}
/*******************************************************************************
* Function Name  : dSPIN_CS_Configuration
* Description    : 片选
* 输入参数       : 
* Output         : 
* Return         : 
*******************************************************************************/
void dSPIN_CS_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *dSPIN_PORT[5] = {dSPIN_UFP_CS_Port,dSPIN_BPP_CS_Port,dSPIN_SBP_CS_Port,dSPIN_SFP_CS_Port,dSPIN_HP_CS_Port};   //端口
  uint16_t   dSPIN_PINx[5] =    {dSPIN_UFP_CS_Pin,dSPIN_BPP_CS_Pin,dSPIN_SBP_CS_Pin,dSPIN_SFP_CS_Pin,dSPIN_HP_CS_Pin};    //Pin
  
  //推挽输出 
  if(dSPIN_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = dSPIN_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(dSPIN_PORT[Motorx], &GPIO_InitStructure);  
  }
}

/*******************************************************************************
* Function Name  : dSPIN_FLAG_Configuration
* Description    : 
* 输入参数       : 
* Output         : 
* Return         : 
*******************************************************************************/
void dSPIN_FLAG_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *dSPIN_PORT[5] = {dSPIN_UFP_FLAG_Port,dSPIN_BPP_FLAG_Port,dSPIN_SBP_FLAG_Port,dSPIN_SFP_FLAG_Port,dSPIN_HP_FLAG_Port};   //端口
  uint16_t   dSPIN_PINx[5] =    {dSPIN_UFP_FLAG_Pin,dSPIN_BPP_FLAG_Pin,dSPIN_SBP_FLAG_Pin,dSPIN_SFP_FLAG_Pin,dSPIN_HP_FLAG_Pin};    //Pin
  
  if(dSPIN_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = dSPIN_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(dSPIN_PORT[Motorx], &GPIO_InitStructure);  
  }
}

void dSPIN_PWM_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_PWM_EN
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_PWM_PORT,MOTOR2_PWM_PORT,MOTOR3_PWM_PORT,MOTOR4_PWM_PORT,MOTOR5_PWM_PORT,MOTOR6_PWM_PORT};   //端口
  uint16_t   Motor_PWM_PINx[6] =    {MOTOR1_PWM_PIN,MOTOR2_PWM_PIN,MOTOR3_PWM_PIN,MOTOR4_PWM_PIN,MOTOR5_PWM_PIN,MOTOR6_PWM_PIN};    //Pin
  
  //  配置CLK(PWM)脚：复用推挽输出 
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_PWM_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif 
}
/*******************************************************************************
* Function Name  : dSPIN_PC_Configuration
* Description    :泵盖检测,浮空输入  
* 输入参数       : 
* Output         : 
* Return         : 
*******************************************************************************/
void dSPIN_PC_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *dSPIN_PORT[6] = {dSPIN_UFP_PC_Port,dSPIN_BPP_PC_Port,dSPIN_SBP_PC_Port,dSPIN_SFP_PC_Port};   //端口
  uint16_t   dSPIN_PINx[6] =    {dSPIN_UFP_PC_Pin,dSPIN_BPP_PC_Pin,dSPIN_SBP_PC_Pin,dSPIN_SFP_PC_Pin};    //Pin
  
  if(dSPIN_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = dSPIN_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(dSPIN_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
}

void dSPIN_Status_Parse(u16 *P,u8 i)
{

  static u8 LOSS[MOTOR_TOTAL] = {0};         //缺相数组
  static u8 LOSS_OK[MOTOR_TOTAL] = {0};
  
  static u8 Thermal[MOTOR_TOTAL] = {0};    //热保护数组
  static u8 Thermal_OK[MOTOR_TOTAL] = {0};
  
  static u8 CMD[MOTOR_TOTAL] = {0};    //通信
  static u8 CMD_OK[MOTOR_TOTAL] = {0};
  
  if(((*P&(dSPIN_STATUS_STEP_LOSS_B|dSPIN_STATUS_STEP_LOSS_A))>>13)!= 0x03)
  {
    LOSS[i]++;                      //缺相次数加加
    if(LOSS[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      LOSS[i] = 0;
      StepMotor_Data[i].NF  = 1;   //A或者B缺相
    }
  }
  else
  {
    LOSS_OK[i]++ ;
    if(LOSS_OK[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      LOSS_OK[i] = 0;
      StepMotor_Data[i].NF  = 0;
    }
  }
  
  if(((*P&(dSPIN_STATUS_OCD|dSPIN_STATUS_TH_SD|dSPIN_STATUS_TH_WRN))>>10) != 0x07)
  {
    Thermal[i]++; 
    Thermal_OK[i] = 0;
    if(Thermal[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      Thermal[i] = 0;
      StepMotor_Data[i].Alert  = 1;   //过流，热停机，热警告
    }
  }
  else
  {
    Thermal_OK[i]++ ;
    Thermal[i] = 0;
    if(Thermal_OK[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      Thermal_OK[i] = 0;
      StepMotor_Data[i].Alert  = 0;
    }
  }
  if(((*P&(dSPIN_STATUS_NOTPERF_CMD|dSPIN_STATUS_WRONG_CMD))>>7) == 0x00)
  {
    CMD[i]++;                
    if(CMD[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      CMD[i] = 0;
      StepMotor_Data[i].CMD  = 0;   
    }
  }
  else
  {
    CMD_OK[i]++ ;
    if(CMD_OK[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      CMD_OK[i] = 0;
      StepMotor_Data[i].CMD  = 1;//SPI命令不能被执行，或者不存在
    }
  }
   

}
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Pin_Configuration
* Description    : 读取强制制动(泵盖等)检测引脚信号，把其状态值写入结构的Braking变量中
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void dSPIN_PC_Read(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN 
  GPIO_TypeDef *StepMotor_PORT[5]  = {dSPIN_UFP_PC_Port,dSPIN_BPP_PC_Port,dSPIN_SBP_PC_Port,dSPIN_SFP_PC_Port};   //端口
  uint16_t   Motor_BRAKING_PINx[5] = {dSPIN_UFP_PC_Pin,dSPIN_BPP_PC_Pin,dSPIN_SBP_PC_Pin,dSPIN_SFP_PC_Pin};    //Pin
 
  static u16 cnt[5]={0}; //去抖计数器
  u8 pinval;
  if(StepMotor_PORT[Motorx])  //强制制动(泵盖等)检测引脚 有效
  {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_BRAKING_PINx[Motorx]);
      if(MOTOR_BRAKING_INVERTER == 1) //泵盖打开时,GPIO端为高电平
      {
        if(pinval != StepMotor_Data[Motorx].Braking)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].Braking = pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      } 
      else  //泵盖打开时,GPIO端为低电平
      {
         if(pinval == StepMotor_Data[Motorx].Braking)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].Braking = !pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      }
  }
#endif

}

/******************************************************************************
函数名:    StepMotor_Set_DCY
函数原形:  void StepMotor_Set_DCY(void)
功能描述:  根据转速(单位:1rpm)设置DCY1,DCY2的电平值          
输入参数:  Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
            speed - 转速(单位:0.1RPM)
输出参数:  无 
返回值:    --
******************************************************************************/
void StepMotor_Set_DCY(StepMotor_TypeDef Motorx,u16 speed)
{
#ifdef MOTOR_SPEED_EN
          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(16+0.25*speed);  //保持占空比
          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(16+0.25*speed);
          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(16+0.25*speed);  //加速占空比
          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(16+0.25*speed);
          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(2000); //1000mA的失速门槛设置
          dSPIN_Registers_Set(Motorx);
//          Delay(1000);
//      if(speed < SPEED_VAL1)  
//        {
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(18);  //保持占空比
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(18);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(18);  //加速占空比
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(18);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA的失速门槛设置
//          dSPIN_Registers_Set(Motorx);
//        }
//      else if(speed < SPEED_VAL2) 
//        { 
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(25);  //保持占空比
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(25);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(25);  //加速占空比
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(25);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA的失速门槛设置
//          dSPIN_Registers_Set(Motorx);
//        }
//      else if( speed < SPEED_VAL3) 
//        { 
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(35);  //保持占空比
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(35);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(35);  //加速占空比
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(35);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA的失速门槛设置
//          dSPIN_Registers_Set(Motorx);
//        }
//      else if(speed < SPEED_VAL4)  
//        { 
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(38);  //保持占空比
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(38);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(38);  //加速占空比
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(38);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA的失速门槛设置
//          dSPIN_Registers_Set(Motorx);
//        }
    

   
#endif  
}
/******************************************************************************
函数名:    StepMotor_Speed_ADJ
函数原形:  void StepMotor_Speed_ADJ(void)
功能描述:  根据转速(0.1rpm)设置DCY1,DCY2的电平值          
输入参数:  Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
            Turn_speed - 转速(单位:1RPM)
输出参数:  无 
返回值:    --
******************************************************************************/
void StepMotor_Speed_ADJ(StepMotor_TypeDef Motorx,float Turn_speed)   
{
 uint32_t Per,div;
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //各泵使用的TIMx
  u8   TIM_CHx[6] =              {MOTOR1_CH,MOTOR2_CH,MOTOR3_CH,MOTOR4_CH,MOTOR5_CH,MOTOR6_CH};    //各泵使用的TIM的CHx
 
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;

 if(StepMotor_Tim[Motorx] && TIM_CHx[Motorx])  //TIM及CH号有效
 {     
      StepMotor_Set_DCY(Motorx,(u16)Turn_speed);
      
     /*************转速与频率输出设置换算关系****************
      *已知条件有：细分度=64;步距角=1.8度;SetSpeed为转速(fload型),单位为rpm;PWM的TIM的工作时钟为72Mhz则有：
      *   每秒的转速度为：SetSpeed/60(r/s) 
      *   电机转一圈所需要的脉冲数为: (360度/1.8度)*64=200*64=12800(脉冲数/r)   
      *则一定转速下的脉冲频率为: Frequency=(SetSpeed/60)*12800 (脉冲数/s) 
      *  得:TIM_Period=72000000/Frequency = 72000000*60/(12800*SetSpeed)=337500/SetSpeed
     ******************************************************/
     Per=(uint32_t)(STEPMOTOR_COEFFICIENT/Turn_speed+0.5); 
     if(Per<=65535) //转速小于5.15(r/min) 
      {  
        TIM_TimeBaseStructure.TIM_Period =Per-1;   //自动重装载值-PWM周期          
        TIM_TimeBaseStructure.TIM_Prescaler =0;    //不分频
        TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;      //时钟倍频因子              
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
      }
      else        //转速大于5.15(r/min) 
      {  
       // Per=(uint32_t)((Per/1000.0)+0.5); 
        div = (Per >> 16);  //分频值 
        Per = Per/(div+1);
        TIM_TimeBaseStructure.TIM_Period =Per-1;    //自动重装载值-PWM周期         
        TIM_TimeBaseStructure.TIM_Prescaler = div;   //分频 (分频系数-1)
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //时钟倍频因子，输出时钟供给死区发生器等
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
      }
      TIM_TimeBaseInit(StepMotor_Tim[Motorx], &TIM_TimeBaseStructure); 
        //配置 CHx的PWM模式  
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //设置PWM输出模式
      TIM_OCInitStructure.TIM_Pulse = Per/2;    //设置占空比
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能该通道输出
      TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //高级定时器-使能互补端输出
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //设置输出极性
      TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;      //高级定时器-设置互补端输出极性
      TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;       //高级定时器-死区后输出状态
      TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;    //高级定时器-死区后互补端输出状态
      
      switch(TIM_CHx[Motorx]) //选通PWM相应的CHx通道
      {
        case 1:
            TIM_OC1Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道1使能
        break;
        case 2:
            TIM_OC2Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道2使能
        break;
        case 3:
            TIM_OC3Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道3使能
        break;
        case 4:
            TIM_OC4Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道4使能
        break;
      }
 }
}
/*******************************************************************************
* Function Name  : StepMotor_TIM_Cmd
* Description    : 泵使用的TIM控制(使能或失能)
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
*                  NewState - 控制命令：ENABLE或DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TIM_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState)   
{
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //各泵使用的TIMx
// u8  StepMotor_Tim_IRQ[6] = {MOTOR1_TIM_IRQ,MOTOR2_TIM_IRQ,MOTOR3_TIM_IRQ,MOTOR4_TIM_IRQ,MOTOR5_TIM_IRQ,MOTOR6_TIM_IRQ};   //1=中断有效
 
 if(StepMotor_Tim[Motorx])
  {  
     TIM_Cmd(StepMotor_Tim[Motorx],NewState);
     TIM_CtrlPWMOutputs(StepMotor_Tim[Motorx],NewState);
     
#ifdef MOTOR_PWM_IRQ_EN   //配置PWM的TIM中断有效，主要用于流量脉冲累计

//    if(1==Motorx)
//    {
//      TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
//      TIM_ITConfig(StepMotor_Tim[Motorx], TIM_IT_Update, NewState);
//    }
//    else
//    {
      TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
      TIM_ITConfig(StepMotor_Tim[Motorx], TIM_IT_Update, NewState);
//    }


#endif
  }
}

/*******************************************************************************
* Function Name  : StepMotor_PWM_TIM_IRQ
* Description    : PWM定时器中断处理程序
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
//void StepMotor_PWM_TIM_IRQ(StepMotor_TypeDef Motorx)   
//{
//  static u16 pwmpulse_real[MOTOR_TOTAL] = {0};
//  u8 ENCODER[6] = {MOTOR1_ENCODER,MOTOR2_ENCODER,MOTOR3_ENCODER,MOTOR4_ENCODER,MOTOR5_ENCODER,MOTOR6_ENCODER};  
//  u16 pulse_encoder;
//#ifdef MOTOR_PWM_IRQ_EN   //配置PWM的TIM中断有效，主要用于流量脉冲累计
//  TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //各泵使用的TIMx
//
// if (TIM_GetITStatus(StepMotor_Tim[Motorx], TIM_IT_Update) != RESET)
// { 
//    TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
//    StepMotor_Data[Motorx].Flow++;      //流量脉累计
//#ifdef MOTOR_PWM_LOCKROTOR_CHECK_EN        //判断是否堵转
//    StepMotor_Data[Motorx].FlowLockRotor++;
//    pulse_encoder = STEPMOTOR_PWMPULSE / ENCODER[Motorx];  //每一个旋转编码器脉冲对应的PWM发出脉冲数 = 12800/齿数
//    if(StepMotor_Data[Motorx].FlowLockRotor > (pulse_encoder + (pulse_encoder >> 1))) //超出每齿脉冲数的50%后判断一次是否堵转
//    { //步时脉冲个数超出 标准数量的1.5倍后，判断是否有堵转
//      if(StepMotor_Data[Motorx].LockRotorSpeed == 0)
//      {
//        StepMotor_Data[Motorx].LockRotor = 1; //堵转
//      }
//      else
//      {
//        StepMotor_Data[Motorx].LockRotor = 0; //未堵转
//      }
//        StepMotor_Data[Motorx].FlowLockRotor = 0;
//        StepMotor_Data[Motorx].LockRotorSpeed = 0;
//    }
//#endif  
// } 
//#endif
//}
u16  dSPIN_data[5] = {0};
void dSPIN_Work()
{
  u8 mi;
  u8 adjsta;    //速度调节标志变量
  u8 clrsta;
  clrsta = 0;   //是否清0调速间隔时间计数器的标志位,6个泵共用
  for(mi = 0; mi < MOTOR_TOTAL; mi++)   //轮询各个泵
  {
    Type = mi;
    dSPIN_data[mi] = dSPIN_Get_Status();
    adjsta = 0;
    if(StepMotor_Data[mi].Enable == DISABLE) //关机命令
    {

      StepMotor_Data[mi].CurrSpeed = 0;
      StepMotor_TIM_Cmd((StepMotor_TypeDef)mi,DISABLE);     
      dSPIN_Hard_HiZ();

    }
    if(StepMotor_Data[mi].Enable == ENABLE)
    {
      
      if((StepMotor_Data[mi].CurrSpeed != StepMotor_Data[mi].SetSpeed)&&(StepMotor_ADJ_Cnt > STEPMOTOR_ADJSPEED_SPACE))   
      { 
        if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //升速
        {
          adjsta = 1;
           StepMotor_Data[mi].CurrSpeed += STEPMOTOR_ADJSPEED_STEP;
           if(StepMotor_Data[mi].CurrSpeed > StepMotor_Data[mi].SetSpeed)  //递增到设定值
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed;
           }
        }
        else //降速
        {
           adjsta = 1;   
           if(StepMotor_Data[mi].CurrSpeed >= STEPMOTOR_ADJSPEED_STEP) 
           {
             StepMotor_Data[mi].CurrSpeed -= STEPMOTOR_ADJSPEED_STEP;   //递减
           }
           else 
           {
              StepMotor_Data[mi].CurrSpeed = 0;     //递减到0
           }
           if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //递减到设定值
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed; 
           }
        } 
        if(adjsta == 1)  //本组泵参数扫描中，有泵速度调节
         {
           StepMotor_Speed_ADJ((StepMotor_TypeDef)mi, StepMotor_Data[mi].CurrSpeed);
           StepMotor_TIM_Cmd((StepMotor_TypeDef)mi,ENABLE);
           dSPIN_Registers_Set(mi);
//           dSPIN_Step_Clock(REV);
           clrsta = 1;
         }
      }
    }
    
  }  
  if(clrsta == 1)
  {
    StepMotor_ADJ_Cnt = 0;   //6个泵中，有任意一个泵执行了速度调节，则清0计数器
  }
}

/*******************************************************************************
* Function Name  : StepMotor_EXTI_IRQ
* Description    : 速度反饋信號的EXTI中斷處理程序
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_EXTI_IRQ(StepMotor_TypeDef Motorx)   
{
#ifdef MOTOR_REALSPEED_CHECK_EN    
  uint32_t    EXTI_Linex[6]  =  {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin
  
  if(EXTI_GetITStatus(EXTI_Linex[Motorx])!=RESET)//判断某个线上的中断是否发生 
  { 
    EXTI_ClearITPendingBit(EXTI_Linex[Motorx]);  //清除 LINE 上的中断标志位    
    StepMotor_Data[Motorx].LockRotorSpeed++;     //累计编码器转速脉冲个数 - 用于堵转检测  
    StepMotor_Data[Motorx].RealPulse++;          //累计编码器转速脉冲个数 - 用于计算真实转速   
  }   
#endif 
}

void Delay_Bus(u32 dly)
{
  u8 i;
  while(dly--)
  {
    i = 8;
    while(--i);
  }
}

void StepMotor_Stop_Turn(StepMotor_TypeDef Motorx)   
{
  u8 ENCODER[6] = {MOTOR1_ENCODER,MOTOR2_ENCODER,MOTOR3_ENCODER,MOTOR4_ENCODER,MOTOR5_ENCODER,MOTOR6_ENCODER};  
#ifdef MOTOR_Stop_Turn  //（圈数*霍尔数-霍尔脉冲数）>2
//    TEMP_Circle[Motorx] = ((StepMotor_Data[Motorx].Circle/100)*ENCODER[Motorx]+2)-StepMotor_Data[Motorx].RealPulse;
    
  
    if(((StepMotor_Data[Motorx].Circle/100)-TEMP_Circle[Motorx])>2)//记录3圈时反馈脉冲数
    {
      TEMP_Circle[Motorx] = (StepMotor_Data[Motorx].Circle/100);  //圈数变化时
      if((StepMotor_Data[Motorx].RealPulse-TEMP_Pulse[Motorx])>=ENCODER[Motorx])
      {
       StepMotor_Data[Motorx].LockRotor = 0; //未堵转 
      }
      else
      {
        StepMotor_Data[Motorx].LockRotor = 1; //堵转
      }
      TEMP_Pulse[Motorx] = StepMotor_Data[Motorx].RealPulse;
      
      
    }
//    if(abs(TEMP_Circle[Motorx])>6) //超出每齿脉冲数的50%后判断一次是否堵转
//    { 
//      StepMotor_Data[Motorx].LockRotor = 1; //堵转
//           
//    }
//    else
//    {
//      StepMotor_Data[Motorx].LockRotor = 0; //未堵转
//    }

#endif
}

