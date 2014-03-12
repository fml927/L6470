/**
*******************************************************
* @file    AD7799.h 

*******************************************************
**/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __user_AD7799_H
#define __user_AD7799_H

/************* ������ع��������Ľṹ��    *******************************/
typedef struct    //���в����ṹ�� - ������ÿ��ͨ��
{
    u8    RefMode;   //1=��׼���źŻ���;0=δ����
    u8    SetGain;    //��һ��ADC���趨���棺0��7 ��= 1��128��
    u8    CurrGain;   //��ǰADֵ�����棺0��7 ��= 1��128��
    s32   CurrValue;  //��ǰ���µ�ADֵ,��GAIN,�����Զ�����
}ADC_RUN_Def;

typedef struct    //�ⲿ�ӿڽṹ�� - ������ÿ��ͨ��
{
    u8    Status;     //0=ADֵ��Ч;1=�õ����µ�ADCת�����,�û���ȡADCֵ������0�ñ�־
    float Value;      //���µ�ADֵ:1)�������˲�����;2)�ѳ��ԷŴ���,��ԭʼ�źŵ�ADֵ
}ADC_VAL_Def;

/************* �ⲿ�û��ӿ�    *******************************/

/**************** AD7799ʹ�����ݼ�ÿƬADC��ͨ����  **********/
#define AD7799_PCS                 1                       //AD7799оƬ����(1~4)
#define AD7799_CHANN_USE1          2                       //��1ƬAD7799��ͨ����(1~3)
#define AD7799_CHANN_USE2          0                       //��2ƬAD7799��ͨ����(0~3,��оƬδʹ��ʱ������Ϊ0)
#define AD7799_CHANN_USE3          0                       //��3ƬAD7799��ͨ����(0~3,��оƬδʹ��ʱ������Ϊ0)
#define AD7799_CHANN_USE4          0                       //��4ƬAD7799��ͨ����(0~3,��оƬδʹ��ʱ������Ϊ0)
#define AD7799_CHANN_USE           2        //�ܵ�ͨ��������AD7799ͨ����֮��
//#define AD7799_CHANN_USE  AD7799_CHANN_USE1+AD7799_CHANN_USE2+AD7799_CHANN_USE3+AD7799_CHANN_USE4

//����AD_Value[]������ͨ�����,��0��ʼ��š�ע�⣺�����ڶ�Ƭ�ı��
typedef enum   
{
  ADSF = 0,   // 
  ADSB,       //
  ADBP,       //
  ADOTH,      //
}AdcChName;

/**************** ADCƬѡ����CS ���GPIO  --���޸� --����Ϊ4�� **********/
#define  ADC1_CS_PORT    GPIOD   //PD1
#define  ADC2_CS_PORT    0   //
#define  ADC3_CS_PORT    0   //
#define  ADC4_CS_PORT    0   //
#define  ADC1_CS_PIN     GPIO_Pin_1
#define  ADC2_CS_PIN     0
#define  ADC3_CS_PIN     0
#define  ADC4_CS_PIN     0
/**************** ADC SPIʱ������SCLK ���GPIO  --���޸� --����Ϊ4�� **********/
#define  ADC1_SCLK_PORT    GPIOD   //PD2
#define  ADC2_SCLK_PORT    0   //
#define  ADC3_SCLK_PORT    0   //
#define  ADC4_SCLK_PORT    0   //
#define  ADC1_SCLK_PIN     GPIO_Pin_2
#define  ADC2_SCLK_PIN     0
#define  ADC3_SCLK_PIN     0
#define  ADC4_SCLK_PIN     0
/**************** ADC������������DIN ���GPIO  --���޸� --����Ϊ4�� **********/
#define  ADC1_DIN_PORT    GPIOD   //PD3
#define  ADC2_DIN_PORT    0   //
#define  ADC3_DIN_PORT    0   //
#define  ADC4_DIN_PORT    0   //
#define  ADC1_DIN_PIN     GPIO_Pin_3
#define  ADC2_DIN_PIN     0
#define  ADC3_DIN_PIN     0
#define  ADC4_DIN_PIN     0
/**************** ADC�����������DOUT ����GPIO  --���޸� --����Ϊ4�� **********/
#define  ADC1_DOUT_PORT    GPIOD   //PD4
#define  ADC2_DOUT_PORT    0   //
#define  ADC3_DOUT_PORT    0   //
#define  ADC4_DOUT_PORT    0   //
#define  ADC1_DOUT_PIN     GPIO_Pin_4
#define  ADC2_DOUT_PIN     0
#define  ADC3_DOUT_PIN     0
#define  ADC4_DOUT_PIN     0

/***********  ����ADC�������˲�����      ********** 
* 2���˲��㷨��ϣ���λֵ�˲����͵���ƽ���˲������ֳƻ���ƽ���˲���)
* ��λֵ�˲���: ��ĳһ��ͨ�����Ӳɼ�SAMPLE_CNT�κ�(�����л�����һ��ͨ��)������Щ�������򣬲�ȡ���м�ֵ���õ���һ�ֵĲɼ������
* ����ƽ���˲���������λֵ�˲��õ������ݣ��滻��������ɵ����ݣ�Ȼ��Զ����е�����(��Ч)������ƽ��ֵ
*/
#define  MEDIAN_FILTERING_BUFSIZE 1     //��λֵ�˲��ģ����������ݲɼ�����. �Ƽ�ֵ��3,4,5,6,7,8,9,10
#define  MOVING_AVERAGE_BUFSIZE   3     //����ƽ���˲��Ķ��г���. �Ƽ���������N=12��ѹ����N=4��Һ�棬N=4~12���¶ȣ�N=1~4

#define AD7799_RATE               AD7799_MODE_16_7HZ //AD7799_MODE_10HZ        //�趨ADC��ת������
#define AD7799_GAIN               AD7799_CONFIG_GAIN_AUTO  //�趨ADC������
#define ADC7799_RD_CYCLE          1000                //ADC��ȡ����ʱ��,Ҫ��ΪAD7799ת������(1/AD7799_RATE)��(1/10~1/3)
#define ADC7799_ERR_CYCLE         300000              //ADC�쳣�ж�����ʱ��

//����AD7799��/˫���Ա���
#define AD7799_BIPOLAR            //AD7799������˫���Ա��뷽ʽ;�������ڵ����Ա��뷽ʽ
//���嵥/˫���Ա����������ADֵ
#ifdef  AD7799_BIPOLAR
#define REF_FULL_VAL              8388607     //23λ: =2^23-1
#define REF_FULL_VAL_FLOAT        8388607.0   //23λ: =2^23-1
#else
#define REF_FULL_VAL              16777215     //24λ: =2^24-1
#define REF_FULL_VAL_FLOAT        16777215.0   //24λ: =2^24-1
#endif


extern ADC_VAL_Def AD_Value[];              //��ͨ��������ADֵ
void   ADC7799_Init(void);                  //AD7799��ʼ��.   ���������ʼ������ε���    
void   ADC_Auto_Conversion(void);           //AD7799�Զ�ת������. ����ѭ�������е���

/************* AD7799�ڲ��궨��,��������    *******************************/

/**************  AD7799��MODE�Ĵ�������ز������� ********************/
//ת��ģʽ����λ: bit15,14,13
#define AD7799_MODE_CONTINUE  0x0000    //v����ת��ģʽ(Ĭ��)
#define AD7799_MODE_SINGLE    0x2000    //����ת��ģʽ
//PSW���ؿ���λ�� bit12
#define AD7799_MODE_PSW_ON    0x1000    //��PSW����
#define AD7799_MODE_PSW_OFF   0x0000    //�ر�PSW����
//bit<12:4>δʹ��,������Ϊ0 
//ת�����ʿ���λ��bit3,2,1,0
#define AD7799_MODE_4_17HZ    0x000f    //ת������=4.17hz;  ����:74db(50,60hz)
#define AD7799_MODE_6_25HZ    0x000e    //ת������=6.25hz;  ����:72db(50,60hz)
#define AD7799_MODE_8_33HZ    0x000d    //ת������=8.33hz;  ����:70db(50,60hz)
#define AD7799_MODE_10HZ      0x000c    //ת������=10hz  ;  ����:69db(50,60hz)
#define AD7799_MODE_12_5HZ    0x000b    //ת������=12.5hz;  ����:66db(50,60hz)
#define AD7799_MODE_16_7HZ    0x000a    //ת������=16.7hz;  ����:65db(50,60hz)
#define AD7799_MODE_16_50HZ   0x0009    //ת������=16.7hz;  ����:80db(��50hz)
#define AD7799_MODE_19_6HZ    0x0008    //ת������=19.6hz;  ����:90db(��60hz)
#define AD7799_MODE_50HZ      0x0005    //ת������=50hz;    ����:-
#define AD7799_MODE_470HZ     0x0001    //ת������=470hz;   ����:-

/**************  AD7799��CONFIG�Ĵ�������ز������� ********************/
//bit15,14,11,7,6,3δʹ�ã�
//BIT13-Ϊ100nA����Դʹ��(=1),Ĭ��Ϊ0(�ر�)������δ����
//��������źŵĵ�/˫���Ա������: bit12
#define AD7799_CONFIG_BIPOLAR     0x0000    //˫���Ա���(Ĭ��)
#define AD7799_CONFIG_UNIPOLAR    0x1000    //v�����Ա���
//����ѡ��λ: bit10,9,8
#define AD7799_CONFIG_GAIN_1      0x0000    //����=1(�Ǳ�Ŵ�������)
#define AD7799_CONFIG_GAIN_2      0x0100    //����=2(�Ǳ�Ŵ�������)
#define AD7799_CONFIG_GAIN_4      0x0200    //����=4
#define AD7799_CONFIG_GAIN_8      0x0300    //����=8
#define AD7799_CONFIG_GAIN_16     0x0400    //����=16
#define AD7799_CONFIG_GAIN_32     0x0500    //����=32
#define AD7799_CONFIG_GAIN_64     0x0600    //����=64
#define AD7799_CONFIG_GAIN_128    0x0700    //����=128
#define AD7799_CONFIG_GAIN_AUTO   0X5A5A    //�Զ����� - AD7799Ӳ���޸ù��ܣ��ù���������Զ�ʵ��
//��׼��ѹ���λ: bit5
#define AD7799_CONFIG_REFDET_EN   0x0020    //��׼��ѹ��⹦����Ч�����ⲿ��׼��ѹ��·��С��0.5Vʱ,״̬�Ĵ�����NOXREFλ��λ
#define AD7799_CONFIG_REFDET_DIS  0x0000    //��׼��ѹ��⹦�ܽ���
//BUFλ: bit4
#define AD7799_CONFIG_BUF_EN      0x0010    //v���幤��ģʽ
#define AD7799_CONFIG_BUF_DIS     0x0000    //�޻��幤��ģʽ
//ͨ��ѡ��λ: bit2,1,0
#define AD7799_CONFIG_AIN1        0x0000    //AIN1�������
#define AD7799_CONFIG_AIN2        0x0001    //AIN2�������
#define AD7799_CONFIG_AIN3        0x0002    //AIN3�������



void AD7799_CS_Pin_Configuration(u8 chipn);
void AD7799_SCLK_Pin_Configuration(u8 chipn);
void AD7799_DIN_Pin_Configuration(u8 chipn);
void AD7799_DOUT_Pin_Configuration(u8 chipn);
void Wr1Byte7799(u8 chipn,u8 data);
void Delay_7799(u16 timecount);

u8 Rd1Byte7799(u8 chipn);
u8 read_status_reg7799(u8 chipn);
void write_reg7799(u8 chipn,u8 xcomm, u8 xlen, u8 *s);
u32 read_data_reg7799(u8 chipn);
void Config_AD7799(u8 chipn,u8 chn);
void auto_gain(u8 chn);


#endif
/******************* (C) COPYRIGHT 2008 SWS *****END OF FILE*******************/
