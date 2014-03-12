#include "stm32f10x.h"
#include "hp.h"
#include "swsbus.h"
#include "flash.h"
#include "StepMotor.h"
#include "AD7799.h"
#include "flag.h"
#include "dspin.h"
#include "flag.h"

u32 HP_Temp_Circle=0;
u8 HP_Time_Flag[6] = {0};
u8 HP_Bottom_State = 0; //肝素泵推注到底 状态标志
float HP_Bottom_Temp = 0; //肝素泵推注到底 压力值暂存

u16 TYPE_Time = 0; //注射器型号检测，时间延时

u16 HP_Time = 0;   //肝素AD读取计时
u16 HP_Number = 0; //肝素AD比较记次数
u8 HP_Lock_State = 0; //肝素泵堵转(夹子未打开) 状态标志
float HP_OldValue = 0; //暂存上一次AD值
float HP_TempValue = 0; //暂存上一次AD值

HP_PARAM hp_param;
HPFlow HPTmp = {0};  //肝素泵流量累计结构体
HP_Work HP_Data = {0};
u8 Write_Buffer[10];
u8 Read_Buffer[10];
u8 CRC_Flag = 0;
void HP_CheckInfo()
{
  
  CRC_Flag = Flash_Read_Param(0,(u8 *)&hp_param,sizeof(HP_PARAM));//读取值
  if(1 == CRC_Flag) //校验不正确保存默认值到FLASH
  {
    hp_param.correct[0] = Correct_Length10;
    hp_param.correct[1] = Correct_Length20;
    hp_param.correct[2] = Correct_Length30;
    hp_param.correct[3] = Correct_Length50;
    
    Flash_Save_Param(0,(u8 *)&hp_param,sizeof(hp_param));   
    
  }
  
}

u8 IO_Shake(u8 hallstate,u8 location)
{
  
  static u8 HallHight[16]={0};
  static u8 HallLow[16] = {0};
  static u8 HallReturn[16] = {0};
  if(1==hallstate)
  {
    HallHight[location]++;
    HallLow[location] = 0;
    if(HallHight[location]>STEPMOTOR_IN_DEBOUNCE)   //去抖
    {
      HallHight[location] = 0;
      HallReturn[location] = 1;
    }
  }
  else
  {
    HallLow[location]++;
    HallHight[location] = 0;
    if(HallLow[location]>STEPMOTOR_IN_DEBOUNCE)
    {
      HallLow[location] = 0;
      HallReturn[location] = 0;
    }
    
  }
  return HallReturn[location];
  
}

void Syringe_Type()
{
#ifdef HP_INSTALL_EN    
  HP_Data.Hall_Top = IO_Shake(Top_HP,0x04);
  HP_Data.Hall_Bottom = IO_Shake(Bottom_HP,0x05);
  if((hp_param.AD_Type[0]*HPTypeLower<AD_Value[1].Value)&&(AD_Value[1].Value<(hp_param.AD_Type[0]*HPTypeUpper)))
  {
//    HP_Data.HP_Type=0;
//    HP_Data.HP_Flow = 0;   //步
    HP_Time_Flag[1] = 0;
    HP_Time_Flag[2] = 0;
    HP_Time_Flag[3] = 0;
    HP_Time_Flag[4] = 0;
    HP_Time_Flag[5] = 0;
    if(HP_Time_Flag[0] == 0)
    {
      HP_Time_Flag[0] = 1;
      TYPE_Time = 0;
    }
    if((HP_Time_Flag[0] == 1)&&(TYPE_Time>Syringe_Time))  //30s
    {
      HP_Time_Flag[0] = 0;
      HP_Data.HP_Type=0;
      HP_Data.HP_Flow = 0;   //步进电机一圈流量 ml/r 
    }
  }
  else if((hp_param.AD_Type[1]*HPTypeLower<AD_Value[1].Value)&&(AD_Value[1].Value<(hp_param.AD_Type[1]*HPTypeUpper)))
  {
//    HP_Data.HP_Type=10;
//    HP_Data.HP_Flow = 10/(float)hp_param.correct[0];   //步进电机一圈流量 ml/r 
    HP_Time_Flag[0] = 0;
    HP_Time_Flag[2] = 0;
    HP_Time_Flag[3] = 0;
    HP_Time_Flag[4] = 0;
    HP_Time_Flag[5] = 0;
    if(HP_Time_Flag[1] == 0)
    {
      HP_Time_Flag[1] = 1;
      TYPE_Time = 0;
    }
    if((HP_Time_Flag[1] == 1)&&(TYPE_Time>Syringe_Time))  //30s
    {
      HP_Time_Flag[1] = 0;
      HP_Data.HP_Type=10;
      HP_Data.HP_Flow = 10/(float)hp_param.correct[0];   //步进电机一圈流量 ml/r 
    }
  }
  else if((hp_param.AD_Type[2]*HPTypeLower<AD_Value[1].Value)&&(AD_Value[1].Value<(hp_param.AD_Type[2]*HPTypeUpper)))
  {
//    HP_Data.HP_Type=20;
//    HP_Data.HP_Flow = 20/(float)hp_param.correct[1];
    HP_Time_Flag[0] = 0;
    HP_Time_Flag[1] = 0;
    HP_Time_Flag[3] = 0;
    HP_Time_Flag[4] = 0;
    HP_Time_Flag[5] = 0;
    if(HP_Time_Flag[2] == 0)
    {
      HP_Time_Flag[2] = 1;
      TYPE_Time = 0;
    }
    if((HP_Time_Flag[2] == 1)&&(TYPE_Time>Syringe_Time))  //30s
    {
      HP_Time_Flag[2] = 0;
      HP_Data.HP_Type=20;
      HP_Data.HP_Flow = 20/(float)hp_param.correct[1];
    }
    
  }
  else if((hp_param.AD_Type[3]*HPTypeLower<AD_Value[1].Value)&&(AD_Value[1].Value<(hp_param.AD_Type[3]*HPTypeUpper)))
  {
//    HP_Data.HP_Type=30;
//    HP_Data.HP_Flow = 30/(float)hp_param.correct[2];
    HP_Time_Flag[0] = 0;    
    HP_Time_Flag[1] = 0;
    HP_Time_Flag[2] = 0;    
    HP_Time_Flag[4] = 0;
    HP_Time_Flag[5] = 0;
    if(HP_Time_Flag[3] == 0)
    {
      HP_Time_Flag[3] = 1;
      TYPE_Time = 0;
    }
    if((HP_Time_Flag[3] == 1)&&(TYPE_Time>Syringe_Time))  //30s
    {
      HP_Time_Flag[3] = 0;
      HP_Data.HP_Type=30;
      HP_Data.HP_Flow = 30/(float)hp_param.correct[2];
    }
    
  }
  else if((hp_param.AD_Type[4]*HPTypeLower<AD_Value[1].Value)&&(AD_Value[1].Value<(hp_param.AD_Type[4]*HPTypeUpper)))
  {
//    HP_Data.HP_Type=50;
//    HP_Data.HP_Flow = 50/(float)hp_param.correct[3];
    HP_Time_Flag[0] = 0;    
    HP_Time_Flag[1] = 0;
    HP_Time_Flag[2] = 0;    
    HP_Time_Flag[3] = 0;
    HP_Time_Flag[5] = 0;
    if(HP_Time_Flag[4] == 0)
    {
      HP_Time_Flag[4] = 1;
      TYPE_Time = 0;
    }
    if((HP_Time_Flag[4] == 1)&&(TYPE_Time>Syringe_Time))  //30s
    {
      HP_Time_Flag[4] = 0;
      HP_Data.HP_Type=50;
      HP_Data.HP_Flow = 50/(float)hp_param.correct[3]; 
    }
  }
  
  else   
  {
//    HP_Data.HP_Type=60;   //协议中增加重新校正注射器
//    HP_Data.HP_Flow = 20/(float)hp_param.correct[1];
    HP_Time_Flag[0] = 0;    
    HP_Time_Flag[1] = 0;
    HP_Time_Flag[2] = 0;    
    HP_Time_Flag[3] = 0;
    HP_Time_Flag[4] = 0;
    if(HP_Time_Flag[5] == 0)
    {
      HP_Time_Flag[5] = 1;
      TYPE_Time = 0;
    }
    if((HP_Time_Flag[5] == 1)&&(TYPE_Time>Syringe_Time))  //30s
    {
      HP_Time_Flag[5] = 0;
      HP_Data.HP_Type=60;   //协议中增加重新校正注射器
      HP_Data.HP_Flow = 20/(float)hp_param.correct[1];
    }
    
  }
  
#endif
  
}

void HP_FlowACC()
{
  for(u8 i = 0; i<(MOTOR_TOTAL-1) ; i++)
  {
    Integrated[i].Flow = (u32)(StepMotor_Data[i].ReadSpeed*StepMotor_Data[i].Ratio/10);
  }
  if(HPTmp.HP_Type!=HP_Data.HP_Type)          //肝素泵型号不一样,肝素流量累计
  {
    HPTmp.HP_Type = HP_Data.HP_Type;         //型号暂存
    HPTmp.Ratio  = HP_Data.HP_Flow;          //上一次的系数   ml/r
    HPTmp.Plus = StepMotor_Data[HP].Circle;  //上一次圈数
    HPTmp.Temp_Flow = HPTmp.Total_Flow;
  }
  HPTmp.Total_Flow = HPTmp.Temp_Flow + ((float)(StepMotor_Data[HP].Circle-HPTmp.Plus))*HPTmp.Ratio;
  
  
}

void HP_RUN_Over()
{
  if((HP_Data.Hall_Bottom == 0)&&(StepMotor_Data[HP].Enable == 1))   //肝素泵到底后底端压力传感器值迅速增加
  {           
    switch(HP_Bottom_State)
    {
    case 0 :
      HP_Bottom_Temp = AD_Value[0].Value;
      HP_Temp_Circle = StepMotor_Data[HP].Circle;
      HP_Bottom_State = 1;
      break;
    case 1 :
      if((StepMotor_Data[HP].Circle-HP_Temp_Circle)>3000)  //30圈
      {
        HP_Bottom_State = 2;
      }
      break;
    case 2:
      if((AD_Value[0].Value - HP_Bottom_Temp)>500)
      {
        StepMotor_Data[HP].Enable = 0;
        dSPIN_Hard_HiZ (); 
        HP_Data.HP_Over = 1;                   //肝素泵推注完成
        HP_Bottom_State = 0;
      }
      break;
    default:
      break;
    }
    
  }
}

void HP_RUN_Stop()
{
  if((StepMotor_Data[HP].SetFlow<1000)&&(StepMotor_Data[HP].Enable == 1)&&(StepMotor_Data[HP].Down_Mode != HP_CORRECT))  //肝素流量小于100ml/h
  {
    if(100 < HP_Time)  //1S读取一次
    {
      HP_Time = 0;      //计时清0
      if(AD_Value[0].Value!=HP_OldValue)
      {
        if(AD_Value[0].Value>HP_OldValue)
        {
          HP_Number++;  //比较次数自加
          
          if(HP_Number == 60)
          {               
            HP_TempValue = AD_Value[0].Value;
            
          }
//          if((HP_Number > 30)&&((AD_Value[0].Value-HP_TempValue)/StepMotor_Data[HP].Circle>200))
          if((HP_Number > 60)&&((AD_Value[0].Value-HP_TempValue)>800))  // 50,500
          {
            StepMotor_Data[HP].LockRotor = 1;    //肝素泵堵转 
            StepMotor_Data[HP].Enable = 0;
            
          }
          else
          {
            StepMotor_Data[HP].LockRotor = 0;    
          }
          
        }
        else
        {
          HP_Number = 0;
        }
        HP_OldValue = AD_Value[0].Value;
        
      }
      
      
    }
    
  }
  else if(((StepMotor_Data[HP].SetFlow<20000)&&(StepMotor_Data[HP].Enable == 1))&&(StepMotor_Data[HP].Down_Mode != HP_CORRECT))
  {
    switch(HP_Lock_State)
    {
    case 0 :
      HP_OldValue = AD_Value[0].Value;
      HP_Lock_State = 1;
      HP_Time = 0;      //计时清0
      break;
    case 1:
      if(((AD_Value[0].Value - HP_OldValue)>800)&&(HP_Time<1500))
      
      {
        StepMotor_Data[HP].LockRotor = 1;    //肝素泵堵转     
        StepMotor_Data[HP].Enable = 0;
        dSPIN_Hard_HiZ (); 
        HP_Lock_State = 0;
      }
      break;
    default:
      break;
    }
    
  }
  
}

void HP_Correction()
{
  
  static u8 Flash_Flag = 0;  //写入FLASH数据返回值
  if(StepMotor_Data[HP].Enable == 1 )
  {
    HP_Data.Up_Mode = HP_CORRECT_RUNING;        //肝素泵正在校正           
    
  }
  if((1==HP_Data.HP_Flag)&&(HP_Data.Hall_Bottom == 0))  //注射器型号校正
  {
    switch(HP_Data.HP_OrderType)
    {
    case 0:
      hp_param.AD_Type[0] = (u32)(AD_Value[1].Value);
      HP_Data.HP_Flag = 0;
      break;              
    case 10:
      hp_param.AD_Type[1] = (u32)(AD_Value[1].Value);
      HP_Data.HP_Flag = 0;
      break;
    case 20:
      hp_param.AD_Type[2] = (u32)(AD_Value[1].Value);
      HP_Data.HP_Flag = 0;
      break;
    case 30:
      hp_param.AD_Type[3] = (u32)(AD_Value[1].Value);
      HP_Data.HP_Flag = 0;
      break;
    case 50:
      hp_param.AD_Type[4] = (u32)(AD_Value[1].Value);
      HP_Data.HP_Flag = 0;
      break;
    default:
      HP_Data.HP_Flag = 0;
      break;
    }
    
    
  }
  
  //          if(HP_Data.HP_OrderType!=HP_Data.HP_Type)
  //          {
  //            HP_Data.Up_Mode = HP_CORRECT_ERROR;   //肝素泵校正时型号不匹配
  //            StepMotor_Data[HP].Down_Mode = HP_OVER;  //修改下行标志，为了重新校正肝素泵
  //          }
  if(1==HP_Data.HP_Over)   //推注到底后
  {
    StepMotor_Data[HP].Enable = 0;
    dSPIN_Hard_HiZ (); 
    StepMotor_Data[HP].Down_Mode = HP_NORMAL_RUN;   //校正完成
    switch(HP_Data.HP_OrderType)
    {
    case 10:
      if(((LowerLimit*Correct_Length10)<(StepMotor_Data[HP].Circle))&&((StepMotor_Data[HP].Circle)<(Correct_Length10*UpperLimit)))
      {
        hp_param.correct[0] = (u16)(StepMotor_Data[HP].Circle/100);//注射器校正圈数
      }
      else
      {
        hp_param.correct[0] = Correct_Length10;   //默认值
      }              
      break;
    case 20:
      if(((LowerLimit*Correct_Length20)<(StepMotor_Data[HP].Circle))&&((StepMotor_Data[HP].Circle)<(Correct_Length20*UpperLimit)))
      {
        hp_param.correct[1] = (u16)(StepMotor_Data[HP].Circle/100);//注射器校正圈数
      }
      else
      {
        hp_param.correct[1] = Correct_Length20; 
      }
      break;
    case 30:
      if(((LowerLimit*Correct_Length30)<(StepMotor_Data[HP].Circle))&&((StepMotor_Data[HP].Circle)<(Correct_Length30*UpperLimit)))
      {
        hp_param.correct[2] = (u16)(StepMotor_Data[HP].Circle/100);//注射器校正圈数
      }
      else
      {
        hp_param.correct[2] = Correct_Length30; 
      }
      break;
    case 50:
      if(((LowerLimit*Correct_Length50)<(StepMotor_Data[HP].Circle))&&((StepMotor_Data[HP].Circle)<(Correct_Length50*UpperLimit)))
      {
        hp_param.correct[3] = (u16)(StepMotor_Data[HP].Circle/100);//注射器校正圈数
      }
      else
      {
        hp_param.correct[3] = Correct_Length50;  
      }
      break;
    default:
      HP_Data.Up_Mode = HP_CORRECT_ERROR;   //肝素泵校正时型号不匹配
      break;
      
    }
    Flash_Flag = Flash_Save_Param(0,(u8 *)&hp_param,sizeof(hp_param));  //校正数据写入FLASH
    if(0 == Flash_Flag)
    {
      HP_Data.Up_Mode = HP_CORRECT_PASS;  //肝素泵校正通过 
    }
    else
    {
      HP_Data.Up_Mode = HP_CORRECT_ERROR;  //肝素泵校正异常
    }
    
  } 
}
void HP_Self_Test()
{
  if(ENABLE == StepMotor_Data[HP].Enable)         //开命令
  {
    
    if(dSPIN_Busy_HW())                                 //读取BUSY标志
    {
      StepMotor_Data[HP].Up_Mode =  SELF_TEST_RUNING;     //正在自检 
    }
    if((HP_Data.Hall_Bottom == 0)&&(StepMotor_Data[HP].SetDir== 0))      //肝素泵未安装时，可以自检通过？
    {
      dSPIN_Hard_HiZ ();
      FlowClearZero(HP);
      StepMotor_Data[HP].SetDir = 1;
      dSPIN_Run(StepMotor_Data[HP].SetDir, Speed_Steps_to_Par(50*HPSELF_Speed/15));  //肝素泵反转
      
    }
    if((StepMotor_Data[HP].SetDir == 1)&&(StepMotor_Data[HP].Circle>10000))//自检反转80圈后
    {
      StepMotor_Data[HP].SetDir = 0;
      dSPIN_Hard_HiZ ();
      StepMotor_Data[HP].Enable = 0;
      StepMotor_Data[HP].Up_Mode = SELF_TEST_PASS;      //自检完成
      StepMotor_Data[HP].Down_Mode = SELF_Finish;
      
    }
         
  }
  if((StepMotor_Data[HP].LockRotor==1)||(StepMotor_Data[HP].Alert==1)||(Motor_Time>40000))  //自检时间超过220S
  {             
    StepMotor_Data[HP].Enable = 0;
    dSPIN_Hard_HiZ (); 
    StepMotor_Data[HP].Up_Mode = SELF_TEST_ERROR;      //自检错误
    
  }  
  
}