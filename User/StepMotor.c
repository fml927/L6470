#include "stm32f10x.h"
#include "StepMotor.h"
#include "dspin.h"
#include "flag.h"
u32 StepMotor_ADJ_Cnt = 0;   //����PWMƵ��ʱ��������
u32 TEMP_Circle[5] = {0};   //�ж϶�תʱ��ʱȦ��
u32 TEMP_Pulse[5] = {0};    //�ж϶�תʱ��ʱ����
StepMotor_WorkInfo_Def StepMotor_Data[5];


/*******************************************************************************
* Function Name  : StepMotor_REAL_Pin_EXTI_Configuration
* Description    : ���ö�ת����GPIO����EXTI�ж�
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_REAL_Pin_EXTI_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_REAL_PORT,MOTOR2_REAL_PORT,MOTOR3_REAL_PORT,MOTOR4_REAL_PORT,MOTOR5_REAL_PORT,MOTOR6_REAL_PORT};   //�˿�
  uint16_t    Motor_REAL_PINx[6] =  {MOTOR1_REAL_PIN,MOTOR2_REAL_PIN,MOTOR3_REAL_PIN,MOTOR4_REAL_PIN,MOTOR5_REAL_PIN,MOTOR6_REAL_PIN};    //Pin
  uint16_t    Source_PORT[6] =     {MOTOR1_REAL_SourcePORT,MOTOR2_REAL_SourcePORT,MOTOR3_REAL_SourcePORT,MOTOR4_REAL_SourcePORT,MOTOR5_REAL_SourcePORT,MOTOR6_REAL_SourcePORT};   //�˿�
  uint16_t    Source_PINx[6] =     {MOTOR1_REAL_SourcePIN,MOTOR2_REAL_SourcePIN,MOTOR3_REAL_SourcePIN,MOTOR4_REAL_SourcePIN,MOTOR5_REAL_SourcePIN,MOTOR6_REAL_SourcePIN};    //Pin
  uint32_t    EXTI_Linex[6]  =     {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin
 
   // ����REAL�ţ���������  
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
  for(i = 0; i < MOTOR_TOTAL; i++)  //��չ���״̬
  {
    StepMotor_Data[i].Enable = DISABLE;
    StepMotor_Data[i].Status = DISABLE;  //����Ӧ��ʼ��ΪDISABLE�������ʼ��ΪENABLE��Ŀ�����ô�������ͷŵ��
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
* Description    : Ƭѡ
* �������       : 
* Output         : 
* Return         : 
*******************************************************************************/
void dSPIN_CS_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *dSPIN_PORT[5] = {dSPIN_UFP_CS_Port,dSPIN_BPP_CS_Port,dSPIN_SBP_CS_Port,dSPIN_SFP_CS_Port,dSPIN_HP_CS_Port};   //�˿�
  uint16_t   dSPIN_PINx[5] =    {dSPIN_UFP_CS_Pin,dSPIN_BPP_CS_Pin,dSPIN_SBP_CS_Pin,dSPIN_SFP_CS_Pin,dSPIN_HP_CS_Pin};    //Pin
  
  //������� 
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
* �������       : 
* Output         : 
* Return         : 
*******************************************************************************/
void dSPIN_FLAG_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *dSPIN_PORT[5] = {dSPIN_UFP_FLAG_Port,dSPIN_BPP_FLAG_Port,dSPIN_SBP_FLAG_Port,dSPIN_SFP_FLAG_Port,dSPIN_HP_FLAG_Port};   //�˿�
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
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_PWM_PORT,MOTOR2_PWM_PORT,MOTOR3_PWM_PORT,MOTOR4_PWM_PORT,MOTOR5_PWM_PORT,MOTOR6_PWM_PORT};   //�˿�
  uint16_t   Motor_PWM_PINx[6] =    {MOTOR1_PWM_PIN,MOTOR2_PWM_PIN,MOTOR3_PWM_PIN,MOTOR4_PWM_PIN,MOTOR5_PWM_PIN,MOTOR6_PWM_PIN};    //Pin
  
  //  ����CLK(PWM)�ţ������������ 
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
* Description    :�øǼ��,��������  
* �������       : 
* Output         : 
* Return         : 
*******************************************************************************/
void dSPIN_PC_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *dSPIN_PORT[6] = {dSPIN_UFP_PC_Port,dSPIN_BPP_PC_Port,dSPIN_SBP_PC_Port,dSPIN_SFP_PC_Port};   //�˿�
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

  static u8 LOSS[MOTOR_TOTAL] = {0};         //ȱ������
  static u8 LOSS_OK[MOTOR_TOTAL] = {0};
  
  static u8 Thermal[MOTOR_TOTAL] = {0};    //�ȱ�������
  static u8 Thermal_OK[MOTOR_TOTAL] = {0};
  
  static u8 CMD[MOTOR_TOTAL] = {0};    //ͨ��
  static u8 CMD_OK[MOTOR_TOTAL] = {0};
  
  if(((*P&(dSPIN_STATUS_STEP_LOSS_B|dSPIN_STATUS_STEP_LOSS_A))>>13)!= 0x03)
  {
    LOSS[i]++;                      //ȱ������Ӽ�
    if(LOSS[i] > STEPMOTOR_IN_DEBOUNCE)
    {
      LOSS[i] = 0;
      StepMotor_Data[i].NF  = 1;   //A����Bȱ��
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
      StepMotor_Data[i].Alert  = 1;   //��������ͣ�����Ⱦ���
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
      StepMotor_Data[i].CMD  = 1;//SPI����ܱ�ִ�У����߲�����
    }
  }
   

}
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Pin_Configuration
* Description    : ��ȡǿ���ƶ�(�øǵ�)��������źţ�����״ֵ̬д��ṹ��Braking������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void dSPIN_PC_Read(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN 
  GPIO_TypeDef *StepMotor_PORT[5]  = {dSPIN_UFP_PC_Port,dSPIN_BPP_PC_Port,dSPIN_SBP_PC_Port,dSPIN_SFP_PC_Port};   //�˿�
  uint16_t   Motor_BRAKING_PINx[5] = {dSPIN_UFP_PC_Pin,dSPIN_BPP_PC_Pin,dSPIN_SBP_PC_Pin,dSPIN_SFP_PC_Pin};    //Pin
 
  static u16 cnt[5]={0}; //ȥ��������
  u8 pinval;
  if(StepMotor_PORT[Motorx])  //ǿ���ƶ�(�øǵ�)������� ��Ч
  {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_BRAKING_PINx[Motorx]);
      if(MOTOR_BRAKING_INVERTER == 1) //�øǴ�ʱ,GPIO��Ϊ�ߵ�ƽ
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
      else  //�øǴ�ʱ,GPIO��Ϊ�͵�ƽ
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
������:    StepMotor_Set_DCY
����ԭ��:  void StepMotor_Set_DCY(void)
��������:  ����ת��(��λ:1rpm)����DCY1,DCY2�ĵ�ƽֵ          
�������:  Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
            speed - ת��(��λ:0.1RPM)
�������:  �� 
����ֵ:    --
******************************************************************************/
void StepMotor_Set_DCY(StepMotor_TypeDef Motorx,u16 speed)
{
#ifdef MOTOR_SPEED_EN
          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(16+0.25*speed);  //����ռ�ձ�
          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(16+0.25*speed);
          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(16+0.25*speed);  //����ռ�ձ�
          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(16+0.25*speed);
          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(2000); //1000mA��ʧ���ż�����
          dSPIN_Registers_Set(Motorx);
//          Delay(1000);
//      if(speed < SPEED_VAL1)  
//        {
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(18);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(18);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(18);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(18);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA��ʧ���ż�����
//          dSPIN_Registers_Set(Motorx);
//        }
//      else if(speed < SPEED_VAL2) 
//        { 
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(25);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(25);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(25);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(25);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA��ʧ���ż�����
//          dSPIN_Registers_Set(Motorx);
//        }
//      else if( speed < SPEED_VAL3) 
//        { 
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(35);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(35);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(35);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(35);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA��ʧ���ż�����
//          dSPIN_Registers_Set(Motorx);
//        }
//      else if(speed < SPEED_VAL4)  
//        { 
//          dSPIN_RegsStruct[Motorx].KVAL_HOLD = Kval_Perc_to_Par(38);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_RUN = Kval_Perc_to_Par(38);
//          dSPIN_RegsStruct[Motorx].KVAL_ACC = Kval_Perc_to_Par(38);  //����ռ�ձ�
//          dSPIN_RegsStruct[Motorx].KVAL_DEC = Kval_Perc_to_Par(38);
//          dSPIN_RegsStruct[Motorx].STALL_TH = StallTh_to_Par(3000); //1000mA��ʧ���ż�����
//          dSPIN_Registers_Set(Motorx);
//        }
    

   
#endif  
}
/******************************************************************************
������:    StepMotor_Speed_ADJ
����ԭ��:  void StepMotor_Speed_ADJ(void)
��������:  ����ת��(0.1rpm)����DCY1,DCY2�ĵ�ƽֵ          
�������:  Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
            Turn_speed - ת��(��λ:1RPM)
�������:  �� 
����ֵ:    --
******************************************************************************/
void StepMotor_Speed_ADJ(StepMotor_TypeDef Motorx,float Turn_speed)   
{
 uint32_t Per,div;
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //����ʹ�õ�TIMx
  u8   TIM_CHx[6] =              {MOTOR1_CH,MOTOR2_CH,MOTOR3_CH,MOTOR4_CH,MOTOR5_CH,MOTOR6_CH};    //����ʹ�õ�TIM��CHx
 
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;

 if(StepMotor_Tim[Motorx] && TIM_CHx[Motorx])  //TIM��CH����Ч
 {     
      StepMotor_Set_DCY(Motorx,(u16)Turn_speed);
      
     /*************ת����Ƶ��������û����ϵ****************
      *��֪�����У�ϸ�ֶ�=64;�����=1.8��;SetSpeedΪת��(fload��),��λΪrpm;PWM��TIM�Ĺ���ʱ��Ϊ72Mhz���У�
      *   ÿ���ת�ٶ�Ϊ��SetSpeed/60(r/s) 
      *   ���תһȦ����Ҫ��������Ϊ: (360��/1.8��)*64=200*64=12800(������/r)   
      *��һ��ת���µ�����Ƶ��Ϊ: Frequency=(SetSpeed/60)*12800 (������/s) 
      *  ��:TIM_Period=72000000/Frequency = 72000000*60/(12800*SetSpeed)=337500/SetSpeed
     ******************************************************/
     Per=(uint32_t)(STEPMOTOR_COEFFICIENT/Turn_speed+0.5); 
     if(Per<=65535) //ת��С��5.15(r/min) 
      {  
        TIM_TimeBaseStructure.TIM_Period =Per-1;   //�Զ���װ��ֵ-PWM����          
        TIM_TimeBaseStructure.TIM_Prescaler =0;    //����Ƶ
        TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;      //ʱ�ӱ�Ƶ����              
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
      }
      else        //ת�ٴ���5.15(r/min) 
      {  
       // Per=(uint32_t)((Per/1000.0)+0.5); 
        div = (Per >> 16);  //��Ƶֵ 
        Per = Per/(div+1);
        TIM_TimeBaseStructure.TIM_Period =Per-1;    //�Զ���װ��ֵ-PWM����         
        TIM_TimeBaseStructure.TIM_Prescaler = div;   //��Ƶ (��Ƶϵ��-1)
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //ʱ�ӱ�Ƶ���ӣ����ʱ�ӹ���������������
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
      }
      TIM_TimeBaseInit(StepMotor_Tim[Motorx], &TIM_TimeBaseStructure); 
        //���� CHx��PWMģʽ  
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //����PWM���ģʽ
      TIM_OCInitStructure.TIM_Pulse = Per/2;    //����ռ�ձ�
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //ʹ�ܸ�ͨ�����
      TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //�߼���ʱ��-ʹ�ܻ��������
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //�����������
      TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;      //�߼���ʱ��-���û������������
      TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;       //�߼���ʱ��-���������״̬
      TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;    //�߼���ʱ��-�����󻥲������״̬
      
      switch(TIM_CHx[Motorx]) //ѡͨPWM��Ӧ��CHxͨ��
      {
        case 1:
            TIM_OC1Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��1ʹ��
        break;
        case 2:
            TIM_OC2Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��2ʹ��
        break;
        case 3:
            TIM_OC3Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��3ʹ��
        break;
        case 4:
            TIM_OC4Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��4ʹ��
        break;
      }
 }
}
/*******************************************************************************
* Function Name  : StepMotor_TIM_Cmd
* Description    : ��ʹ�õ�TIM����(ʹ�ܻ�ʧ��)
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
*                  NewState - �������ENABLE��DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TIM_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState)   
{
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //����ʹ�õ�TIMx
// u8  StepMotor_Tim_IRQ[6] = {MOTOR1_TIM_IRQ,MOTOR2_TIM_IRQ,MOTOR3_TIM_IRQ,MOTOR4_TIM_IRQ,MOTOR5_TIM_IRQ,MOTOR6_TIM_IRQ};   //1=�ж���Ч
 
 if(StepMotor_Tim[Motorx])
  {  
     TIM_Cmd(StepMotor_Tim[Motorx],NewState);
     TIM_CtrlPWMOutputs(StepMotor_Tim[Motorx],NewState);
     
#ifdef MOTOR_PWM_IRQ_EN   //����PWM��TIM�ж���Ч����Ҫ�������������ۼ�

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
* Description    : PWM��ʱ���жϴ������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
//void StepMotor_PWM_TIM_IRQ(StepMotor_TypeDef Motorx)   
//{
//  static u16 pwmpulse_real[MOTOR_TOTAL] = {0};
//  u8 ENCODER[6] = {MOTOR1_ENCODER,MOTOR2_ENCODER,MOTOR3_ENCODER,MOTOR4_ENCODER,MOTOR5_ENCODER,MOTOR6_ENCODER};  
//  u16 pulse_encoder;
//#ifdef MOTOR_PWM_IRQ_EN   //����PWM��TIM�ж���Ч����Ҫ�������������ۼ�
//  TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //����ʹ�õ�TIMx
//
// if (TIM_GetITStatus(StepMotor_Tim[Motorx], TIM_IT_Update) != RESET)
// { 
//    TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
//    StepMotor_Data[Motorx].Flow++;      //�������ۼ�
//#ifdef MOTOR_PWM_LOCKROTOR_CHECK_EN        //�ж��Ƿ��ת
//    StepMotor_Data[Motorx].FlowLockRotor++;
//    pulse_encoder = STEPMOTOR_PWMPULSE / ENCODER[Motorx];  //ÿһ����ת�����������Ӧ��PWM���������� = 12800/����
//    if(StepMotor_Data[Motorx].FlowLockRotor > (pulse_encoder + (pulse_encoder >> 1))) //����ÿ����������50%���ж�һ���Ƿ��ת
//    { //��ʱ����������� ��׼������1.5�����ж��Ƿ��ж�ת
//      if(StepMotor_Data[Motorx].LockRotorSpeed == 0)
//      {
//        StepMotor_Data[Motorx].LockRotor = 1; //��ת
//      }
//      else
//      {
//        StepMotor_Data[Motorx].LockRotor = 0; //δ��ת
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
  u8 adjsta;    //�ٶȵ��ڱ�־����
  u8 clrsta;
  clrsta = 0;   //�Ƿ���0���ټ��ʱ��������ı�־λ,6���ù���
  for(mi = 0; mi < MOTOR_TOTAL; mi++)   //��ѯ������
  {
    Type = mi;
    dSPIN_data[mi] = dSPIN_Get_Status();
    adjsta = 0;
    if(StepMotor_Data[mi].Enable == DISABLE) //�ػ�����
    {

      StepMotor_Data[mi].CurrSpeed = 0;
      StepMotor_TIM_Cmd((StepMotor_TypeDef)mi,DISABLE);     
      dSPIN_Hard_HiZ();

    }
    if(StepMotor_Data[mi].Enable == ENABLE)
    {
      
      if((StepMotor_Data[mi].CurrSpeed != StepMotor_Data[mi].SetSpeed)&&(StepMotor_ADJ_Cnt > STEPMOTOR_ADJSPEED_SPACE))   
      { 
        if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //����
        {
          adjsta = 1;
           StepMotor_Data[mi].CurrSpeed += STEPMOTOR_ADJSPEED_STEP;
           if(StepMotor_Data[mi].CurrSpeed > StepMotor_Data[mi].SetSpeed)  //�������趨ֵ
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed;
           }
        }
        else //����
        {
           adjsta = 1;   
           if(StepMotor_Data[mi].CurrSpeed >= STEPMOTOR_ADJSPEED_STEP) 
           {
             StepMotor_Data[mi].CurrSpeed -= STEPMOTOR_ADJSPEED_STEP;   //�ݼ�
           }
           else 
           {
              StepMotor_Data[mi].CurrSpeed = 0;     //�ݼ���0
           }
           if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //�ݼ����趨ֵ
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed; 
           }
        } 
        if(adjsta == 1)  //����ò���ɨ���У��б��ٶȵ���
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
    StepMotor_ADJ_Cnt = 0;   //6�����У�������һ����ִ�����ٶȵ��ڣ�����0������
  }
}

/*******************************************************************************
* Function Name  : StepMotor_EXTI_IRQ
* Description    : �ٶȷ�����̖��EXTI�Д�̎�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_EXTI_IRQ(StepMotor_TypeDef Motorx)   
{
#ifdef MOTOR_REALSPEED_CHECK_EN    
  uint32_t    EXTI_Linex[6]  =  {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin
  
  if(EXTI_GetITStatus(EXTI_Linex[Motorx])!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
  { 
    EXTI_ClearITPendingBit(EXTI_Linex[Motorx]);  //��� LINE �ϵ��жϱ�־λ    
    StepMotor_Data[Motorx].LockRotorSpeed++;     //�ۼƱ�����ת��������� - ���ڶ�ת���  
    StepMotor_Data[Motorx].RealPulse++;          //�ۼƱ�����ת��������� - ���ڼ�����ʵת��   
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
#ifdef MOTOR_Stop_Turn  //��Ȧ��*������-������������>2
//    TEMP_Circle[Motorx] = ((StepMotor_Data[Motorx].Circle/100)*ENCODER[Motorx]+2)-StepMotor_Data[Motorx].RealPulse;
    
  
    if(((StepMotor_Data[Motorx].Circle/100)-TEMP_Circle[Motorx])>2)//��¼3Ȧʱ����������
    {
      TEMP_Circle[Motorx] = (StepMotor_Data[Motorx].Circle/100);  //Ȧ���仯ʱ
      if((StepMotor_Data[Motorx].RealPulse-TEMP_Pulse[Motorx])>=ENCODER[Motorx])
      {
       StepMotor_Data[Motorx].LockRotor = 0; //δ��ת 
      }
      else
      {
        StepMotor_Data[Motorx].LockRotor = 1; //��ת
      }
      TEMP_Pulse[Motorx] = StepMotor_Data[Motorx].RealPulse;
      
      
    }
//    if(abs(TEMP_Circle[Motorx])>6) //����ÿ����������50%���ж�һ���Ƿ��ת
//    { 
//      StepMotor_Data[Motorx].LockRotor = 1; //��ת
//           
//    }
//    else
//    {
//      StepMotor_Data[Motorx].LockRotor = 0; //δ��ת
//    }

#endif
}

