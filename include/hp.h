#ifndef __HP_H
#define __HP_H

#define Contrast(value) (((value*0.9)<value)&&(value<(value*1.1)) ? 1:0)
#define HPSELF_Speed 100        //肝素泵自检时转速
#define Check_Speed 100        //肝素泵校正时转速

#define HP_NORMAL_RUN      0    //肝素正常运行模式
#define HP_FAST_FORWARD    1    //肝素快进模式
#define HP_SELF_TEST       3    //肝素自检模式
#define HP_CORRECT         4    //肝素泵校正模式
#define HP_OVER            5    //肝素泵校正完成 置为其他模式

#define Syringe_Time       500  //注射器检测 ，延迟时间

#define HP_CORRECT_DEFAULT     0    //肝素泵校正   
#define HP_CORRECT_RUNING      1    //肝素泵正在校正
#define HP_CORRECT_PASS        2    //肝素泵校正通过
#define HP_CORRECT_ERROR       3    //肝素泵校正异常 

extern float CoeffIcient_HP;

#define Correct_Length10 280            //10ml注射器校正圈数(1毫米长度=0.2圈)  282
#define Correct_Length20 374            //20ml注射器校正圈数(1毫米长度=0.2圈)  349
#define Correct_Length30 376            //30ml注射器校正圈数(1毫米长度=0.2圈)  378
#define Correct_Length50 402            //50ml注射器校正圈数(1毫米长度=0.2圈)  373

#define LowerLimit 80  //肝素泵校正下限
#define UpperLimit 120 //肝素泵校正上限


#define HPTypeLower 0.9  //肝素型号下限
#define HPTypeUpper 1.1 //肝素型号上限

#define Correct_Flow10  0.03348     //步进电机一圈流量 ml/r
#define Correct_Flow20  0.0555
#define Correct_Flow30  0.0831
#define Correct_Flow50  0.133

#define   Correct_Length10_B 50      //10mL情况 检测到位后需要推注10mm   
#define   Correct_Length20_B 45      //20mL情况 检测到位后需要推注9mm
#define   Correct_Length30_B 47      //30mL情况 检测到位后需要推注9.5mm
#define   Correct_Length50_B 8       //50mL情况 检测到位后需要推注1.5mm

typedef struct struct_Hp_param  
{  
  u16 correct[4];   //4     10ml、20、30、50ml注射器校正圈数    
  u32 AD_Type[5]; //8    0ml 10ml、20、30、50ml 读取的AD值   
} HP_PARAM;

#define PARAM_SIZE 18

extern HP_PARAM hp_param;

typedef struct   
{  
  u8 HP_OrderType;    //上位机下发的注射器型号
  u8 HP_Compare;       //注射器型号对比结果  

  float HP_Flow;
  u8 HP_Length;       //检测到位后需要推注长度换算的圈数
  u8 HP_Check;       //肝素泵校正开关
  u8 HP_Type;         //霍尔检测注射器型号
  u8 Up_Mode;        //肝素泵校正上行模式
  
  u8 Hall_Top;         //顶端光电开关状态
  u8 Hall_Bottom;      //低端光电开关状态
  u8 Hall_State;       //3个型号光电开关状态相与
  u8 HP_Over;          //肝素泵推注完成
  
  u8 HP_Flag;         //肝素泵校正标志
  
  
} HP_Work;
extern HP_Work HP_Data;

typedef struct
{
    float  Ratio;               //系数
    u32  Plus;              //暂存脉冲数
    u32  HP_Type;           //肝素泵型号暂存
    float  Total_Flow;         //总流量
    float  Temp_Flow;          //流量暂存
}HPFlow;
extern HPFlow HPTmp;

#define  Top_HP     GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)   //注射器是否安装到位检测 PD1
#define  Bottom_HP  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)   //肝素到底检测  PD6
//#define  SyringeMode_S1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) //注射器型号检测 PC12
//#define  SyringeMode_S2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) //注射器型号检测 PD0
//#define  SyringeMode_S3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) //注射器型号检测 PC11
#define  Speed_Hall      GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12) //速度光电开关  PA12

#define TYPE_0       6500000     //AD值小于范围   6.38  3.794V   6.35 3.784V
#define TYPE_10      6100000     //AD值小于范围   6.61  3.927V   5.82 3.452V
#define TYPE_20      5500000     //AD值小于范围  6.89  4.095V    5.03 2.997V
#define TYPE_30      4500000     //AD值小于范围                  4.15 2.478V
#define TYPE_50      3500000     //AD值小于范围  7.31  4.337V    3.07 1.916V
#define TYPE         3000000 

extern u16 TYPE_Time;
extern void HP_CheckInfo();
extern void Syringe_Type();
extern u8 IO_Shake(u8 hallstate,u8 location);
extern void HP_FlowACC();
extern void HP_RUN_Over();
extern void HP_RUN_Stop();
extern void HP_Correction();
extern void HP_Self_Test();
#endif 