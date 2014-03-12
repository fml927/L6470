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


ADC_VAL_Def  AD_Value[AD7799_CHANN_USE];      //��ͨ��(������оƬ)��ADCֵ,�������˲����ADCֵ
ADC_RUN_Def  AD_Work_Info[AD7799_CHANN_USE];  //��ͨ��(������оƬ)�Ĺ�����Ϣ

u8       ADC_Chn[AD7799_PCS] = {0};           //��оƬ ADCת��ͨ����:0~2
#define  SPI_PULSE_WIDE	    20                //SPIģ��ͨ���У�ʱ���źŵ���ʱ

/*******************************************************************************
* Function Name  : auto_gain
* Description    : ���ݲ�����ADֵ,�Զ�����AD7799������
* Input          : chn - ADC�Ĳ���ͨ��(AD7799_CHANN_USE����)
* Output         : ȫ�ֱ���AD_Work_Info[chn].SetGain����ֵΪ0~7,��Ӧ����Ϊ1~128
* Return         : None
* �㷨˵��       : ������δ��������Сʱ���Զ�����������ADֵ��%40~80%֮��
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
  
  if(xcv > (REF_FULL_VAL * 81 / 100))  //����81%,��������ϵ��ֵ��Ϊ0ʱ,����ϵ��-1(�����潵��2��)
  {
    if(AD_Work_Info[chn].CurrGain) 
    { AD_Work_Info[chn].SetGain = AD_Work_Info[chn].CurrGain - 1;}
  }
  else if(xcv < (REF_FULL_VAL * 39/ 100))  //С��39%ʱ��ֱ�Ӱ���������N����ʹ��ﵽ50%���ڡ�
  {
    xd = REF_FULL_VAL / xcv;
    xd = xd  / 2;
    if(xd) 
    {
      xd--;
    }
    AD_Work_Info[chn].SetGain = AD_Work_Info[chn].CurrGain + xd;
    if(AD_Work_Info[chn].SetGain >= 7)   //�����������ϵ��Ϊ7,��128��
    {
      AD_Work_Info[chn].SetGain = 7;
    }
  }
  
}
/*******************************************************************************
* Function Name  : ADC7799_Init
* Description    : AD7799��ʼ��: GPIO,��������,������0
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
* Description    : ����MCU���AD7799��CS���ӵ����� (OUT)
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_CS_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_CS_PORT,ADC2_CS_PORT,ADC3_CS_PORT,ADC4_CS_PORT};   //�˿�
  uint16_t      AD7799_PINx[4] = {ADC1_CS_PIN, ADC2_CS_PIN, ADC3_CS_PIN, ADC4_CS_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //�˿ں�оƬ��Ч���ŶԶ˿ڽ�������
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  �������
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : ����MCU���AD7799��SCLK���ӵ����� (OUT)
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_SCLK_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_SCLK_PORT,ADC2_SCLK_PORT,ADC3_SCLK_PORT,ADC4_SCLK_PORT};   //�˿�
  uint16_t      AD7799_PINx[4] = {ADC1_SCLK_PIN, ADC2_SCLK_PIN, ADC3_SCLK_PIN, ADC4_SCLK_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //�˿ں�оƬ��Ч���ŶԶ˿ڽ�������
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  �������
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : ����MCU���AD7799��DIN���ӵ����� (OUT)
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_DIN_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_DIN_PORT,ADC2_DIN_PORT,ADC3_DIN_PORT,ADC4_DIN_PORT};   //�˿�
  uint16_t      AD7799_PINx[4] = {ADC1_DIN_PIN, ADC2_DIN_PIN, ADC3_DIN_PIN, ADC4_DIN_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //�˿ں�оƬ��Ч���ŶԶ˿ڽ�������
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  �������
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : AD7799_SCLK_Pin_Configuration
* Description    : ����MCU���AD7799��DOUT���ӵ����� (IN)
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : None
*******************************************************************************/
void AD7799_DOUT_Pin_Configuration(u8 chipn)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *AD7799_PORT[4] = {ADC1_DOUT_PORT,ADC2_DOUT_PORT,ADC3_DOUT_PORT,ADC4_DOUT_PORT};   //�˿�
  uint16_t      AD7799_PINx[4] = {ADC1_DOUT_PIN, ADC2_DOUT_PIN, ADC3_DOUT_PIN, ADC4_DOUT_PIN };    //Pin
  
  if(AD7799_PORT[chipn] && (chipn < AD7799_PCS))   //�˿ں�оƬ��Ч���ŶԶ˿ڽ�������
  {  
    GPIO_InitStructure.GPIO_Pin = AD7799_PINx[chipn];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //  �Ͻ�����
    GPIO_Init(AD7799_PORT[chipn], &GPIO_InitStructure);  
  }
}

/*******************************************************************************
* Function Name  : Delay_7799
* Description    : ��ʱ����
* Input          : timecount - ��ʱ����(0~65535)
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
* Description    : ��AD7799д��1�ֽ�(MSB��ǰ,������)��������
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
*                  data - д���8bit����
* Output         : None
* Return         : None
*******************************************************************************/
void Wr1Byte7799(u8 chipn,u8 data)  //ģ��SPI
{ 
  GPIO_TypeDef *AD7799_DIN_PORT[4] =  {ADC1_DIN_PORT,ADC2_DIN_PORT,ADC3_DIN_PORT,ADC4_DIN_PORT};   //�˿�
  uint16_t      AD7799_DIN_PINx[4] =  {ADC1_DIN_PIN, ADC2_DIN_PIN, ADC3_DIN_PIN, ADC4_DIN_PIN };    //Pin
  GPIO_TypeDef *AD7799_SCLK_PORT[4] = {ADC1_SCLK_PORT,ADC2_SCLK_PORT,ADC3_SCLK_PORT,ADC4_SCLK_PORT};   //�˿�
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
    data = data << 1;  //����1λ
    GPIO_SetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //AD7799_CLK_H;   
    Delay_7799(SPI_PULSE_WIDE);
  }
}

/*******************************************************************************
* Function Name  : Rd1Byte7799
* Description    : ��1�ֽ�, AD7799�������������,������ʱ������Ч,MSB��ǰ
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : 8λ����
*******************************************************************************/
u8 Rd1Byte7799(u8 chipn)  //ģ��SPI
{          
  GPIO_TypeDef *AD7799_DOUT_PORT[4] = {ADC1_DOUT_PORT,ADC2_DOUT_PORT,ADC3_DOUT_PORT,ADC4_DOUT_PORT};   //�˿�
  uint16_t      AD7799_DOUT_PINx[4] = {ADC1_DOUT_PIN, ADC2_DOUT_PIN, ADC3_DOUT_PIN, ADC4_DOUT_PIN };    //Pin
  GPIO_TypeDef *AD7799_SCLK_PORT[4] = {ADC1_SCLK_PORT,ADC2_SCLK_PORT,ADC3_SCLK_PORT,ADC4_SCLK_PORT};   //�˿�
  uint16_t      AD7799_SCLK_PINx[4] = {ADC1_SCLK_PIN, ADC2_SCLK_PIN, ADC3_SCLK_PIN, ADC4_SCLK_PIN };    //Pin
  u8 xi,xd = 0;
  
  GPIO_SetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //	AD7799_CLK_H; 
  for(xi = 0,xd = 0; xi < 8; xi++)
  {
    Delay_7799(SPI_PULSE_WIDE);
    GPIO_ResetBits(AD7799_SCLK_PORT[chipn],AD7799_SCLK_PINx[chipn]);    //AD7799_CLK_L;   
    Delay_7799(SPI_PULSE_WIDE);
    xd = xd * 2;   //����һλ
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
* Description    : ����AD7799��CSΪ�ߵ�ƽ(1)��͵�ƽ(0)
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
*                    val - CS�ĵ�ƽֵ: 0=�͵�ƽ; 1=�ߵ�ƽ
* Output         : None
* Return         : None
*******************************************************************************/
void Set_CS_7799(u8 chipn,u8 val)  
{
  GPIO_TypeDef *AD7799_CS_PORT[4] = {ADC1_CS_PORT,ADC2_CS_PORT,ADC3_CS_PORT,ADC4_CS_PORT};   //�˿�
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
* Description    : ��AD7799��״̬�Ĵ���
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : 8λ״ֵ̬
*******************************************************************************/
u8 read_status_reg7799(u8 chipn)  //ģ��SPI
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
* Description    : ��AD7799������Ĵ���д��ָ�����ȵ�����
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
*                  xcomm - ����
*                   xlen - Ҫд����ֽڳ���
*	             *s  -  Ҫд�������
* Output         : None
* Return         : None
*******************************************************************************/
void write_reg7799(u8 chipn,u8 xcomm, u8 xlen, u8 *s)  //ģ��SPI
{
  u8 xi;
  
  Set_CS_7799(chipn,0);    //AD7799_CS_L;  
  Wr1Byte7799(chipn,xcomm & 0xbf);	//bit6��Ϊ0 (0=д)
  for(xi = 0; xi < xlen; xi++)
  {
    Wr1Byte7799(chipn,s[xi]);
  }
  Set_CS_7799(chipn,1);      //AD7799_CS_H;  
}

/*******************************************************************************
* Function Name  : read_data_reg7799
* Description    : ��AD7799��24λ���ݼĴ���
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : 24λADֵ
*******************************************************************************/
u32 read_data_reg7799(u8 chipn)  //ģ��SPI
{
  u32 xa;
  
  xa = 0;	
  Set_CS_7799(chipn,0);       //AD7799_CS_L;   
  Wr1Byte7799(chipn,0x58);
  xa = Rd1Byte7799(chipn);
  xa = (xa << 8) & 0xffffff00;   //���8bit��Ϊ0,׼����һ�ֽڵ�����
  xa = xa + Rd1Byte7799(chipn);
  xa = (xa << 8) & 0xffffff00;   //���8bit��Ϊ0,׼����һ�ֽڵ�����
  xa = xa + Rd1Byte7799(chipn);
  
  Set_CS_7799(chipn,1);      //AD7799_CS_H;   
  return(xa);
}

/*******************************************************************************
* Function Name  : Reset_AD7799
* Description    : AD7799��λ: ����32��1,���ɶ�AD7799��λ
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
* Output         : None
* Return         : None
*******************************************************************************/
void Reset_AD7799(u8 chipn)  
{
  u8 xi;
  
  Set_CS_7799(chipn,0);    //AD7799_CS_L;  
  for(xi = 0; xi < 4; xi++)      //����32��
  {
    Wr1Byte7799(chipn,0xff);
  }
  Set_CS_7799(chipn,1);    //AD7799_CS_H;  
}

/*******************************************************************************
* Function Name  : Config_AD7799
* Description    : ����AD7799
* Input          : chipn - AD7799оƬ��ţ�0��(AD7799_PCS-1)�����3
*                  chn   - ��оƬ���Ե�ͨ�����(0~2)
* Output         : None
* Return         : None
*******************************************************************************/
void Config_AD7799(u8 chipn,u8 chn)   //xchann-ͨ����
{
  u8 xwork[2];
  u16 mode,config;
  u8 i,chnbase;
  u8  Ad7799ChnS[4] = {AD7799_CHANN_USE1,AD7799_CHANN_USE2,AD7799_CHANN_USE3,AD7799_CHANN_USE4};  //��оƬ��Ӧ��ͨ����
  
  chnbase = 0;    
  for(i = 0; i < chipn; i++)
  {  
    if(chipn > 0)
    { 
      chnbase += Ad7799ChnS[chipn-1];
    }
  }
  mode = 0; config = 0;
  //MODE�Ĵ�������ֵ
  //              ����ת��ģʽ          ת������
  mode = mode | AD7799_MODE_CONTINUE | AD7799_RATE;
  
  chn = chn & 0x07;
  //config�Ĵ�������ֵ
  //               ˫/�����Ա���            //���幤��ģʽ     //ͨ����
#ifdef  AD7799_BIPOLAR
  config = config | AD7799_CONFIG_BIPOLAR | AD7799_CONFIG_BUF_EN |chn;
#else
  config = config | AD7799_CONFIG_UNIPOLAR | AD7799_CONFIG_BUF_EN |chn;
#endif        
  //����-�û���AD7799.H�ļ��ж���
  if(AD7799_GAIN== AD7799_CONFIG_GAIN_AUTO)  //�Զ�����
  {
    auto_gain(chnbase+chn);    
    config = config | (AD_Work_Info[chnbase+chn].SetGain << 8);   
    AD_Work_Info[chnbase+chn].CurrGain = AD_Work_Info[chnbase+chn].SetGain;
  }
  else   //�̶�����
  {  
    config = config | AD7799_GAIN;   //����128
    AD_Work_Info[chnbase+chn].SetGain = (AD7799_GAIN >> 8) & 0x07;
    AD_Work_Info[chnbase+chn].CurrGain = AD_Work_Info[chnbase+chn].SetGain;
  }
  xwork[0] = mode >> 8;   //H8
  xwork[1] = mode;        //L8 
  write_reg7799(chipn,0x08,2,xwork);	//дMODE�Ĵ���: 
  
  //дconfig�Ĵ���
  xwork[0] = config >> 8;   //H8
  xwork[1] = config;        //L8 
  write_reg7799(chipn,0x10,2,xwork);	//дconfig�Ĵ���
  
  //io�Ĵ���
  xwork[0] = 0x00;		//<6:4>:����,��AIN3������������źš��˴�Ϊ0,���������������
  write_reg7799(chipn,0x28,1,xwork);	//дIO�Ĵ���(����Դ���ƼĴ���)
  
}

/*******************************************************************************
* Function Name  : ADC_Auto_Conversion
* Description    : ��ѯ��ƬAD7799,�����µ�ADֵ���˲������װ����Ӧ��AD_Value[]ͨ����
* Input          : None
* Output         : AD_Value[](ȫ��) - ���˲�������ADֵ
* Return         : None
*******************************************************************************/
void ADC_Auto_Conversion(void)
{  //����ƽ���˲� - ���ܵ�ͨ����ƥ��
  static u8  MovingAv_Fill[AD7799_CHANN_USE] = {0};   //1=����ƽ���˲�ʱ���ɼ��������Ѵﵽ�����е�(���3������)�ı�־
  static float MovingAv_Buf[AD7799_CHANN_USE][MOVING_AVERAGE_BUFSIZE];   //����ƽ���˲� - ��ͨ�������ݻ���
  static u8  MovingAv_Cnt[AD7799_CHANN_USE] = {0};    
  //��λֵ�˲� - ��оƬ��ƥ��
  static u8  MedianFilt_Cnt[AD7799_PCS] = {0};    //��λֵ�˲� - ���ݼ�����
  static s32 MedianFilt_Buf[AD7799_PCS][MEDIAN_FILTERING_BUFSIZE];   //��λֵ�˲� - ���ݻ���
  static u16  ADC_Read_Cnt = 0;                   //��ѯAD7799ת�������ʱ���������� 
  static u32 adcerr_cnt[AD7799_PCS] = {0};        //AD7799�쳣������

  u8  Ad7799ChnS[4] = {AD7799_CHANN_USE1,AD7799_CHANN_USE2,AD7799_CHANN_USE3,AD7799_CHANN_USE4};  //��оƬ��Ӧ��ͨ����
  s32 xd,tmp;
  float xf;
  u8  i,chipn;
  u8  chnbase = 0;   //����оƬ�ĸ���ͨ��תΪȫ��ͨ��ʱ�����ַ���
 
  ADC_Read_Cnt++;
  if(ADC_Read_Cnt < ADC7799_RD_CYCLE)  //��ʱ
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
    adcerr_cnt[chipn]++;      //ADC�쳣��ʱ��������ΪADC_Read_Cnt��1000��
    
    //��ADCת����ɺ�,��ȡADC��ֵ.(ͬʱ���IO�ڵ�״̬ 
    if((xd & 0x80) == 0)  //���λ(bit7)Ϊ0,��ʾADCת����ɣ����ȡת��������ҽ����˲�����
     {    
      adcerr_cnt[chipn] = 0;
      ADC_Read_Cnt = 0;
      xd = xd & 0x03;  //AD7799��ǰ��ת��ͨ��
      if(xd >= AD7799_CHANN_USE) //ͨ������ȷ
      {
        ADC_Chn[chipn] = 0;
        Config_AD7799(chipn,ADC_Chn[chipn]);    
        return;
      }
      xd = read_data_reg7799(chipn);   //��ȡת�����
#ifdef  AD7799_BIPOLAR
      //˫����: ����������ADֵ:0~0x7fffff;0v��ѹ��ADֵ:0x800000;����������ADֵ:0x800001~0xffffff;
      xd = xd - 0x800000;
#endif
      //��λֵ�˲� - ����:�����²ɼ���ADֵ����С������뵽���򻺴���
      if(MedianFilt_Cnt[chipn] > 0)  //��λֵ�˲������������ݣ�������
      {
        for(i = 0; i < MedianFilt_Cnt[chipn]; i++) //����
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
      
      if(MedianFilt_Cnt[chipn] >= MEDIAN_FILTERING_BUFSIZE) //�����ɼ���һ����λֵ�˲���������, ȡ��ֵ,���л�����һ��ͨ��
      {
        xd = MedianFilt_Buf[chipn][MEDIAN_FILTERING_BUFSIZE/2];  //ȡ��λֵ������ܸ���Ϊż������ȡ�м�ƫ�������
        MedianFilt_Cnt[chipn] = 0;
        AD_Work_Info[chnbase+ADC_Chn[chipn]].CurrValue = xd;  //AD��ǰ����(GAIN��)�µĲɼ�ֵ���Ǽ����Զ����������
        
        //�Զ�������̶����洦��: 
        xf = (float)xd / (float)(1 << AD_Work_Info[chnbase+ADC_Chn[chipn]].SetGain);   //��ԭΪ������ʱ��ADֵ-С��
        
        //����ƽ���˲�
        MovingAv_Buf[chnbase+ADC_Chn[chipn]][MovingAv_Cnt[chnbase+ADC_Chn[chipn]]] = xf; 
        MovingAv_Cnt[chnbase+ADC_Chn[chipn]]++;
        
        if(MovingAv_Cnt[chnbase+ADC_Chn[chipn]]>= MOVING_AVERAGE_BUFSIZE )
        {
          MovingAv_Cnt[chnbase+ADC_Chn[chipn]] = 0;    //������ѭ�����뻺��,�Ҹ�����ɵ�����
          MovingAv_Fill[chnbase+ADC_Chn[chipn]] = 1;   //������������־
        }
        //�ж���������ƽ��ֵ�Ļ������ݸ���:
        if(MovingAv_Fill[chnbase+ADC_Chn[chipn]]) //����������"����ƽ���˲�"���Ļ��棬����������ݼ���ƽ��ֵ
        {
          tmp = MOVING_AVERAGE_BUFSIZE;
        }
        else //���ݻ�δ����"����ƽ���˲�"���Ļ��棬��ʵ�ʸ���(С�ڻ����������)����ƽ��ֵ
        {
          tmp = MovingAv_Cnt[chnbase+ADC_Chn[chipn]];
        }
        //��ƽ��ֵ
        for(xf = 0.0, i = 0; i < tmp; i++)
        { 
          xf += MovingAv_Buf[chnbase+ADC_Chn[chipn]][i]; 
        }
        xf = xf / (float)tmp;
        AD_Value[chnbase+ADC_Chn[chipn]].Value = xf;   
        AD_Value[chnbase+ADC_Chn[chipn]].Status = 1;      //������Ч
        //�л�����һͨ��
        ADC_Chn[chipn]++;
        if(ADC_Chn[chipn] >= Ad7799ChnS[chipn])
        {
          ADC_Chn[chipn] = 0;
        }
        if(Ad7799ChnS[chipn] > 1)  //��ֻ��һ��ͨ��ʱ������AD7799���³�ʼ��
        {
          Config_AD7799(chipn,ADC_Chn[chipn]);
        }
      } 
    }
    else
    {
      if(adcerr_cnt[chipn] > ADC7799_ERR_CYCLE) //Լ3~10S, ADC��ʱ��δ�����µ�ת������������³�ʼ��
      {   
        Reset_AD7799(chipn); 
        Config_AD7799(chipn,ADC_Chn[chipn]);    
        adcerr_cnt[chipn] = 0;
      }
    }
  }
  
}


