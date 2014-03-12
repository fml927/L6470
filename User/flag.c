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






u8 Cycle[MOTOR_TOTAL] = {0};  //ABS_POS寄存器溢出次数
u32 Old_Circle[MOTOR_TOTAL] = {0};  //上次圈数暂存

Flow Integrated[5]={0};   //累计流量结构体

u8  Type=0;              //轮询电机的变量

u16  dSPIN_rx_data[5] = {0};  //读取L6470寄存器状态

u16 Motor_Time = 0;    //泵自检时间计时

u8  SWITCH = 0;       //WR0 01跳过自检 10所有泵自检 11 所有泵及注射器自检
u8  SWITCH_SFP = 0;   //电机开关(0:关,1:开) 功能泵
u8  SWITCH_SBP = 0;  //补液泵
u8  SWITCH_UFP = 0;  //超滤泵
u8  SWITCH_BPP = 0;  //血浆泵
u8  SWITCH_HP = 0;    //肝素泵

void dSPIN_Cycles_Speed(StepMotor_TypeDef Motorx)
{
  if((StepMotor_Data[Motorx].Enable == 1)&&(dSPIN_Get_Param(dSPIN_ABS_POS)>0))                         //电机转速读取
  {     
    
    if(StepMotor_Data[Motorx].SetDir == 1)   //正转，记圈数
    {
      StepMotor_Data[Motorx].Step  = dSPIN_Get_Param(dSPIN_ABS_POS);
    }
    else
    {
      StepMotor_Data[Motorx].Step  = 0x400000-dSPIN_Get_Param(dSPIN_ABS_POS);
    }   
    StepMotor_Data[Motorx].Temp_Circle = StepMotor_Data[Motorx].Step/STEPMOTOR_PULSE; //圈数放大100倍
    if(Old_Circle[Motorx]!=StepMotor_Data[Motorx].Temp_Circle)
    {          
      if(Old_Circle[Motorx]>StepMotor_Data[Motorx].Temp_Circle)   //寄存器溢出
      {
        Cycle[Motorx]++;
        
      }
      Old_Circle[Motorx] = StepMotor_Data[Motorx].Temp_Circle;   //暂存上一次的值
    }
    
    StepMotor_Data[Motorx].Circle = Cycle[Motorx]*16384 + StepMotor_Data[Motorx].Temp_Circle;  //163.84圈寄存器溢出
    
  } 
  if(StepMotor_Data[Motorx].Enable ==1)
  {
    StepMotor_Data[Motorx].ReadSpeed = (float)dSPIN_Get_Param(dSPIN_SPEED)/224;   //读取转速
  }
  else
  {
    StepMotor_Data[Motorx].ReadSpeed = 0;  //停电机时，速度清0
  }
}

void Down_flag()
{
  if(1==NEW_CMD)       //公共信息，占未使用
  {
    NEW_CMD = 0;
    SWITCH = BUS_Buffer.WR0[1];
    if(1 == SWITCH)      //跳过自检,关泵,流量清0
    {
      for(u8 i = 0; i<5; i++)
      {
        Type = i;
        StepMotor_Data[i].Enable = DISABLE;  
        dSPIN_Hard_HiZ ();
        FlowClearZero(i);
      }
      
    }
    if(3 == SWITCH)      //泵自检
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
          dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*HPSELF_Speed/15));  //肝素泵自检速度
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
    if(2 == SWITCH)   //肝素泵不自检
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
  
  if(1==NEW_CMD_SFP)     //功能泵
  {
    NEW_CMD_SFP = 0;
    SWITCH_SFP = ((0x08 & BUS_Buffer.WR1[1]) >> 3); //泵开关
    if(SWITCH_SFP)
    {
      StepMotor_Data[SFP].Enable = ENABLE;  
      StepMotor_Data[SFP].LockRotor = 0; 
    }
    else   //关泵
    {      
      StepMotor_Data[SFP].Enable = DISABLE; 
    }
    
    StepMotor_Data[SFP].SetDir = ((0x04 & BUS_Buffer.WR1[1]) >> 2); //正反转
    StepMotor_Data[SFP].Down_Mode = (0x03 & BUS_Buffer.WR1[1]); //运转模式
    if(SLOW_RUN==StepMotor_Data[SFP].Down_Mode)
    {
      Type = SFP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //限制最大转速
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800));      
    }
    else
    {
      Type = SFP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500);
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR1[1]) >> 5))  //0流量累计，1流量归零
    {
      StepMotor_Data[SFP].Flow = 0;
      FlowClearZero(SFP);
    }
    StepMotor_Data[SFP].RatioSwitch = ((0x10 & BUS_Buffer.WR1[1]) >> 4); //泵管系数测试开关 
    
    StepMotor_Data[SFP].Ratio = BUS_Buffer.WR1[4]*0x100+BUS_Buffer.WR1[3];  //ml/R
    StepMotor_Data[SFP].SetFlow = BUS_Buffer.WR1[6]*0x100+BUS_Buffer.WR1[5];  //ml/R
    StepMotor_Data[SFP].SetSpeed =  (float)(BUS_Buffer.WR1[6]*0x100+BUS_Buffer.WR1[5])/(float)(BUS_Buffer.WR1[4]*0x100+BUS_Buffer.WR1[3]); // 流量（ml/min）除以泵系数
  }
  
  if(1==NEW_CMD_SBP)     //补液泵
  {
    NEW_CMD_SBP = 0;
    SWITCH_SBP = ((0x08 & BUS_Buffer.WR2[1]) >> 3); //泵开关
    if(SWITCH_SBP)
    {
      StepMotor_Data[SBP].Enable = ENABLE; 
      StepMotor_Data[SBP].LockRotor = 0; 
    }
    else   //关泵
    {
      StepMotor_Data[SBP].Enable = DISABLE;
    }
    StepMotor_Data[SBP].SetDir = ((0x04 & BUS_Buffer.WR2[1]) >> 2); //正反转
    StepMotor_Data[SBP].Down_Mode = (0x03 & BUS_Buffer.WR2[1]); //运转模式 
    if(SLOW_RUN==StepMotor_Data[SBP].Down_Mode)
    {
      Type = SBP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //限制最大转速
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800));      
    }
    else
    {
      Type = SBP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500);
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR2[1]) >> 5))  //0累计，1累计归零
    {
      StepMotor_Data[SBP].Flow = 0;
      FlowClearZero(SBP);
    }
    StepMotor_Data[SBP].RatioSwitch = ((0x10 & BUS_Buffer.WR2[1]) >> 4); //泵管系数测试开关   
    
    StepMotor_Data[SBP].Ratio = BUS_Buffer.WR2[4]*0x100+BUS_Buffer.WR2[3];  //ml/R
    StepMotor_Data[SBP].SetFlow = BUS_Buffer.WR2[6]*0x100+BUS_Buffer.WR2[5];  //ml/R
    StepMotor_Data[SBP].SetSpeed =  (float)(BUS_Buffer.WR2[6]*0x100+BUS_Buffer.WR2[5])/(float)(BUS_Buffer.WR2[4]*0x100+BUS_Buffer.WR2[3]); // 流量（ml/min）除以泵系数
  }
  
  if(1==NEW_CMD_UFP)     //超滤泵
  {
    NEW_CMD_UFP = 0;
    SWITCH_UFP = ((0x08 & BUS_Buffer.WR3[1]) >> 3); 
    if(SWITCH_UFP)      //泵开
    {
      StepMotor_Data[UFP].Enable = ENABLE;  
      StepMotor_Data[UFP].LockRotor = 0;
    }
    else               //关泵
    {
      StepMotor_Data[UFP].Enable = DISABLE;
    }
    StepMotor_Data[UFP].SetDir = ((0x04 & BUS_Buffer.WR3[1]) >> 2); //正反转
    StepMotor_Data[UFP].Down_Mode = (0x03 & BUS_Buffer.WR3[1]); //运转模式   
    if(SLOW_RUN==StepMotor_Data[UFP].Down_Mode)
    {
      Type = UFP;
//      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //限制最大转速
//      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800));  
      
    }
    else
    {
      Type = UFP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500);
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR3[1]) >> 5))  //0累计，1累计归零
    {
      StepMotor_Data[UFP].Flow = 0;
      FlowClearZero(UFP);
    }
    StepMotor_Data[UFP].RatioSwitch = ((0x10 & BUS_Buffer.WR3[1]) >> 4); //泵管系数测试开关
    
    StepMotor_Data[UFP].Ratio = BUS_Buffer.WR3[4]*0x100+BUS_Buffer.WR3[3];  //ml/R
    StepMotor_Data[UFP].SetFlow = BUS_Buffer.WR3[6]*0x100+BUS_Buffer.WR3[5];  //ml/R
    StepMotor_Data[UFP].SetSpeed =  (float)(BUS_Buffer.WR3[6]*0x100+BUS_Buffer.WR3[5])/(float)(BUS_Buffer.WR3[4]*0x100+BUS_Buffer.WR3[3]); // 流量（ml/min）除以泵系数
  }
  
  if(1==NEW_CMD_BPP)     //血浆泵
  {
    NEW_CMD_BPP = 0;
    SWITCH_BPP = ((0x08 & BUS_Buffer.WR4[1]) >> 3); //泵开关
    if(SWITCH_BPP)
    {
      StepMotor_Data[BPP].Enable = ENABLE; 
      StepMotor_Data[BPP].LockRotor = 0;
    }
    else   //关泵
    {
      StepMotor_Data[BPP].Enable = DISABLE; 
    }
    StepMotor_Data[BPP].SetDir = ((0x04 & BUS_Buffer.WR4[1]) >> 2); //正反转
    StepMotor_Data[BPP].Down_Mode = (0x03 & BUS_Buffer.WR4[1]); //运转模式 
    if(SLOW_RUN==StepMotor_Data[BPP].Down_Mode)
    {
      Type = BPP;     
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(50);  //限制最大转速
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
      dSPIN_Move(REV, (uint32_t)(12800)); 
    }
    else
    {
      Type = BPP;
      dSPIN_RegsStruct[Type].MAX_SPEED = MaxSpd_Steps_to_Par(500); 
      dSPIN_Set_Param(dSPIN_MAX_SPEED, dSPIN_RegsStruct[Type].MAX_SPEED);
    }
    
    if(((0x20 & BUS_Buffer.WR4[1]) >> 5))  //0累计，1累计归零
    {
      StepMotor_Data[BPP].Flow=0;
      FlowClearZero(BPP);     
    }
    StepMotor_Data[BPP].RatioSwitch = ((0x10 & BUS_Buffer.WR4[1]) >> 4); //泵管系数测试开关    
    StepMotor_Data[BPP].Ratio = BUS_Buffer.WR4[4]*0x100+BUS_Buffer.WR4[3];  //ml/R
    StepMotor_Data[BPP].SetFlow = BUS_Buffer.WR4[6]*0x100+BUS_Buffer.WR4[5];  //ml/R
    StepMotor_Data[BPP].SetSpeed =  (float)(BUS_Buffer.WR4[6]*0x100+BUS_Buffer.WR4[5])/(float)(BUS_Buffer.WR4[4]*0x100+BUS_Buffer.WR4[3]); // 流量（ml/min）除以泵系数
  }
  
  if(1==NEW_CMD_HP)      //肝素泵
  {
    NEW_CMD_HP = 0;
    SWITCH_HP = ((0x08 & BUS_Buffer.WR5[1]) >> 3); //泵开关
    if(SWITCH_HP)
    {
      StepMotor_Data[HP].Enable = ENABLE;    
      StepMotor_Data[HP].LockRotor = 0;    //堵转清0  
      HP_Data.HP_Over = 0;                 //肝素泵推注完成清0
    }
    else   //关泵
    {
      StepMotor_Data[HP].Enable = DISABLE; 
    }
    StepMotor_Data[HP].SetDir = ((0x04 & BUS_Buffer.WR5[1]) >> 2); // 0顺时针 正转，1反转
    StepMotor_Data[HP].Down_Mode = (0x03 & BUS_Buffer.WR5[1]); //运转模式  
    if(((0x20 & BUS_Buffer.WR5[1]) >> 5))  //0累计，1累计归零
    {
      FlowClearZero(HP);
    }
    StepMotor_Data[HP].SetFlow = BUS_Buffer.WR5[5]*0x100+BUS_Buffer.WR5[4];//目标推注流量（ml/h）
  }
  
  if(1==NEW_CMD2_HP)      //肝素泵校正
  {
    NEW_CMD2_HP = 0;
    HP_Data.HP_Check=((0x80&BUS_Buffer.WR6[1])>>7);    //肝素泵校正命令
    if(HP_Data.HP_Check)
    {
      HP_Data.HP_Over = 0;  //推注完成清0
      HP_Data.HP_Flag = 1;  // 肝素校正标志
      HP_Data.Up_Mode = HP_CORRECT_DEFAULT; 
      StepMotor_Data[HP].Enable = ENABLE; 
      StepMotor_Data[HP].Down_Mode = HP_CORRECT;  //校正模式   //肝素流量清零
      Type = HP;
      FlowClearZero(HP);
      dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*HPSELF_Speed/15));
    }
    HP_Data.HP_OrderType = BUS_Buffer.WR6[2];        //下发注射器型号
    //    if(HP_Data.HP_OrderType!=HP_Data.HP_Type)     //注射器型号对比
    //    {
    //      HP_Data.HP_Compare = 0x01;  
    //    }
  }
  
}


void Up_flag()
{
  BUS_Buffer.RD0[5] = (StepMotor_Data[HP].Up_Mode|
                       (StepMotor_Data[HP].Alert<<2)|       //过热、过流
                         (StepMotor_Data[HP].NF<<3)|          //缺相
                           (StepMotor_Data[HP].LockRotor<<4)|   //堵转
                             (StepMotor_Data[HP].Braking<<6));   //泵盖开关 
  
  BUS_Buffer.RD0[4] = (StepMotor_Data[BPP].Up_Mode|
                       (StepMotor_Data[BPP].Alert<<2)|       //过热、过流
                         (StepMotor_Data[BPP].NF<<3)|          //缺相
                           (StepMotor_Data[BPP].LockRotor<<4)|   //堵转
                             (StepMotor_Data[BPP].Braking<<6));   //泵盖开关 
  
  
  BUS_Buffer.RD0[3] = (StepMotor_Data[UFP].Up_Mode|
                       (StepMotor_Data[UFP].Alert<<2)|       //过热、过流
                         (StepMotor_Data[UFP].NF<<3)|          //缺相
                           (StepMotor_Data[UFP].LockRotor<<4)|   //堵转
                             (StepMotor_Data[UFP].Braking<<6));   //泵盖开关 
  
  BUS_Buffer.RD0[2] = (StepMotor_Data[SBP].Up_Mode|
                       (StepMotor_Data[SBP].Alert<<2)|       //过热、过流
                         (StepMotor_Data[SBP].NF<<3)|          //缺相
                           (StepMotor_Data[SBP].LockRotor<<4)|   //堵转
                             (StepMotor_Data[SBP].Braking<<6));   //泵盖开关 
  
  BUS_Buffer.RD0[1] = (StepMotor_Data[SFP].Up_Mode|
                       (StepMotor_Data[SFP].Alert<<2)|       //过热、过流
                         (StepMotor_Data[SFP].NF<<3)|          //缺相
                           (StepMotor_Data[SFP].LockRotor<<4)|   //堵转
                             (StepMotor_Data[SFP].Braking<<6));   //泵盖开关 
  
  
  
  if((SELF_TEST_PASS==StepMotor_Data[SFP].Up_Mode)&&
     (SELF_TEST_PASS==StepMotor_Data[SBP].Up_Mode)&&
       (SELF_TEST_PASS==StepMotor_Data[BPP].Up_Mode)&&
         (SELF_TEST_PASS==StepMotor_Data[UFP].Up_Mode)&&
           (SELF_TEST_PASS==StepMotor_Data[HP].Up_Mode))
  {
    BUS_Buffer.RD0[0]=SELF_TEST_PASS;   //5个泵都自检通过 
  }
  
  if((SELF_TEST_RUNING==StepMotor_Data[SFP].Up_Mode)||
     (SELF_TEST_RUNING==StepMotor_Data[SBP].Up_Mode)||
       (SELF_TEST_RUNING==StepMotor_Data[BPP].Up_Mode)||
         (SELF_TEST_RUNING==StepMotor_Data[UFP].Up_Mode)||
           (SELF_TEST_RUNING==StepMotor_Data[HP].Up_Mode))
  {
    BUS_Buffer.RD0[0]=SELF_TEST_RUNING;   //5个泵中有一个自检未完成 
  }
  else
  {
    if((SELF_TEST_PASS==StepMotor_Data[SFP].Up_Mode)&&
       (SELF_TEST_PASS==StepMotor_Data[SBP].Up_Mode)&&
         (SELF_TEST_PASS==StepMotor_Data[BPP].Up_Mode)&&
           (SELF_TEST_PASS==StepMotor_Data[UFP].Up_Mode)&&
             (SELF_TEST_PASS==StepMotor_Data[HP].Up_Mode))
    {
      BUS_Buffer.RD0[0]=SELF_TEST_PASS;   //5个泵都自检通过 
    }
    else
    {
      BUS_Buffer.RD0[0]=SELF_TEST_ERROR;   //5个泵中有一个自检异常 
    }
  }
  
  
  //功能泵
  BUS_Buffer.RD1[13]=(u8)((u16)(StepMotor_Data[SFP].ReadSpeed*StepMotor_Data[SFP].Ratio)>>8);    
  BUS_Buffer.RD1[12]=(u8)(StepMotor_Data[SFP].ReadSpeed*StepMotor_Data[SFP].Ratio);
  BUS_Buffer.RD1[11]=BUS_Buffer.WR1[4];      //  ml/r
  BUS_Buffer.RD1[10]=BUS_Buffer.WR1[3];        
  BUS_Buffer.RD1[9]=(u8)(Integrated[SFP].Flow>>24);       //累计流量
  BUS_Buffer.RD1[8]=(u8)(Integrated[SFP].Flow>>16);     
  BUS_Buffer.RD1[7]=(u8)(Integrated[SFP].Flow>>8);      
  BUS_Buffer.RD1[6]=(u8)(Integrated[SFP].Flow);  
  BUS_Buffer.RD1[5]=(u8)((StepMotor_Data[SFP].Circle)>>24);                   //累计圈数高字节
  BUS_Buffer.RD1[4]=(u8)((StepMotor_Data[SFP].Circle)>>16);                   //累计圈数高字节
  BUS_Buffer.RD1[3]=(u8)((StepMotor_Data[SFP].Circle)>>8);                   //累计圈数高字节
  BUS_Buffer.RD1[2]=(u8)(StepMotor_Data[SFP].Circle);                      //累计圈数低字节 
  BUS_Buffer.RD1[0]=(StepMotor_Data[SFP].Up_Mode<<4)|(StepMotor_Data[SFP].Status<<3)|(StepMotor_Data[SFP].LockRotor<<1)|(StepMotor_Data[SFP].Braking);
  
  //补液泵
  BUS_Buffer.RD2[13]=(u8)((u16)(StepMotor_Data[SBP].ReadSpeed*StepMotor_Data[SBP].Ratio)>>8);    
  BUS_Buffer.RD2[12]=(u8)(StepMotor_Data[SBP].ReadSpeed*StepMotor_Data[SBP].Ratio);     //
  BUS_Buffer.RD2[11]=BUS_Buffer.WR2[4];      //  ml/r
  BUS_Buffer.RD2[10]=BUS_Buffer.WR2[3];        
  BUS_Buffer.RD2[9]=(u8)(Integrated[SBP].Flow>>24);       //累计流量
  BUS_Buffer.RD2[8]=(u8)(Integrated[SBP].Flow>>16);     
  BUS_Buffer.RD2[7]=(u8)(Integrated[SBP].Flow>>8);      
  BUS_Buffer.RD2[6]=(u8)(Integrated[SBP].Flow);  
  BUS_Buffer.RD2[5]=(u8)((StepMotor_Data[SBP].Circle)>>24); 
  BUS_Buffer.RD2[4]=(u8)((StepMotor_Data[SBP].Circle)>>16); 
  BUS_Buffer.RD2[3]=(u8)((StepMotor_Data[SBP].Circle)>>8);                   //累计圈数高字节
  BUS_Buffer.RD2[2]=(u8)(StepMotor_Data[SBP].Circle);                   
  BUS_Buffer.RD2[0]=(StepMotor_Data[SBP].Up_Mode<<4)|(StepMotor_Data[SBP].Status<<3)|(StepMotor_Data[SBP].LockRotor<<1)|(StepMotor_Data[SBP].Braking);    
  
  //超滤泵
  BUS_Buffer.RD3[13]=(u8)((u16)(StepMotor_Data[UFP].ReadSpeed*StepMotor_Data[UFP].Ratio)>>8);    
  BUS_Buffer.RD3[12]=(u8)(StepMotor_Data[UFP].ReadSpeed*StepMotor_Data[UFP].Ratio);     //  ml/mm
  BUS_Buffer.RD3[11]=BUS_Buffer.WR3[4];      //  ml/r
  BUS_Buffer.RD3[10]=BUS_Buffer.WR3[3];        
  BUS_Buffer.RD3[9]=(u8)(Integrated[UFP].Flow>>24);       //累计流量
  BUS_Buffer.RD3[8]=(u8)(Integrated[UFP].Flow>>16);     
  BUS_Buffer.RD3[7]=(u8)(Integrated[UFP].Flow>>8);      
  BUS_Buffer.RD3[6]=(u8)(Integrated[UFP].Flow);  
  BUS_Buffer.RD3[5]=(u8)((StepMotor_Data[UFP].Circle)>>24);
  BUS_Buffer.RD3[4]=(u8)((StepMotor_Data[UFP].Circle)>>16);
  BUS_Buffer.RD3[3]=(u8)((StepMotor_Data[UFP].Circle)>>8);                   //累计圈数高字节
  BUS_Buffer.RD3[2]=(u8)(StepMotor_Data[UFP].Circle);
  BUS_Buffer.RD3[0]=(StepMotor_Data[UFP].Up_Mode<<4)|(StepMotor_Data[UFP].Status<<3)|(StepMotor_Data[UFP].LockRotor<<1)|(StepMotor_Data[UFP].Braking);      
  
  //血浆泵
  BUS_Buffer.RD4[13]=(u8)((u16)(StepMotor_Data[BPP].ReadSpeed*StepMotor_Data[BPP].Ratio)>>8);    
  BUS_Buffer.RD4[12]=(u8)(StepMotor_Data[BPP].ReadSpeed*StepMotor_Data[BPP].Ratio);     //  ml/m
  BUS_Buffer.RD4[11]=BUS_Buffer.WR4[4];      //  ml/r
  BUS_Buffer.RD4[10]=BUS_Buffer.WR4[3];        
  BUS_Buffer.RD4[9]=(u8)(Integrated[BPP].Flow>>24);       //累计流量
  BUS_Buffer.RD4[8]=(u8)(Integrated[BPP].Flow>>16);     
  BUS_Buffer.RD4[7]=(u8)(Integrated[BPP].Flow>>8);      
  BUS_Buffer.RD4[6]=(u8)(Integrated[BPP].Flow);
  BUS_Buffer.RD4[5]=(u8)((StepMotor_Data[BPP].Circle)>>24); 
  BUS_Buffer.RD4[4]=(u8)((StepMotor_Data[BPP].Circle)>>16); 
  BUS_Buffer.RD4[3]=(u8)((StepMotor_Data[BPP].Circle)>>8);                   //累计圈数高字节
  BUS_Buffer.RD4[2]=(u8)(StepMotor_Data[BPP].Circle);
  BUS_Buffer.RD4[0]=(StepMotor_Data[BPP].Up_Mode<<4)|(StepMotor_Data[BPP].Status<<3)|(StepMotor_Data[BPP].Enable<<2)|(StepMotor_Data[BPP].LockRotor<<1)|(StepMotor_Data[BPP].Braking); 
  
  //肝素泵
  BUS_Buffer.RD5[8]=BUS_Buffer.WR5[5];      //  推注速率ml/h
  BUS_Buffer.RD5[7]=BUS_Buffer.WR5[4];         
  BUS_Buffer.RD5[6]=(u8)((u32)HPTmp.Total_Flow>>24);       //累计流量  放大1000倍
  BUS_Buffer.RD5[5]=(u8)((u32)HPTmp.Total_Flow>>16);     
  BUS_Buffer.RD5[4]=(u8)((u32)HPTmp.Total_Flow>>8);      
  BUS_Buffer.RD5[3]=(u8)(HPTmp.Total_Flow);   
  BUS_Buffer.RD5[2]=HP_Data.HP_Type;
  BUS_Buffer.RD5[1]=(HP_Data.Hall_State<<3)|(Speed_Hall<<2)|(HP_Data.Hall_Bottom<<1)|HP_Data.Hall_Top;  //顶端光电开关未 上传
  BUS_Buffer.RD5[0]=(StepMotor_Data[HP].Up_Mode<<4)|(StepMotor_Data[HP].Enable<<3)|(StepMotor_Data[HP].SetDir<<2)|(StepMotor_Data[HP].LockRotor<<1)|(HP_Data.HP_Over<<7); 
  
  //肝素泵校正信息
  
  BUS_Buffer.RD6[6] = hp_param.correct[3];
  BUS_Buffer.RD6[5] = hp_param.correct[2];
  BUS_Buffer.RD6[4] = hp_param.correct[1];
  BUS_Buffer.RD6[3]=  hp_param.correct[0];
  BUS_Buffer.RD6[2]=HP_Data.HP_Type;    //注射器型号
  BUS_Buffer.RD6[0]=HP_Data.Up_Mode;   //肝素泵校正模式
  
  //上下公共信息
  
  BUS_Buffer.RD9[0] = CODE/0x100;         //部件标识码高字节
  BUS_Buffer.RD9[1] = CODE%0x100;         //部件标识码低字节
  BUS_Buffer.RD9[2] = H_VER/0x100;        //硬件版本高字节
  BUS_Buffer.RD9[3] = H_VER%0x100;        //硬件版本低字节
  BUS_Buffer.RD9[4] = S_VER/0x100;        //软件版本高字节
  BUS_Buffer.RD9[5] = S_VER%0x100;        //软件版本低字节
}

void Motor_Run_Mode()
{
  u8 mi;
  for(mi = 0; mi < MOTOR_TOTAL; mi++) 
  {
    Type = mi;
    dSPIN_rx_data[mi] = dSPIN_Get_Status();
    dSPIN_PC_Read((StepMotor_TypeDef)mi);    //泵盖检测
    dSPIN_Status_Parse(&dSPIN_rx_data[mi],mi);    //电机运行状态解析   
    dSPIN_Cycles_Speed((StepMotor_TypeDef)mi);  //电机圈数和转速读取，圈数放大100倍
//    StepMotor_Stop_Turn((StepMotor_TypeDef)mi);// 电机堵转判断
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
      HP_RUN_Over();  //肝素泵推注完成判断
      HP_RUN_Stop();  //肝素夹子未打开报警
      switch(StepMotor_Data[HP].Down_Mode)    // 肝素泵
      {
      case HP_NORMAL_RUN:;     //正常运转
      case HP_FAST_FORWARD: 
        {      
          if((ENABLE == StepMotor_Data[HP].Enable)&&(HP_Data.HP_Flow!=0)) //检测到注射器
          {            
            StepMotor_Data[HP].SetSpeed = StepMotor_Data[HP].SetFlow/HP_Data.HP_Flow/600;   //流量 ml/h 除以 ml/rpm 除以60= rpm/min       
            dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*StepMotor_Data[HP].SetSpeed/15));     //15转   
            
            if((StepMotor_Data[HP].LockRotor==1)||(HP_Data.HP_Over==1))  //肝素泵堵转或推注完成停电机
            {
              StepMotor_Data[HP].Enable = 0;
              dSPIN_Hard_HiZ ();   
            }

          }
          else   //紧急制动，并关闭电桥
          {
            StepMotor_Data[HP].Enable = 0;
            dSPIN_Hard_HiZ ();  
          }
    
        }
        break;
      case HP_SELF_TEST:   //肝素泵自检模式
        {
          HP_Self_Test();         
        }
        break;
      case HP_CORRECT:      //肝素泵校正模式
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
      switch(StepMotor_Data[mi].Down_Mode)    // 功能泵，补液泵，超滤泵，血浆泵循环
      {
      case NORMAL_RUN:     //正常运转
        {      
          if((ENABLE == StepMotor_Data[mi].Enable)&&(DISABLE == StepMotor_Data[mi].Braking))  //开命令，并且泵盖关
          {
            StepMotor_Data[mi].Status = ENABLE;  //电机运行状态回传
            dSPIN_Run(StepMotor_Data[mi].SetDir, Speed_Steps_to_Par(50*StepMotor_Data[mi].SetSpeed/15));     //15转
          }
          else   //紧急制动，并关闭电桥
          {
            StepMotor_Data[mi].Status = DISABLE;
            dSPIN_Hard_HiZ ();  
          }
          if((StepMotor_Data[mi].LockRotor)||(StepMotor_Data[mi].Alert))  //治疗中电机过流或堵转
          {
            
            StepMotor_Data[mi].Enable = 0;
            StepMotor_Data[mi].Status = 0;
            dSPIN_Hard_HiZ (); 
            
          }
          
          
        }
        break;
      case SLOW_RUN:    //管路安装模式
        {
          if(!dSPIN_Busy_HW())
          {
            dSPIN_Hard_HiZ ();
          }
        }
        break;
      case SELF_TEST:   //自检
        { 
          if(ENABLE == StepMotor_Data[mi].Enable)         //开命令
          {           
            if(dSPIN_Busy_HW())                                 //读取BUSY标志
            {
              StepMotor_Data[mi].Up_Mode =  SELF_TEST_RUNING;     //正在自检 
            }
            if(StepMotor_Data[mi].Circle > 500)   //自检圈数5圈
            {
              dSPIN_Hard_HiZ ();
              StepMotor_Data[mi].Enable = 0;
              StepMotor_Data[mi].Up_Mode = SELF_TEST_PASS;      //自检完成
              StepMotor_Data[mi].Down_Mode = SELF_Finish;
            }
            if(((StepMotor_Data[mi].LockRotor)||(StepMotor_Data[mi].Alert)||(ENABLE == StepMotor_Data[mi].Braking))||(5000<Motor_Time))  //自检时间超过50S
            {
              
              StepMotor_Data[mi].Enable = 0;
              dSPIN_Hard_HiZ (); 
              StepMotor_Data[mi].Up_Mode = SELF_TEST_ERROR;      //自检错误
              
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




void FlowClearZero(u8 i)     //肝素流量清0函数
{
  Type = i;
  
  StepMotor_Data[i].RealPulse = 0;   //霍尔脉冲清0
  StepMotor_Data[i].LockRotor = 0; //堵转清0
  
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
    HPTmp.HP_Type = 0;         //型号暂存
    HPTmp.Ratio  = 0;          //上一次的系数   ml/r
    HPTmp.Plus = 0;  //上一次圈数
    HPTmp.Temp_Flow = 0;
    HPTmp.Total_Flow = 0;
  }
}


