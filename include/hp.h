#ifndef __HP_H
#define __HP_H

#define Contrast(value) (((value*0.9)<value)&&(value<(value*1.1)) ? 1:0)
#define HPSELF_Speed 100        //���ر��Լ�ʱת��
#define Check_Speed 100        //���ر�У��ʱת��

#define HP_NORMAL_RUN      0    //������������ģʽ
#define HP_FAST_FORWARD    1    //���ؿ��ģʽ
#define HP_SELF_TEST       3    //�����Լ�ģʽ
#define HP_CORRECT         4    //���ر�У��ģʽ
#define HP_OVER            5    //���ر�У����� ��Ϊ����ģʽ

#define Syringe_Time       500  //ע������� ���ӳ�ʱ��

#define HP_CORRECT_DEFAULT     0    //���ر�У��   
#define HP_CORRECT_RUNING      1    //���ر�����У��
#define HP_CORRECT_PASS        2    //���ر�У��ͨ��
#define HP_CORRECT_ERROR       3    //���ر�У���쳣 

extern float CoeffIcient_HP;

#define Correct_Length10 280            //10mlע����У��Ȧ��(1���׳���=0.2Ȧ)  282
#define Correct_Length20 374            //20mlע����У��Ȧ��(1���׳���=0.2Ȧ)  349
#define Correct_Length30 376            //30mlע����У��Ȧ��(1���׳���=0.2Ȧ)  378
#define Correct_Length50 402            //50mlע����У��Ȧ��(1���׳���=0.2Ȧ)  373

#define LowerLimit 80  //���ر�У������
#define UpperLimit 120 //���ر�У������


#define HPTypeLower 0.9  //�����ͺ�����
#define HPTypeUpper 1.1 //�����ͺ�����

#define Correct_Flow10  0.03348     //�������һȦ���� ml/r
#define Correct_Flow20  0.0555
#define Correct_Flow30  0.0831
#define Correct_Flow50  0.133

#define   Correct_Length10_B 50      //10mL��� ��⵽λ����Ҫ��ע10mm   
#define   Correct_Length20_B 45      //20mL��� ��⵽λ����Ҫ��ע9mm
#define   Correct_Length30_B 47      //30mL��� ��⵽λ����Ҫ��ע9.5mm
#define   Correct_Length50_B 8       //50mL��� ��⵽λ����Ҫ��ע1.5mm

typedef struct struct_Hp_param  
{  
  u16 correct[4];   //4     10ml��20��30��50mlע����У��Ȧ��    
  u32 AD_Type[5]; //8    0ml 10ml��20��30��50ml ��ȡ��ADֵ   
} HP_PARAM;

#define PARAM_SIZE 18

extern HP_PARAM hp_param;

typedef struct   
{  
  u8 HP_OrderType;    //��λ���·���ע�����ͺ�
  u8 HP_Compare;       //ע�����ͺŶԱȽ��  

  float HP_Flow;
  u8 HP_Length;       //��⵽λ����Ҫ��ע���Ȼ����Ȧ��
  u8 HP_Check;       //���ر�У������
  u8 HP_Type;         //�������ע�����ͺ�
  u8 Up_Mode;        //���ر�У������ģʽ
  
  u8 Hall_Top;         //���˹�翪��״̬
  u8 Hall_Bottom;      //�Ͷ˹�翪��״̬
  u8 Hall_State;       //3���ͺŹ�翪��״̬����
  u8 HP_Over;          //���ر���ע���
  
  u8 HP_Flag;         //���ر�У����־
  
  
} HP_Work;
extern HP_Work HP_Data;

typedef struct
{
    float  Ratio;               //ϵ��
    u32  Plus;              //�ݴ�������
    u32  HP_Type;           //���ر��ͺ��ݴ�
    float  Total_Flow;         //������
    float  Temp_Flow;          //�����ݴ�
}HPFlow;
extern HPFlow HPTmp;

#define  Top_HP     GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)   //ע�����Ƿ�װ��λ��� PD1
#define  Bottom_HP  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)   //���ص��׼��  PD6
//#define  SyringeMode_S1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) //ע�����ͺż�� PC12
//#define  SyringeMode_S2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) //ע�����ͺż�� PD0
//#define  SyringeMode_S3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) //ע�����ͺż�� PC11
#define  Speed_Hall      GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12) //�ٶȹ�翪��  PA12

#define TYPE_0       6500000     //ADֵС�ڷ�Χ   6.38  3.794V   6.35 3.784V
#define TYPE_10      6100000     //ADֵС�ڷ�Χ   6.61  3.927V   5.82 3.452V
#define TYPE_20      5500000     //ADֵС�ڷ�Χ  6.89  4.095V    5.03 2.997V
#define TYPE_30      4500000     //ADֵС�ڷ�Χ                  4.15 2.478V
#define TYPE_50      3500000     //ADֵС�ڷ�Χ  7.31  4.337V    3.07 1.916V
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