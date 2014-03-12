/***************************************************************************
-----------文件信息-----------
名称:SYS.h
所属模块:SWSII型步进电机驱动器
创建:刘旭东
创建日期:2008.8.30
修订记录:Vser1.0
***************************************************************************/
#ifndef __MAIN_CCOND_H
#define __MAIN_CCOND_H

#define LED_HP_ON     GPIO_ResetBits(GPIOE,GPIO_Pin_6)  //PE6-HP工作状态指灯
#define LED_HP_OFF    GPIO_SetBits(GPIOE,GPIO_Pin_6) 
#define LED_SFP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_5)  //PE5-SFP工作状态指灯
#define LED_SFP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_5) 
#define LED_SBP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_4)  //PE4-SBP工作状态指灯
#define LED_SBP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_4) 
#define LED_SPP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_3)  //PE3-SPP工作状态指灯
#define LED_SPP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_3) 
#define LED_UFP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_2)  //PE2-UFP工作状态指灯
#define LED_UFP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_2) 
#define LED_485_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_1)  //PE1-485工作状态指灯
#define LED_485_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_1) 
#define LED_RUN_ON    GPIO_ResetBits(GPIOA,GPIO_Pin_0)  //PD15-RUN工作状态指灯
#define LED_RUN_OFF   GPIO_SetBits(GPIOA,GPIO_Pin_0) 


extern u8  Work_Mode;		   //工作模式 
extern u8  K_ADJ; 
extern u8 RS485_STDL;
extern u8 RS485_STDH;  
extern u16 Comm_CNT;     //485通讯次数计数
extern u8  Comm_State;   //485通讯状态
extern u16   LED_COM_Cnt;

struct RUN_PAR_A
{  
  u16 Flux;       //A液流量10倍
  u16 K_Flux;    //A泵管系数1000倍
  u8 Set_Conduct;//A目标电导值10倍
  u16 Fack_Conduct; //A电导值100倍
  u16 Temp;      //A温度10倍
  u16  Set_RPM;  //A泵参考转速X10
  u8  Fack_RPM;  //霍尔转速
  u16 Work_RPM;
  u16 Freq;      //A频率Hz
};



/*struct RS485_PAR
{
u16 Conduct_A;
u16 Conduct_B;
u16 Temp_A;
u16 Temp_B;
u16 Freq_A;
u16 Freq_B;
	  };*/ 
extern u8 TC_ADJ;

extern struct RUN_PAR_A Run_Par_A;
extern struct RUN_PAR_A Run_Par_B;
extern u8 SpiHead_OK;
extern u8 SPI_Buf[18];
extern u8 SPI_CNT;
extern u8 SysState;
extern u8 Back_Mode;
extern u8 Int_Conduct_A,Int_Conduct_B;
extern u8 SysErr;
extern u8 Data_OK;
extern u8 Confect_Start;
extern u8 Flag_3S;
extern u8 Flag_Sec;
extern u8 Flux_Flag;
extern void TIM2_Configuration_A(void);
extern void TIM2_Configuration_B(void);
extern void Delay(u16 uS);
extern void System_Init(void);
//void StepMotor_Set_DCY_A(u16 speed);
//void StepMotor_Set_DCY_B(u16 speed);



void check_lockrotor(void);

#endif