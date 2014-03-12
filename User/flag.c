#include "stm32f10x.h"
#include "flag.h"
#include "swsbus.h"
#include "hp.h"
#include "dspin.h"
#include "StepMotor.h"
#include "AD7799.h"
#include "stdlib.h"
#include "flash.h"
#include "main.h"






u8 Cycle[MOTOR_TOTAL] = {0};  //ABS_POS�Ĵ����������
u32 Old_Circle[MOTOR_TOTAL] = {0};  //�ϴ�Ȧ���ݴ�

Flow Integrated[5]={0};   //�ۼ������ṹ��

u8  Type=0;              //��ѯ����ı���

u16  dSPIN_rx_data[5] = {0};  //��ȡL6470�Ĵ���״̬

u16 Motor_Time = 0;    //���Լ�ʱ���ʱ

u8  SWITCH = 0;       //WR0 01�����Լ� 10���б��Լ� 11 ���бü�ע�����Լ�
u8  SWITCH_SFP = 0;   //�������(0:��,1:��) ���ܱ�
u8  SWITCH_SBP = 0;  //��Һ��
u8  SWITCH_UFP = 0;  //���˱�
u8  SWITCH_BPP = 0;  //Ѫ����
u8  SWITCH_HP = 0;    //���ر�

void dSPIN_Cycles_Speed(StepMotor_TypeDef Motorx)
{
  if((StepMotor_Data[Motorx].Enable == 1)&&(dSPIN_Get_Param(dSPIN_ABS_POS)>0))                         //���ת�ٶ�ȡ
  {     
    
    if(StepMotor_Data[Motorx].SetDir == 1)   //��ת����Ȧ��
    {
      StepMotor_Data[Motorx].Step  = dSPIN_Get_Param(dSPIN_ABS_POS);
    }
    else
    {
      StepMotor_Data[Motorx].Step  = 0x400000-dSPIN_Get_Param(dSPIN_ABS_POS);
    }   
    StepMotor_Data[Motorx].Temp_Circle = StepMotor_Data[Motorx].Step/STEPMOTOR_PULSE; //Ȧ���Ŵ�100��
    if(Old_Circle[Motorx]!=StepMotor_Data[Motorx].Temp_Circle)
    {          
      if(Old_Circle[Motorx]>StepMotor_Data[Motorx].Temp_Circle)   //�Ĵ������
      {
        Cycle[Motorx]++;
        
      }
      Old_Circle[Motorx] = StepMotor_Data[Motorx].Temp_Circle;   //�ݴ���һ�ε�ֵ
    }
    
    StepMotor_Data[Motorx].Circle = Cycle[Motorx]*16384 + StepMotor_Data[Motorx].Temp_Circle;  //163.84Ȧ�Ĵ������
    
  } 
  if(StepMotor_Data[Motorx].Enable ==1)
  {
    StepMotor_Data[Motorx].ReadSpeed = (float)dSPIN_Get_Param(dSPIN_SPEED)/224;   //��ȡת��
  }
  else
  {
    StepMotor_Data[Motorx].ReadSpeed = 0;  //ͣ���ʱ���ٶ���0
  }
}

void Down_flag()
{
  if(1==NEW_CMD)       //������Ϣ��ռδʹ��
  {
    NEW_CMD = 0;
    SWITCH = BUS_Buffer.WR0[1];
    if(1 == SWITCH)      //�����Լ�,�ر�,������0
    {
      for(u8 i = 0; i<5; i++)
      {
        Type = i;
        StepMotor_Data[i].Enable = DISABLE;  
        dSPIN_Hard_HiZ ();
        FlowClearZero(i);
      }
      
    }
    if(3 == SWITCH)      //���Լ�
    {
      
      Motor_Time = 0;
      for(u8 i = 0; i<5; i++)
      {
        Type = i;
        if(Type==HP)
        {                
          FlowClearZero(i);
          StepMotor_Data[HP].Down_Mode = SELF_TEST;
          StepMotor_Data[HP].SetDir = 0;
          StepMotor_Data[HP].Enable = ENABLE;
          dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*HPSELF_Speed/15));  //���ر��Լ��ٶ�
        }
        else
        {
          FlowClearZero(i);
          StepMotor_Data[i].Down_Mode = SELF_TEST;
          StepMotor_Data[i].Enable = ENABLE;
          dSPIN_Run(StepMotor_Data[i].SetDir, Speed_Steps_to_Par(50*SELF_Speed/15));  
        }
        
      }     
    }
    if(2 == SWITCH)   //���رò��Լ�
    {
      Motor_Time = 0;
      for(u8 i = 0; i<(MOTOR_TOTAL-1); i++)
      {
        StepMotor_Data[i].Down_Mode = SELF_TEST;
        StepMotor_Data[i].Enable = ENABLE;
        dSPIN_Run(StepMotor_Data[i].SetDir, Speed_Steps_to_Par(50*SELF_Speed/15));
        
      }
    }
    //    hp_param.correct[0] = BUS_Buffer.WR0[3];
    //    hp_param.correct[1] = BUS_Buffer.WR0[4];
    //    hp_param.correct[2] = BUS_Buffer.WR0[5];
    //    hp_param.correct[3] = BUS_Buffer.WR0[6];    
  }
  
  if(1==NEW_CMD_SFP)     //���ܱ�
  {
    NEW_CMD_SFP = 0;
    SWITCH_SFP = ((0x08 & BUS_Buffer.WR1[1]) >> 3); //�ÿ���
    if(SWITCH_SFP)
    {
      StepMotor_Data[SFP].Enable = ENABLE;  
      StepMotor_Data[SFP].LockRotor = 0; 
    }
    else   //�ر�
    {      
      StepMotor_Data[SFP].Enable = DISABLE; 
    }
    
    StepMotor_Data[SFP].SetDir = ((0x04 & BUS_Buffer.WR1[1]) >> 2); //����ת
    StepMotor_Data[SFP].Down_Mode = (0x03 & BUS_Buffer.WR1[1]); //��תģʽ
    if(SLOW_RUN==StepMotor_Data[SFP].Down_Mode)
    {
      Type = SFP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //�������ת��
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800));      
    }
    else
    {
      Type = SFP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500);
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR1[1]) >> 5))  //0�����ۼƣ�1��������
    {
      StepMotor_Data[SFP].Flow = 0;
      FlowClearZero(SFP);
    }
    StepMotor_Data[SFP].RatioSwitch = ((0x10 & BUS_Buffer.WR1[1]) >> 4); //�ù�ϵ�����Կ��� 
    
    StepMotor_Data[SFP].Ratio = BUS_Buffer.WR1[4]*0x100+BUS_Buffer.WR1[3];  //ml/R
    StepMotor_Data[SFP].SetFlow = BUS_Buffer.WR1[6]*0x100+BUS_Buffer.WR1[5];  //ml/R
    StepMotor_Data[SFP].SetSpeed =  (float)(BUS_Buffer.WR1[6]*0x100+BUS_Buffer.WR1[5])/(float)(BUS_Buffer.WR1[4]*0x100+BUS_Buffer.WR1[3]); // ������ml/min�����Ա�ϵ��
  }
  
  if(1==NEW_CMD_SBP)     //��Һ��
  {
    NEW_CMD_SBP = 0;
    SWITCH_SBP = ((0x08 & BUS_Buffer.WR2[1]) >> 3); //�ÿ���
    if(SWITCH_SBP)
    {
      StepMotor_Data[SBP].Enable = ENABLE; 
      StepMotor_Data[SBP].LockRotor = 0; 
    }
    else   //�ر�
    {
      StepMotor_Data[SBP].Enable = DISABLE;
    }
    StepMotor_Data[SBP].SetDir = ((0x04 & BUS_Buffer.WR2[1]) >> 2); //����ת
    StepMotor_Data[SBP].Down_Mode = (0x03 & BUS_Buffer.WR2[1]); //��תģʽ 
    if(SLOW_RUN==StepMotor_Data[SBP].Down_Mode)
    {
      Type = SBP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //�������ת��
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800));      
    }
    else
    {
      Type = SBP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500);
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR2[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[SBP].Flow = 0;
      FlowClearZero(SBP);
    }
    StepMotor_Data[SBP].RatioSwitch = ((0x10 & BUS_Buffer.WR2[1]) >> 4); //�ù�ϵ�����Կ���   
    
    StepMotor_Data[SBP].Ratio = BUS_Buffer.WR2[4]*0x100+BUS_Buffer.WR2[3];  //ml/R
    StepMotor_Data[SBP].SetFlow = BUS_Buffer.WR2[6]*0x100+BUS_Buffer.WR2[5];  //ml/R
    StepMotor_Data[SBP].SetSpeed =  (float)(BUS_Buffer.WR2[6]*0x100+BUS_Buffer.WR2[5])/(float)(BUS_Buffer.WR2[4]*0x100+BUS_Buffer.WR2[3]); // ������ml/min�����Ա�ϵ��
  }
  
  if(1==NEW_CMD_UFP)     //���˱�
  {
    NEW_CMD_UFP = 0;
    SWITCH_UFP = ((0x08 & BUS_Buffer.WR3[1]) >> 3); 
    if(SWITCH_UFP)      //�ÿ�
    {
      StepMotor_Data[UFP].Enable = ENABLE;  
      StepMotor_Data[UFP].LockRotor = 0;
    }
    else               //�ر�
    {
      StepMotor_Data[UFP].Enable = DISABLE;
    }
    StepMotor_Data[UFP].SetDir = ((0x04 & BUS_Buffer.WR3[1]) >> 2); //����ת
    StepMotor_Data[UFP].Down_Mode = (0x03 & BUS_Buffer.WR3[1]); //��תģʽ   
    if(SLOW_RUN==StepMotor_Data[UFP].Down_Mode)
    {
      Type = UFP;
//      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //�������ת��
//      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800));  
      
    }
    else
    {
      Type = UFP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500);
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR3[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[UFP].Flow = 0;
      FlowClearZero(UFP);
    }
    StepMotor_Data[UFP].RatioSwitch = ((0x10 & BUS_Buffer.WR3[1]) >> 4); //�ù�ϵ�����Կ���
    
    StepMotor_Data[UFP].Ratio = BUS_Buffer.WR3[4]*0x100+BUS_Buffer.WR3[3];  //ml/R
    StepMotor_Data[UFP].SetFlow = BUS_Buffer.WR3[6]*0x100+BUS_Buffer.WR3[5];  //ml/R
    StepMotor_Data[UFP].SetSpeed =  (float)(BUS_Buffer.WR3[6]*0x100+BUS_Buffer.WR3[5])/(float)(BUS_Buffer.WR3[4]*0x100+BUS_Buffer.WR3[3]); // ������ml/min�����Ա�ϵ��
  }
  
  if(1==NEW_CMD_BPP)     //Ѫ����
  {
    NEW_CMD_BPP = 0;
    SWITCH_BPP = ((0x08 & BUS_Buffer.WR4[1]) >> 3); //�ÿ���
    if(SWITCH_BPP)
    {
      StepMotor_Data[BPP].Enable = ENABLE; 
      StepMotor_Data[BPP].LockRotor = 0;
    }
    else   //�ر�
    {
      StepMotor_Data[BPP].Enable = DISABLE; 
    }
    StepMotor_Data[BPP].SetDir = ((0x04 & BUS_Buffer.WR4[1]) >> 2); //����ת
    StepMotor_Data[BPP].Down_Mode = (0x03 & BUS_Buffer.WR4[1]); //��תģʽ 
    if(SLOW_RUN==StepMotor_Data[BPP].Down_Mode)
    {
      Type = BPP;     
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //�������ת��
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800)); 
    }
    else
    {
      Type = BPP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500); 
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR4[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[BPP].Flow=0;
      FlowClearZero(BPP);     
    }
    StepMotor_Data[BPP].RatioSwitch = ((0x10 & BUS_Buffer.WR4[1]) >> 4); //�ù�ϵ�����Կ���    
    StepMotor_Data[BPP].Ratio = BUS_Buffer.WR4[4]*0x100+BUS_Buffer.WR4[3];  //ml/R
    StepMotor_Data[BPP].SetFlow = BUS_Buffer.WR4[6]*0x100+BUS_Buffer.WR4[5];  //ml/R
    StepMotor_Data[BPP].SetSpeed =  (float)(BUS_Buffer.WR4[6]*0x100+BUS_Buffer.WR4[5])/(float)(BUS_Buffer.WR4[4]*0x100+BUS_Buffer.WR4[3]); // ������ml/min�����Ա�ϵ��
  }
  
  if(1==NEW_CMD_HP)      //���ر�
  {
    NEW_CMD_HP = 0;
    SWITCH_HP = ((0x08 & BUS_Buffer.WR5[1]) >> 3); //�ÿ���
    if(SWITCH_HP)
    {
      StepMotor_Data[HP].Enable = ENABLE;    
      StepMotor_Data[HP].LockRotor = 0;    //��ת��0  
      HP_Data.HP_Over = 0;                 //���ر���ע�����0
    }
    else   //�ر�
    {
      StepMotor_Data[HP].Enable = DISABLE; 
    }
    StepMotor_Data[HP].SetDir = ((0x04 & BUS_Buffer.WR5[1]) >> 2); // 0˳ʱ�� ��ת��1��ת
    StepMotor_Data[HP].Down_Mode = (0x03 & BUS_Buffer.WR5[1]); //��תģʽ  
    if(((0x20 & BUS_Buffer.WR5[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      FlowClearZero(HP);
    }
    StepMotor_Data[HP].SetFlow = BUS_Buffer.WR5[5]*0x100+BUS_Buffer.WR5[4];//Ŀ����ע������ml/h��
  }
  
  if(1==NEW_CMD2_HP)      //���ر�У��
  {
    NEW_CMD2_HP = 0;
    HP_Data.HP_Check=((0x80&BUS_Buffer.WR6[1])>>7);    //���ر�У������
    if(HP_Data.HP_Check)
    {
      HP_Data.HP_Over = 0;  //��ע�����0
      HP_Data.HP_Flag = 1;  // ����У����־
      HP_Data.Up_Mode = HP_CORRECT_DEFAULT; 
      StepMotor_Data[HP].Enable = ENABLE; 
      StepMotor_Data[HP].Down_Mode = HP_CORRECT;  //У��ģʽ   //������������
      Type = HP;
      FlowClearZero(HP);
      dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*HPSELF_Speed/15));
    }
    HP_Data.HP_OrderType = BUS_Buffer.WR6[2];        //�·�ע�����ͺ�
    //    if(HP_Data.HP_OrderType!=HP_Data.HP_Type)     //ע�����ͺŶԱ�
    //    {
    //      HP_Data.HP_Compare = 0x01;  
    //    }
  }
  
}


void Up_flag()
{
  BUS_Buffer.RD0[5] = (StepMotor_Data[HP].Up_Mode|
                       (StepMotor_Data[HP].Alert<<2)|       //���ȡ�����
                         (StepMotor_Data[HP].NF<<3)|          //ȱ��
                           (StepMotor_Data[HP].LockRotor<<4)|   //��ת
                             (StepMotor_Data[HP].Braking<<6));   //�øǿ��� 
  
  BUS_Buffer.RD0[4] = (StepMotor_Data[BPP].Up_Mode|
                       (StepMotor_Data[BPP].Alert<<2)|       //���ȡ�����
                         (StepMotor_Data[BPP].NF<<3)|          //ȱ��
                           (StepMotor_Data[BPP].LockRotor<<4)|   //��ת
                             (StepMotor_Data[BPP].Braking<<6));   //�øǿ��� 
  
  
  BUS_Buffer.RD0[3] = (StepMotor_Data[UFP].Up_Mode|
                       (StepMotor_Data[UFP].Alert<<2)|       //���ȡ�����
                         (StepMotor_Data[UFP].NF<<3)|          //ȱ��
                           (StepMotor_Data[UFP].LockRotor<<4)|   //��ת
                             (StepMotor_Data[UFP].Braking<<6));   //�øǿ��� 
  
  BUS_Buffer.RD0[2] = (StepMotor_Data[SBP].Up_Mode|
                       (StepMotor_Data[SBP].Alert<<2)|       //���ȡ�����
                         (StepMotor_Data[SBP].NF<<3)|          //ȱ��
                           (StepMotor_Data[SBP].LockRotor<<4)|   //��ת
                             (StepMotor_Data[SBP].Braking<<6));   //�øǿ��� 
  
  BUS_Buffer.RD0[1] = (StepMotor_Data[SFP].Up_Mode|
                       (StepMotor_Data[SFP].Alert<<2)|       //���ȡ�����
                         (StepMotor_Data[SFP].NF<<3)|          //ȱ��
                           (StepMotor_Data[SFP].LockRotor<<4)|   //��ת
                             (StepMotor_Data[SFP].Braking<<6));   //�øǿ��� 
  
  
  
  if((SELF_TEST_PASS==StepMotor_Data[SFP].Up_Mode)&&
     (SELF_TEST_PASS==StepMotor_Data[SBP].Up_Mode)&&
       (SELF_TEST_PASS==StepMotor_Data[BPP].Up_Mode)&&
         (SELF_TEST_PASS==StepMotor_Data[UFP].Up_Mode)&&
           (SELF_TEST_PASS==StepMotor_Data[HP].Up_Mode))
  {
    BUS_Buffer.RD0[0]=SELF_TEST_PASS;   //5���ö��Լ�ͨ�� 
  }
  
  if((SELF_TEST_RUNING==StepMotor_Data[SFP].Up_Mode)||
     (SELF_TEST_RUNING==StepMotor_Data[SBP].Up_Mode)||
       (SELF_TEST_RUNING==StepMotor_Data[BPP].Up_Mode)||
         (SELF_TEST_RUNING==StepMotor_Data[UFP].Up_Mode)||
           (SELF_TEST_RUNING==StepMotor_Data[HP].Up_Mode))
  {
    BUS_Buffer.RD0[0]=SELF_TEST_RUNING;   //5��������һ���Լ�δ��� 
  }
  else
  {
    if((SELF_TEST_PASS==StepMotor_Data[SFP].Up_Mode)&&
       (SELF_TEST_PASS==StepMotor_Data[SBP].Up_Mode)&&
         (SELF_TEST_PASS==StepMotor_Data[BPP].Up_Mode)&&
           (SELF_TEST_PASS==StepMotor_Data[UFP].Up_Mode)&&
             (SELF_TEST_PASS==StepMotor_Data[HP].Up_Mode))
    {
      BUS_Buffer.RD0[0]=SELF_TEST_PASS;   //5���ö��Լ�ͨ�� 
    }
    else
    {
      BUS_Buffer.RD0[0]=SELF_TEST_ERROR;   //5��������һ���Լ��쳣 
    }
  }
  
  
  //���ܱ�
  BUS_Buffer.RD1[13]=(u8)((u16)(StepMotor_Data[SFP].ReadSpeed*StepMotor_Data[SFP].Ratio)>>8);    
  BUS_Buffer.RD1[12]=(u8)(StepMotor_Data[SFP].ReadSpeed*StepMotor_Data[SFP].Ratio);
  BUS_Buffer.RD1[11]=BUS_Buffer.WR1[4];      //  ml/r
  BUS_Buffer.RD1[10]=BUS_Buffer.WR1[3];        
  BUS_Buffer.RD1[9]=(u8)(Integrated[SFP].Flow>>24);       //�ۼ�����
  BUS_Buffer.RD1[8]=(u8)(Integrated[SFP].Flow>>16);     
  BUS_Buffer.RD1[7]=(u8)(Integrated[SFP].Flow>>8);      
  BUS_Buffer.RD1[6]=(u8)(Integrated[SFP].Flow);  
  BUS_Buffer.RD1[5]=(u8)((StepMotor_Data[SFP].Circle)>>24);                   //�ۼ�Ȧ�����ֽ�
  BUS_Buffer.RD1[4]=(u8)((StepMotor_Data[SFP].Circle)>>16);                   //�ۼ�Ȧ�����ֽ�
  BUS_Buffer.RD1[3]=(u8)((StepMotor_Data[SFP].Circle)>>8);                   //�ۼ�Ȧ�����ֽ�
  BUS_Buffer.RD1[2]=(u8)(StepMotor_Data[SFP].Circle);                      //�ۼ�Ȧ�����ֽ� 
  BUS_Buffer.RD1[0]=(StepMotor_Data[SFP].Up_Mode<<4)|(StepMotor_Data[SFP].Status<<3)|(StepMotor_Data[SFP].LockRotor<<1)|(StepMotor_Data[SFP].Braking);
  
  //��Һ��
  BUS_Buffer.RD2[13]=(u8)((u16)(StepMotor_Data[SBP].ReadSpeed*StepMotor_Data[SBP].Ratio)>>8);    
  BUS_Buffer.RD2[12]=(u8)(StepMotor_Data[SBP].ReadSpeed*StepMotor_Data[SBP].Ratio);     //
  BUS_Buffer.RD2[11]=BUS_Buffer.WR2[4];      //  ml/r
  BUS_Buffer.RD2[10]=BUS_Buffer.WR2[3];        
  BUS_Buffer.RD2[9]=(u8)(Integrated[SBP].Flow>>24);       //�ۼ�����
  BUS_Buffer.RD2[8]=(u8)(Integrated[SBP].Flow>>16);     
  BUS_Buffer.RD2[7]=(u8)(Integrated[SBP].Flow>>8);      
  BUS_Buffer.RD2[6]=(u8)(Integrated[SBP].Flow);  
  BUS_Buffer.RD2[5]=(u8)((StepMotor_Data[SBP].Circle)>>24); 
  BUS_Buffer.RD2[4]=(u8)((StepMotor_Data[SBP].Circle)>>16); 
  BUS_Buffer.RD2[3]=(u8)((StepMotor_Data[SBP].Circle)>>8);                   //�ۼ�Ȧ�����ֽ�
  BUS_Buffer.RD2[2]=(u8)(StepMotor_Data[SBP].Circle);                   
  BUS_Buffer.RD2[0]=(StepMotor_Data[SBP].Up_Mode<<4)|(StepMotor_Data[SBP].Status<<3)|(StepMotor_Data[SBP].LockRotor<<1)|(StepMotor_Data[SBP].Braking);    
  
  //���˱�
  BUS_Buffer.RD3[13]=(u8)((u16)(StepMotor_Data[UFP].ReadSpeed*StepMotor_Data[UFP].Ratio)>>8);    
  BUS_Buffer.RD3[12]=(u8)(StepMotor_Data[UFP].ReadSpeed*StepMotor_Data[UFP].Ratio);     //  ml/mm
  BUS_Buffer.RD3[11]=BUS_Buffer.WR3[4];      //  ml/r
  BUS_Buffer.RD3[10]=BUS_Buffer.WR3[3];        
  BUS_Buffer.RD3[9]=(u8)(Integrated[UFP].Flow>>24);       //�ۼ�����
  BUS_Buffer.RD3[8]=(u8)(Integrated[UFP].Flow>>16);     
  BUS_Buffer.RD3[7]=(u8)(Integrated[UFP].Flow>>8);      
  BUS_Buffer.RD3[6]=(u8)(Integrated[UFP].Flow);  
  BUS_Buffer.RD3[5]=(u8)((StepMotor_Data[UFP].Circle)>>24);
  BUS_Buffer.RD3[4]=(u8)((StepMotor_Data[UFP].Circle)>>16);
  BUS_Buffer.RD3[3]=(u8)((StepMotor_Data[UFP].Circle)>>8);                   //�ۼ�Ȧ�����ֽ�
  BUS_Buffer.RD3[2]=(u8)(StepMotor_Data[UFP].Circle);
  BUS_Buffer.RD3[0]=(StepMotor_Data[UFP].Up_Mode<<4)|(StepMotor_Data[UFP].Status<<3)|(StepMotor_Data[UFP].LockRotor<<1)|(StepMotor_Data[UFP].Braking);      
  
  //Ѫ����
  BUS_Buffer.RD4[13]=(u8)((u16)(StepMotor_Data[BPP].ReadSpeed*StepMotor_Data[BPP].Ratio)>>8);    
  BUS_Buffer.RD4[12]=(u8)(StepMotor_Data[BPP].ReadSpeed*StepMotor_Data[BPP].Ratio);     //  ml/m
  BUS_Buffer.RD4[11]=BUS_Buffer.WR4[4];      //  ml/r
  BUS_Buffer.RD4[10]=BUS_Buffer.WR4[3];        
  BUS_Buffer.RD4[9]=(u8)(Integrated[BPP].Flow>>24);       //�ۼ�����
  BUS_Buffer.RD4[8]=(u8)(Integrated[BPP].Flow>>16);     
  BUS_Buffer.RD4[7]=(u8)(Integrated[BPP].Flow>>8);      
  BUS_Buffer.RD4[6]=(u8)(Integrated[BPP].Flow);
  BUS_Buffer.RD4[5]=(u8)((StepMotor_Data[BPP].Circle)>>24); 
  BUS_Buffer.RD4[4]=(u8)((StepMotor_Data[BPP].Circle)>>16); 
  BUS_Buffer.RD4[3]=(u8)((StepMotor_Data[BPP].Circle)>>8);                   //�ۼ�Ȧ�����ֽ�
  BUS_Buffer.RD4[2]=(u8)(StepMotor_Data[BPP].Circle);
  BUS_Buffer.RD4[0]=(StepMotor_Data[BPP].Up_Mode<<4)|(StepMotor_Data[BPP].Status<<3)|(StepMotor_Data[BPP].Enable<<2)|(StepMotor_Data[BPP].LockRotor<<1)|(StepMotor_Data[BPP].Braking); 
  
  //���ر�
  BUS_Buffer.RD5[8]=BUS_Buffer.WR5[5];      //  ��ע����ml/h
  BUS_Buffer.RD5[7]=BUS_Buffer.WR5[4];         
  BUS_Buffer.RD5[6]=(u8)((u32)HPTmp.Total_Flow>>24);       //�ۼ�����  �Ŵ�1000��
  BUS_Buffer.RD5[5]=(u8)((u32)HPTmp.Total_Flow>>16);     
  BUS_Buffer.RD5[4]=(u8)((u32)HPTmp.Total_Flow>>8);      
  BUS_Buffer.RD5[3]=(u8)(HPTmp.Total_Flow);   
  BUS_Buffer.RD5[2]=HP_Data.HP_Type;
  BUS_Buffer.RD5[1]=(HP_Data.Hall_State<<3)|(Speed_Hall<<2)|(HP_Data.Hall_Bottom<<1)|HP_Data.Hall_Top;  //���˹�翪��δ �ϴ�
  BUS_Buffer.RD5[0]=(StepMotor_Data[HP].Up_Mode<<4)|(StepMotor_Data[HP].Enable<<3)|(StepMotor_Data[HP].SetDir<<2)|(StepMotor_Data[HP].LockRotor<<1)|(HP_Data.HP_Over<<7); 
  
  //���ر�У����Ϣ
  
  BUS_Buffer.RD6[6] = hp_param.correct[3];
  BUS_Buffer.RD6[5] = hp_param.correct[2];
  BUS_Buffer.RD6[4] = hp_param.correct[1];
  BUS_Buffer.RD6[3]=  hp_param.correct[0];
  BUS_Buffer.RD6[2]=HP_Data.HP_Type;    //ע�����ͺ�
  BUS_Buffer.RD6[0]=HP_Data.Up_Mode;   //���ر�У��ģʽ
  
  //���¹�����Ϣ
  
  BUS_Buffer.RD9[0] = CODE/0x100;         //������ʶ����ֽ�
  BUS_Buffer.RD9[1] = CODE%0x100;         //������ʶ����ֽ�
  BUS_Buffer.RD9[2] = H_VER/0x100;        //Ӳ���汾���ֽ�
  BUS_Buffer.RD9[3] = H_VER%0x100;        //Ӳ���汾���ֽ�
  BUS_Buffer.RD9[4] = S_VER/0x100;        //����汾���ֽ�
  BUS_Buffer.RD9[5] = S_VER%0x100;        //����汾���ֽ�
}

void Motor_Run_Mode()
{
  u8 mi;
  for(mi = 0; mi < MOTOR_TOTAL; mi++) 
  {
    Type = mi;
    dSPIN_rx_data[mi] = dSPIN_Get_Status();
    dSPIN_PC_Read((StepMotor_TypeDef)mi);    //�øǼ��
    dSPIN_Status_Parse(&dSPIN_rx_data[mi],mi);    //�������״̬����   
    dSPIN_Cycles_Speed((StepMotor_TypeDef)mi);  //���Ȧ����ת�ٶ�ȡ��Ȧ���Ŵ�100��
//    StepMotor_Stop_Turn((StepMotor_TypeDef)mi);// �����ת�ж�
    if(ENABLE == StepMotor_Data[SBP].Enable)
    {
      LED_HP_ON;
    }
    else
    {
      LED_HP_OFF;
    }
    if(HP == Type)
    {
      HP_RUN_Over();  //���ر���ע����ж�
      HP_RUN_Stop();  //���ؼ���δ�򿪱���
      switch(StepMotor_Data[HP].Down_Mode)    // ���ر�
      {
      case HP_NORMAL_RUN:;     //������ת
      case HP_FAST_FORWARD: 
        {      
          if((ENABLE == StepMotor_Data[HP].Enable)&&(HP_Data.HP_Flow!=0)) //��⵽ע����
          {            
            StepMotor_Data[HP].SetSpeed = StepMotor_Data[HP].SetFlow/HP_Data.HP_Flow/600;   //���� ml/h ���� ml/rpm ����60= rpm/min       
            dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*StepMotor_Data[HP].SetSpeed/15));     //15ת   
            
            if((StepMotor_Data[HP].LockRotor==1)||(HP_Data.HP_Over==1))  //���رö�ת����ע���ͣ���
            {
              StepMotor_Data[HP].Enable = 0;
              dSPIN_Hard_HiZ ();   
            }

          }
          else   //�����ƶ������رյ���
          {
            StepMotor_Data[HP].Enable = 0;
            dSPIN_Hard_HiZ ();  
          }
    
        }
        break;
      case HP_SELF_TEST:   //���ر��Լ�ģʽ
        {
          HP_Self_Test();         
        }
        break;
      case HP_CORRECT:      //���ر�У��ģʽ
        {
          HP_Correction();        
        }
        break;
      default:         
        break;
        
      }  
      
    }
    else
    {
      switch(StepMotor_Data[mi].Down_Mode)    // ���ܱã���Һ�ã����˱ã�Ѫ����ѭ��
      {
      case NORMAL_RUN:     //������ת
        {      
          if((ENABLE == StepMotor_Data[mi].Enable)&&(DISABLE == StepMotor_Data[mi].Braking))  //��������ұøǹ�
          {
            StepMotor_Data[mi].Status = ENABLE;  //�������״̬�ش�
            dSPIN_Run(StepMotor_Data[mi].SetDir, Speed_Steps_to_Par(50*StepMotor_Data[mi].SetSpeed/15));     //15ת
          }
          else   //�����ƶ������رյ���
          {
            StepMotor_Data[mi].Status = DISABLE;
            dSPIN_Hard_HiZ ();  
          }
          if((StepMotor_Data[mi].LockRotor)||(StepMotor_Data[mi].Alert))  //�����е���������ת
          {
            
            StepMotor_Data[mi].Enable = 0;
            StepMotor_Data[mi].Status = 0;
            dSPIN_Hard_HiZ (); 
            
          }
          
          
        }
        break;
      case SLOW_RUN:    //��·��װģʽ
        {
          if(!dSPIN_Busy_HW())
          {
            dSPIN_Hard_HiZ ();
          }
        }
        break;
      case SELF_TEST:   //�Լ�
        { 
          if(ENABLE == StepMotor_Data[mi].Enable)         //������
          {           
            if(dSPIN_Busy_HW())                                 //��ȡBUSY��־
            {
              StepMotor_Data[mi].Up_Mode =  SELF_TEST_RUNING;     //�����Լ� 
            }
            if(StepMotor_Data[mi].Circle > 500)   //�Լ�Ȧ��5Ȧ
            {
              dSPIN_Hard_HiZ ();
              StepMotor_Data[mi].Enable = 0;
              StepMotor_Data[mi].Up_Mode = SELF_TEST_PASS;      //�Լ����
              StepMotor_Data[mi].Down_Mode = SELF_Finish;
            }
            if(((StepMotor_Data[mi].LockRotor)||(StepMotor_Data[mi].Alert)||(ENABLE == StepMotor_Data[mi].Braking))||(5000<Motor_Time))  //�Լ�ʱ�䳬��50S
            {
              
              StepMotor_Data[mi].Enable = 0;
              dSPIN_Hard_HiZ (); 
              StepMotor_Data[mi].Up_Mode = SELF_TEST_ERROR;      //�Լ����
              
            }           
          }
          
        }
        break;
      default:         
        break;
        
      }
    }
  }
  
  
  
}




void FlowClearZero(u8 i)     //����������0����
{
  Type = i;
  
  StepMotor_Data[i].RealPulse = 0;   //����������0
  StepMotor_Data[i].LockRotor = 0; //��ת��0
  
  dSPIN_RegsStruct[i].ABS_POS = 0;
  dSPIN_Set_Param(dSPIN_ABS_POS, dSPIN_RegsStruct[i].ABS_POS);
  
  StepMotor_Data[i].Step = 0;
  Old_Circle[i] = 0;
  StepMotor_Data[i].Circle = 0;
  StepMotor_Data[i].Temp_Circle = 0;
  Cycle[i]= 0;
  StepMotor_Data[i].ReadSpeed = 0;
  if(HP == i)
  {
    HPTmp.HP_Type = 0;         //�ͺ��ݴ�
    HPTmp.Ratio  = 0;          //��һ�ε�ϵ��   ml/r
    HPTmp.Plus = 0;  //��һ��Ȧ��
    HPTmp.Temp_Flow = 0;
    HPTmp.Total_Flow = 0;
  }
}


