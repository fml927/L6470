/**
***********************************************************************************
* @file		~/USER/TM7711.c 
* @author	TianYu
* @version	V0.9
* @date		29-Aug-2011
* @brief	This file contains all of the initialization function for this project
***********************************************************************************
**/

//Defined necessary head file and CONST flag
#include"stm32f10x_conf.h"
#include"stm32f10x.h"
#include "AD7799.h"
#include "main.h"


ADC_VAL_Def  AD_Value[AD7799_CHANN_USE];      //各通道(含所有芯片)的ADC值,经数字滤波后的ADC值
ADC_RUN_Def  AD_Work_Info[AD7799_CHANN_USE];  //各通道(含所有芯片)的工作信息

u8       ADC_Chn[AD7799_PCS] = {0};           //各芯片 ADC转换通道号:0~2
#define  SPI_PULSE_WIDE	    20                //SPI模拟通信中，时钟信号的延时

/*******************************************************************************
* Function Name  : auto_gain
* Description    : 根据采样的AD值,自动调整AD7799的增益
* Input          : chn - ADC的采样通道(AD7799_CHANN_USE以内)
* Output         : 全局变量AD_Work_Info[chn].SetGain，其值为0~7,对应增益为1~128
* Return         : None
* 算法说明       : 在增益未达最大和最小时，自动控制增益后的AD值在%40~80%之间
*******************************************************************************/
void auto_gain(u8 chn)
{
  u32 xd,xcv;
  
  chn = chn & 0x03;
  
  if(AD_Work_Info[chn].CurrValue < 0) 
  {
    xcv = -AD_Work_Info[chn].CurrValue;
  }
  else
  {
    xcv = AD_Work_Info[chn].CurrValue;
  }
  
  if(xcv > (REF_FULL_VAL * 81 / 100))  //大于81%,则在增益系数值不为0时,增益系数-1(即增益降低2倍)
  {
    if(AD_Work_Info[chn].CurrGain) 
    { AD_Work_Info[chn].SetGain = AD_Work_Info[chn].CurrGain - 1;}
  }
  else if(xcv < (REF_FULL_VAL * 39/ 100))  //小于39%时，直接把增益增大N倍，使其达到50%以内。
  {
    xd = REF_FULL_VAL / xcv;
    xd = xd  / 2;
    if(xd) 
    {
      xd--;
    }
    AD_Work_Info[chn].SetGain = AD_Work_Info[chn].CurrGain + xd;
    if(AD_Work_Info[chn].SetGain >= 7)   //限制最大增益系数为7,即128倍
    {
      AD_Work_Info[chn].SetGain = 7;
    }
  }
  
}
/*******************************************************************************
* Function Name  : ADC7799_Init
* Description    : AD7799初始化: GPIO,配置启动,变量清0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC7799_Init(void)  
{
  u8 i;
  for(i = 0; i < AD7799_CHANN_USE; i++)
  {
    AD_Value[i].Status = 0;
    AD_Value[i].Value = 0;
    
    AD_Work_Info[i].RefMode = 0;
    AD_Work_Info[i].SetGain = 0;
    AD_Work_Info[i].CurrGain = 0;
    AD_Work_Info[i].CurrValue = 0;
  }
  
  for(i = 0; i < AD7799_PCS; i++)
  {
    AD7799_CS_Pin_Configuration(i);
    AD7799_SCLK_Pin_Configuration(i);
    AD7799_DIN_Pin_Configuration(i);
    AD7799_DOUT_Pin_Configuration(i);
    ADC_Chn[i] = 0;
    Config_AD7799(i,ADC_Chn[i]);   
  }
}

/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : 配置MCU与各AD7799的CS连接的引脚 (OUT)
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_CS_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_CS_PORT,ADC2_CS_PORT,ADC3_CS_PORT,ADC4_CS_PORT};   //端口
  uint16_t      AD7799_PINx[4] = {ADC1_CS_PIN, ADC2_CS_PIN, ADC3_CS_PIN, ADC4_CS_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //端口和芯片有效，才对端口进行配置
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  推挽输出
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : 配置MCU与各AD7799的SCLK连接的引脚 (OUT)
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_SCLK_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_SCLK_PORT,ADC2_SCLK_PORT,ADC3_SCLK_PORT,ADC4_SCLK_PORT};   //端口
  uint16_t      AD7799_PINx[4] = {ADC1_SCLK_PIN, ADC2_SCLK_PIN, ADC3_SCLK_PIN, ADC4_SCLK_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //端口和芯片有效，才对端口进行配置
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  推挽输出
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : 配置MCU与各AD7799的DIN连接的引脚 (OUT)
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_DIN_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_DIN_PORT,ADC2_DIN_PORT,ADC3_DIN_PORT,ADC4_DIN_PORT};   //端口
  uint16_t      AD7799_PINx[4] = {ADC1_DIN_PIN, ADC2_DIN_PIN, ADC3_DIN_PIN, ADC4_DIN_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //端口和芯片有效，才对端口进行配置
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  推挽输出
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : 配置MCU与各AD7799的DOUT连接的引脚 (IN)
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_DOUT_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_DOUT_PORT,ADC2_DOUT_PORT,ADC3_DOUT_PORT,ADC4_DOUT_PORT};   //端口
  uint16_t      AD7799_PINx[4] = {ADC1_DOUT_PIN, ADC2_DOUT_PIN, ADC3_DOUT_PIN, ADC4_DOUT_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //端口和芯片有效，才对端口进行配置
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //  上接输入
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}

/*******************************************************************************
* Function Name  : Delay_7799
* Description    : 延时函数
* Input          : timecount - 延时参数(0~65535)
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_7799(u16 timecount)
{
  while(timecount>0)
  {
    timecount--;
  }
}

/*******************************************************************************
* Function Name  : Wr1Byte7799
* Description    : 向AD7799写入1字节(MSB在前,即左移)上沿锁存
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
*                  data - 写入的8bit数据
* Output         : None
* Return         : None
*******************************************************************************/
void Wr1Byte7799(u8 chipn,u8 data)  //模拟SPI
{ 
  GPIO_TypeDef *AD7799_DIN_PORT[4] =  {ADC1_DIN_PORT,ADC2_DIN_PORT,ADC3_DIN_PORT,ADC4_DIN_PORT};   //端口
  uint16_t      AD7799_DIN_PINx[4] =  {ADC1_DIN_PIN, ADC2_DIN_PIN, ADC3_DIN_PIN, ADC4_DIN_PIN };    //Pin
  GPIO_TypeDef *AD7799_SCLK_PORT[4] = {ADC1_SCLK_PORT,ADC2_SCLK_PORT,ADC3_SCLK_PORT,ADC4_SCLK_PORT};   //端口
  uint16_t      AD7799_SCLK_PINx[4] = {ADC1_SCLK_PIN, ADC2_SCLK_PIN, ADC3_SCLK_PIN, ADC4_SCLK_PIN };    //Pin
  
  u8 xi;
  for(xi = 0; xi < 8; xi++)
  { 
    GPIO_ResetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //AD7799_CLK_L;
    if((data & 0x80) == 0x80 ) 
    {
      GPIO_SetBits(AD7799_DIN_PORT[chipn],AD7799_DIN_PINx[chipn]);  //AD7799_DIN_H;  
    }  
    else 
    {
      GPIO_ResetBits(AD7799_DIN_PORT[chipn],AD7799_DIN_PINx[chipn]); //AD7799_DIN_L;  
    }  
    Delay_7799(SPI_PULSE_WIDE);
    data = data << 1;  //左移1位
    GPIO_SetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //AD7799_CLK_H;   
    Delay_7799(SPI_PULSE_WIDE);
  }
}

/*******************************************************************************
* Function Name  : Rd1Byte7799
* Description    : 读1字节, AD7799在下沿输出数据,上升沿时数据有效,MSB在前
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : 8位数据
*******************************************************************************/
u8 Rd1Byte7799(u8 chipn)  //模拟SPI
{          
  GPIO_TypeDef *AD7799_DOUT_PORT[4] = {ADC1_DOUT_PORT,ADC2_DOUT_PORT,ADC3_DOUT_PORT,ADC4_DOUT_PORT};   //端口
  uint16_t      AD7799_DOUT_PINx[4] = {ADC1_DOUT_PIN, ADC2_DOUT_PIN, ADC3_DOUT_PIN, ADC4_DOUT_PIN };    //Pin
  GPIO_TypeDef *AD7799_SCLK_PORT[4] = {ADC1_SCLK_PORT,ADC2_SCLK_PORT,ADC3_SCLK_PORT,ADC4_SCLK_PORT};   //端口
  uint16_t      AD7799_SCLK_PINx[4] = {ADC1_SCLK_PIN, ADC2_SCLK_PIN, ADC3_SCLK_PIN, ADC4_SCLK_PIN };    //Pin
  u8 xi,xd = 0;
  
  GPIO_SetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //	AD7799_CLK_H; 
  for(xi = 0,xd = 0; xi < 8; xi++)
  {
    Delay_7799(SPI_PULSE_WIDE);
    GPIO_ResetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //AD7799_CLK_L;   
    Delay_7799(SPI_PULSE_WIDE);
    xd = xd * 2;   //左移一位
    // if(AD7799_DOUT) xd++;
    if(GPIO_ReadInputDataBit(AD7799_DOUT_PORT[chipn],AD7799_DOUT_PINx[chipn]))
    {
      xd++;
    }
    GPIO_SetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //AD7799_CLK_H; 
  }
  return xd;
}

/*******************************************************************************
* Function Name  : Set_CS_7799
* Description    : 设置AD7799的CS为高电平(1)或低电平(0)
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
*                    val - CS的电平值: 0=低电平; 1=高电平
* Output         : None
* Return         : None
*******************************************************************************/
void Set_CS_7799(u8 chipn,u8 val)  
{
  GPIO_TypeDef *AD7799_CS_PORT[4] = {ADC1_CS_PORT,ADC2_CS_PORT,ADC3_CS_PORT,ADC4_CS_PORT};   //端口
  uint16_t      AD7799_CS_PINx[4] = {ADC1_CS_PIN, ADC2_CS_PIN, ADC3_CS_PIN, ADC4_CS_PIN };    //Pin
  
  if(val == 0)
  {
    GPIO_ResetBits(AD7799_CS_PORT[chipn],AD7799_CS_PINx[chipn]); //AD7799_CS_L; 
  }
  else
  {
    GPIO_SetBits(AD7799_CS_PORT[chipn],AD7799_CS_PINx[chipn]);   //AD7799_CS_H;  
  }
}

/*******************************************************************************
* Function Name  : read_status_reg7799
* Description    : 读AD7799的状态寄存器
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : 8位状态值
*******************************************************************************/
u8 read_status_reg7799(u8 chipn)  //模拟SPI
{
  u8 xa;
  
  Set_CS_7799(chipn,0);      //AD7799_CS_L; 
  Wr1Byte7799(chipn,0x40);
  xa = Rd1Byte7799(chipn);
  Set_CS_7799(chipn,1);      //AD7799_CS_H;  
  return(xa);
}

/*******************************************************************************
* Function Name  : write_reg7799
* Description    : 向AD7799的任意寄存器写入指定长度的数据
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
*                  xcomm - 命令
*                   xlen - 要写入的字节长度
*	             *s  -  要写入的数据
* Output         : None
* Return         : None
*******************************************************************************/
void write_reg7799(u8 chipn,u8 xcomm, u8 xlen, u8 *s)  //模拟SPI
{
  u8 xi;
  
  Set_CS_7799(chipn,0);    //AD7799_CS_L;  
  Wr1Byte7799(chipn,xcomm & 0xbf);	//bit6置为0 (0=写)
  for(xi = 0; xi < xlen; xi++)
  {
    Wr1Byte7799(chipn,s[xi]);
  }
  Set_CS_7799(chipn,1);      //AD7799_CS_H;  
}

/*******************************************************************************
* Function Name  : read_data_reg7799
* Description    : 读AD7799的24位数据寄存器
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : 24位AD值
*******************************************************************************/
u32 read_data_reg7799(u8 chipn)  //模拟SPI
{
  u32 xa;
  
  xa = 0;	
  Set_CS_7799(chipn,0);       //AD7799_CS_L;   
  Wr1Byte7799(chipn,0x58);
  xa = Rd1Byte7799(chipn);
  xa = (xa << 8) & 0xffffff00;   //最低8bit清为0,准备下一字节的填入
  xa = xa + Rd1Byte7799(chipn);
  xa = (xa << 8) & 0xffffff00;   //最低8bit清为0,准备下一字节的填入
  xa = xa + Rd1Byte7799(chipn);
  
  Set_CS_7799(chipn,1);      //AD7799_CS_H;   
  return(xa);
}

/*******************************************************************************
* Function Name  : Reset_AD7799
* Description    : AD7799复位: 发送32个1,即可对AD7799复位
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
* Output         : None
* Return         : None
*******************************************************************************/
void Reset_AD7799(u8 chipn)  
{
  u8 xi;
  
  Set_CS_7799(chipn,0);    //AD7799_CS_L;  
  for(xi = 0; xi < 4; xi++)      //发送32个
  {
    Wr1Byte7799(chipn,0xff);
  }
  Set_CS_7799(chipn,1);    //AD7799_CS_H;  
}

/*******************************************************************************
* Function Name  : Config_AD7799
* Description    : 配置AD7799
* Input          : chipn - AD7799芯片编号：0～(AD7799_PCS-1)，最大3
*                  chn   - 各芯片各自的通道编号(0~2)
* Output         : None
* Return         : None
*******************************************************************************/
void Config_AD7799(u8 chipn,u8 chn)   //xchann-通道号
{
  u8 xwork[2];
  u16 mode,config;
  u8 i,chnbase;
  u8  Ad7799ChnS[4] = {AD7799_CHANN_USE1,AD7799_CHANN_USE2,AD7799_CHANN_USE3,AD7799_CHANN_USE4};  //各芯片对应的通道数
  
  chnbase = 0;    
  for(i = 0; i < chipn; i++)
  {  
    if(chipn > 0)
    { 
      chnbase += Ad7799ChnS[chipn-1];
    }
  }
  mode = 0; config = 0;
  //MODE寄存器配置值
  //              连续转换模式          转换速率
  mode = mode | AD7799_MODE_CONTINUE | AD7799_RATE;
  
  chn = chn & 0x07;
  //config寄存器配置值
  //               双/单极性编码            //缓冲工作模式     //通道号
#ifdef  AD7799_BIPOLAR
  config = config | AD7799_CONFIG_BIPOLAR | AD7799_CONFIG_BUF_EN |chn;
#else
  config = config | AD7799_CONFIG_UNIPOLAR | AD7799_CONFIG_BUF_EN |chn;
#endif        
  //增益-用户在AD7799.H文件中定义
  if(AD7799_GAIN== AD7799_CONFIG_GAIN_AUTO)  //自动增益
  {
    auto_gain(chnbase+chn);    
    config = config | (AD_Work_Info[chnbase+chn].SetGain << 8);   
    AD_Work_Info[chnbase+chn].CurrGain = AD_Work_Info[chnbase+chn].SetGain;
  }
  else   //固定增益
  {  
    config = config | AD7799_GAIN;   //增益128
    AD_Work_Info[chnbase+chn].SetGain = (AD7799_GAIN >> 8) & 0x07;
    AD_Work_Info[chnbase+chn].CurrGain = AD_Work_Info[chnbase+chn].SetGain;
  }
  xwork[0] = mode >> 8;   //H8
  xwork[1] = mode;        //L8 
  write_reg7799(chipn,0x08,2,xwork);	//写MODE寄存器: 
  
  //写config寄存器
  xwork[0] = config >> 8;   //H8
  xwork[1] = config;        //L8 
  write_reg7799(chipn,0x10,2,xwork);	//写config寄存器
  
  //io寄存器
  xwork[0] = 0x00;		//<6:4>:可用,在AIN3引脚输出数字信号。此处为0,禁用数字输出功能
  write_reg7799(chipn,0x28,1,xwork);	//写IO寄存器(电流源控制寄存器)
  
}

/*******************************************************************************
* Function Name  : ADC_Auto_Conversion
* Description    : 轮询各片AD7799,把最新的AD值经滤波处理后装入相应的AD_Value[]通道中
* Input          : None
* Output         : AD_Value[](全局) - 经滤波处理后的AD值
* Return         : None
*******************************************************************************/
void ADC_Auto_Conversion(void)
{  //滑动平均滤波 - 与总的通道数匹配
  static u8  MovingAv_Fill[AD7799_CHANN_USE] = {0};   //1=滑动平均滤波时，采集的数据已达到满队列的(最多3个队列)的标志
  static float MovingAv_Buf[AD7799_CHANN_USE][MOVING_AVERAGE_BUFSIZE];   //滑动平均滤波 - 各通道的数据缓存
  static u8  MovingAv_Cnt[AD7799_CHANN_USE] = {0};    
  //中位值滤波 - 与芯片数匹配
  static u8  MedianFilt_Cnt[AD7799_PCS] = {0};    //中位值滤波 - 数据计数器
  static s32 MedianFilt_Buf[AD7799_PCS][MEDIAN_FILTERING_BUFSIZE];   //中位值滤波 - 数据缓存
  static u16  ADC_Read_Cnt = 0;                   //轮询AD7799转换结果的时间间隔计数器 
  static u32 adcerr_cnt[AD7799_PCS] = {0};        //AD7799异常计数器

  u8  Ad7799ChnS[4] = {AD7799_CHANN_USE1,AD7799_CHANN_USE2,AD7799_CHANN_USE3,AD7799_CHANN_USE4};  //各芯片对应的通道数
  s32 xd,tmp;
  float xf;
  u8  i,chipn;
  u8  chnbase = 0;   //将各芯片的各个通道转为全局通道时，其基址编号
 
  ADC_Read_Cnt++;
  if(ADC_Read_Cnt < ADC7799_RD_CYCLE)  //延时
  {
    return;
  }
  for(chipn = 0; chipn < AD7799_PCS; chipn++)
  {  
    if(chipn == 0)
    {
      chnbase = 0;
    }
    else
    {
      chnbase += Ad7799ChnS[chipn-1];
    }
    
    xd = read_status_reg7799(chipn);
    ADC_Read_Cnt = 0;
    adcerr_cnt[chipn]++;      //ADC异常超时计数器，为ADC_Read_Cnt的1000倍
    
    //在ADC转换完成后,读取ADC的值.(同时检测IO口的状态 
    if((xd & 0x80) == 0)  //最高位(bit7)为0,表示ADC转换完成，则读取转换结果并且进行滤波处理
     {    
      adcerr_cnt[chipn] = 0;
      ADC_Read_Cnt = 0;
      xd = xd & 0x03;  //AD7799当前的转换通道
      if(xd >= AD7799_CHANN_USE) //通道不正确
      {
        ADC_Chn[chipn] = 0;
        Config_AD7799(chipn,ADC_Chn[chipn]);    
        return;
      }
      xd = read_data_reg7799(chipn);   //读取转换结果
#ifdef  AD7799_BIPOLAR
      //双极性: 负差分输入的AD值:0~0x7fffff;0v电压的AD值:0x800000;正差分输入的AD值:0x800001~0xffffff;
      xd = xd - 0x800000;
#endif
      //中位值滤波 - 排序:把最新采集的AD值按从小到大插入到排序缓存中
      if(MedianFilt_Cnt[chipn] > 0)  //中位值滤波缓存中有数据，则排序
      {
        for(i = 0; i < MedianFilt_Cnt[chipn]; i++) //排序
        {
          if(MedianFilt_Buf[chipn][i] > xd)
          {
            tmp = MedianFilt_Buf[chipn][i];
            MedianFilt_Buf[chipn][i] = xd;
            xd = tmp;
          }
        }
      }
      MedianFilt_Buf[chipn][MedianFilt_Cnt[chipn]] = xd;
      MedianFilt_Cnt[chipn]++;
      
      if(MedianFilt_Cnt[chipn] >= MEDIAN_FILTERING_BUFSIZE) //连续采集完一组中位值滤波所的数据, 取中值,且切换到下一个通道
      {
        xd = MedianFilt_Buf[chipn][MEDIAN_FILTERING_BUFSIZE/2];  //取中位值，如果总个数为偶数，则取中间偏后面的数
        MedianFilt_Cnt[chipn] = 0;
        AD_Work_Info[chnbase+ADC_Chn[chipn]].CurrValue = xd;  //AD当前条件(GAIN等)下的采集值，是计算自动增益的依据
        
        //自动增益与固定增益处理: 
        xf = (float)xd / (float)(1 << AD_Work_Info[chnbase+ADC_Chn[chipn]].SetGain);   //还原为无增益时的AD值-小数
        
        //滑动平均滤波
        MovingAv_Buf[chnbase+ADC_Chn[chipn]][MovingAv_Cnt[chnbase+ADC_Chn[chipn]]] = xf; 
        MovingAv_Cnt[chnbase+ADC_Chn[chipn]]++;
        
        if(MovingAv_Cnt[chnbase+ADC_Chn[chipn]]>= MOVING_AVERAGE_BUFSIZE )
        {
          MovingAv_Cnt[chnbase+ADC_Chn[chipn]] = 0;    //新数据循环放入缓存,且覆盖最旧的数据
          MovingAv_Fill[chnbase+ADC_Chn[chipn]] = 1;   //缓存已填满标志
        }
        //判断用来计算平均值的缓存数据个数:
        if(MovingAv_Fill[chnbase+ADC_Chn[chipn]]) //数据已填满"滑动平均滤波"器的缓存，即按最大数据计算平均值
        {
          tmp = MOVING_AVERAGE_BUFSIZE;
        }
        else //数据还未填满"滑动平均滤波"器的缓存，则按实际个数(小于缓存的最大个数)计算平均值
        {
          tmp = MovingAv_Cnt[chnbase+ADC_Chn[chipn]];
        }
        //求平均值
        for(xf = 0.0, i = 0; i < tmp; i++)
        { 
          xf += MovingAv_Buf[chnbase+ADC_Chn[chipn]][i]; 
        }
        xf = xf / (float)tmp;
        AD_Value[chnbase+ADC_Chn[chipn]].Value = xf;   
        AD_Value[chnbase+ADC_Chn[chipn]].Status = 1;      //数据有效
        //切换到下一通道
        ADC_Chn[chipn]++;
        if(ADC_Chn[chipn] >= Ad7799ChnS[chipn])
        {
          ADC_Chn[chipn] = 0;
        }
        if(Ad7799ChnS[chipn] > 1)  //当只有一个通道时，不对AD7799重新初始化
        {
          Config_AD7799(chipn,ADC_Chn[chipn]);
        }
      } 
    }
    else
    {
      if(adcerr_cnt[chipn] > ADC7799_ERR_CYCLE) //约3~10S, ADC超时，未产生新的转换结果，则重新初始化
      {   
        Reset_AD7799(chipn); 
        Config_AD7799(chipn,ADC_Chn[chipn]);    
        adcerr_cnt[chipn] = 0;
      }
    }
  }
  
}


