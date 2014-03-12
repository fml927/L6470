/**
*******************************************************
* @file    AD7799.h 

*******************************************************
**/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __user_AD7799_H
#define __user_AD7799_H

/************* 定义相关工作参数的结构体    *******************************/
typedef struct    //运行参数结构体 - 适用于每个通道
{
    u8    RefMode;   //1=基准与信号互换;0=未互换
    u8    SetGain;    //下一次ADC的设定增益：0～7 （= 1～128）
    u8    CurrGain;   //当前AD值的增益：0～7 （= 1～128）
    s32   CurrValue;  //当前最新的AD值,含GAIN,用于自动增益
}ADC_RUN_Def;

typedef struct    //外部接口结构体 - 适用于每个通道
{
    u8    Status;     //0=AD值无效;1=得到最新的ADC转换结果,用户获取ADC值后，需清0该标志
    float Value;      //最新的AD值:1)经过了滤波处理;2)已除以放大倍数,即原始信号的AD值
}ADC_VAL_Def;

/************* 外部用户接口    *******************************/

/**************** AD7799使用数据及每片ADC的通道数  **********/
#define AD7799_PCS                 1                       //AD7799芯片数量(1~4)
#define AD7799_CHANN_USE1          2                       //第1片AD7799的通道数(1~3)
#define AD7799_CHANN_USE2          0                       //第2片AD7799的通道数(0~3,该芯片未使用时必须置为0)
#define AD7799_CHANN_USE3          0                       //第3片AD7799的通道数(0~3,该芯片未使用时必须置为0)
#define AD7799_CHANN_USE4          0                       //第4片AD7799的通道数(0~3,该芯片未使用时必须置为0)
#define AD7799_CHANN_USE           2        //总的通道数：各AD7799通道数之和
//#define AD7799_CHANN_USE  AD7799_CHANN_USE1+AD7799_CHANN_USE2+AD7799_CHANN_USE3+AD7799_CHANN_USE4

//定义AD_Value[]变量的通道编号,从0开始编号。注意：包括第二片的编号
typedef enum   
{
  ADSF = 0,   // 
  ADSB,       //
  ADBP,       //
  ADOTH,      //
}AdcChName;

/**************** ADC片选引脚CS 输出GPIO  --可修改 --必须为4个 **********/
#define  ADC1_CS_PORT    GPIOD   //PD1
#define  ADC2_CS_PORT    0   //
#define  ADC3_CS_PORT    0   //
#define  ADC4_CS_PORT    0   //
#define  ADC1_CS_PIN     GPIO_Pin_1
#define  ADC2_CS_PIN     0
#define  ADC3_CS_PIN     0
#define  ADC4_CS_PIN     0
/**************** ADC SPI时钟引脚SCLK 输出GPIO  --可修改 --必须为4个 **********/
#define  ADC1_SCLK_PORT    GPIOD   //PD2
#define  ADC2_SCLK_PORT    0   //
#define  ADC3_SCLK_PORT    0   //
#define  ADC4_SCLK_PORT    0   //
#define  ADC1_SCLK_PIN     GPIO_Pin_2
#define  ADC2_SCLK_PIN     0
#define  ADC3_SCLK_PIN     0
#define  ADC4_SCLK_PIN     0
/**************** ADC数据输入引脚DIN 输出GPIO  --可修改 --必须为4个 **********/
#define  ADC1_DIN_PORT    GPIOD   //PD3
#define  ADC2_DIN_PORT    0   //
#define  ADC3_DIN_PORT    0   //
#define  ADC4_DIN_PORT    0   //
#define  ADC1_DIN_PIN     GPIO_Pin_3
#define  ADC2_DIN_PIN     0
#define  ADC3_DIN_PIN     0
#define  ADC4_DIN_PIN     0
/**************** ADC数据输出引脚DOUT 输入GPIO  --可修改 --必须为4个 **********/
#define  ADC1_DOUT_PORT    GPIOD   //PD4
#define  ADC2_DOUT_PORT    0   //
#define  ADC3_DOUT_PORT    0   //
#define  ADC4_DOUT_PORT    0   //
#define  ADC1_DOUT_PIN     GPIO_Pin_4
#define  ADC2_DOUT_PIN     0
#define  ADC3_DOUT_PIN     0
#define  ADC4_DOUT_PIN     0

/***********  定义ADC采样的滤波参数      ********** 
* 2种滤波算法结合：中位值滤波法和递推平均滤波法（又称滑动平均滤波法)
* 中位值滤波法: 对某一个通道连接采集SAMPLE_CNT次后(才能切换到下一个通道)，对这些数据排序，并取出中间值，得到这一轮的采集结果。
* 递推平均滤波法：把中位值滤波得到的数据，替换队列中最旧的数据，然后对队列中的所有(有效)数据求平均值
*/
#define  MEDIAN_FILTERING_BUFSIZE 1     //中位值滤波的（连续）数据采集次数. 推荐值：3,4,5,6,7,8,9,10
#define  MOVING_AVERAGE_BUFSIZE   3     //递推平均滤波的队列长度. 推荐，流量：N=12；压力：N=4；液面，N=4~12；温度，N=1~4

#define AD7799_RATE               AD7799_MODE_16_7HZ //AD7799_MODE_10HZ        //设定ADC的转换速率
#define AD7799_GAIN               AD7799_CONFIG_GAIN_AUTO  //设定ADC的增益
#define ADC7799_RD_CYCLE          1000                //ADC读取周期时间,要求为AD7799转换周期(1/AD7799_RATE)的(1/10~1/3)
#define ADC7799_ERR_CYCLE         300000              //ADC异常判断周期时间

//定义AD7799单/双极性编码
#define AD7799_BIPOLAR            //AD7799工作在双极性编码方式;否则工作在单极性编码方式
//定义单/双极性编码的满量程AD值
#ifdef  AD7799_BIPOLAR
#define REF_FULL_VAL              8388607     //23位: =2^23-1
#define REF_FULL_VAL_FLOAT        8388607.0   //23位: =2^23-1
#else
#define REF_FULL_VAL              16777215     //24位: =2^24-1
#define REF_FULL_VAL_FLOAT        16777215.0   //24位: =2^24-1
#endif


extern ADC_VAL_Def AD_Value[];              //各通道的最新AD值
void   ADC7799_Init(void);                  //AD7799初始化.   在主程序初始化程序段调用    
void   ADC_Auto_Conversion(void);           //AD7799自动转换程序. 在主循环程序中调用

/************* AD7799内部宏定义,函数声明    *******************************/

/**************  AD7799的MODE寄存器的相关参数定义 ********************/
//转换模式控制位: bit15,14,13
#define AD7799_MODE_CONTINUE  0x0000    //v连续转换模式(默认)
#define AD7799_MODE_SINGLE    0x2000    //单次转换模式
//PSW开关控制位： bit12
#define AD7799_MODE_PSW_ON    0x1000    //打开PSW开关
#define AD7799_MODE_PSW_OFF   0x0000    //关闭PSW开关
//bit<12:4>未使用,必须置为0 
//转换速率控制位：bit3,2,1,0
#define AD7799_MODE_4_17HZ    0x000f    //转换速率=4.17hz;  抑制:74db(50,60hz)
#define AD7799_MODE_6_25HZ    0x000e    //转换速率=6.25hz;  抑制:72db(50,60hz)
#define AD7799_MODE_8_33HZ    0x000d    //转换速率=8.33hz;  抑制:70db(50,60hz)
#define AD7799_MODE_10HZ      0x000c    //转换速率=10hz  ;  抑制:69db(50,60hz)
#define AD7799_MODE_12_5HZ    0x000b    //转换速率=12.5hz;  抑制:66db(50,60hz)
#define AD7799_MODE_16_7HZ    0x000a    //转换速率=16.7hz;  抑制:65db(50,60hz)
#define AD7799_MODE_16_50HZ   0x0009    //转换速率=16.7hz;  抑制:80db(仅50hz)
#define AD7799_MODE_19_6HZ    0x0008    //转换速率=19.6hz;  抑制:90db(仅60hz)
#define AD7799_MODE_50HZ      0x0005    //转换速率=50hz;    抑制:-
#define AD7799_MODE_470HZ     0x0001    //转换速率=470hz;   抑制:-

/**************  AD7799的CONFIG寄存器的相关参数定义 ********************/
//bit15,14,11,7,6,3未使用；
//BIT13-为100nA电流源使能(=1),默认为0(关闭)，所以未定义
//差分输入信号的单/双极性编码控制: bit12
#define AD7799_CONFIG_BIPOLAR     0x0000    //双极性编码(默认)
#define AD7799_CONFIG_UNIPOLAR    0x1000    //v单极性编码
//增益选择位: bit10,9,8
#define AD7799_CONFIG_GAIN_1      0x0000    //增益=1(仪表放大器不用)
#define AD7799_CONFIG_GAIN_2      0x0100    //增益=2(仪表放大器不用)
#define AD7799_CONFIG_GAIN_4      0x0200    //增益=4
#define AD7799_CONFIG_GAIN_8      0x0300    //增益=8
#define AD7799_CONFIG_GAIN_16     0x0400    //增益=16
#define AD7799_CONFIG_GAIN_32     0x0500    //增益=32
#define AD7799_CONFIG_GAIN_64     0x0600    //增益=64
#define AD7799_CONFIG_GAIN_128    0x0700    //增益=128
#define AD7799_CONFIG_GAIN_AUTO   0X5A5A    //自动增益 - AD7799硬件无该功能，该功能由软件自动实现
//基准电压检测位: bit5
#define AD7799_CONFIG_REFDET_EN   0x0020    //基准电压检测功能有效：当外部基准电压开路或小于0.5V时,状态寄存器的NOXREF位置位
#define AD7799_CONFIG_REFDET_DIS  0x0000    //基准电压检测功能禁用
//BUF位: bit4
#define AD7799_CONFIG_BUF_EN      0x0010    //v缓冲工作模式
#define AD7799_CONFIG_BUF_DIS     0x0000    //无缓冲工作模式
//通道选择位: bit2,1,0
#define AD7799_CONFIG_AIN1        0x0000    //AIN1差分输入
#define AD7799_CONFIG_AIN2        0x0001    //AIN2差分输入
#define AD7799_CONFIG_AIN3        0x0002    //AIN3差分输入



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
