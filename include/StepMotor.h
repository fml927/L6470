#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H
typedef struct       
{
    u8  Enable;     //�����/ͣ������� DISABLE=�رյ����ENABLE=ʹ�ܵ������
    u8  Status;
    u8  SetDir;     //�趨ת������:1=��ת;0=��ת
    float SetSpeed;   //�趨ת��(rpm)
    float CurrSpeed;  //��ǰת��(rpm)
    u8  ToBottom;    //1=����;0=δ����
    u8  ToTop;       //1=����;0=δ����
    u8  Alert;       //���ȡ�������1=����,0=����
    u8  NF;          //ȱ�ࣺ1=����,0=����
    u8  Braking;     //ǿ���ƶ�(�øǵ�)���ŵ�״̬��1=�ƶ���Ч;0=���ƶ�,��������
    u8  RunSta_Brak; //�ݴ�ǿ���ƶ�(�øǵ�)ǰ�õĹ���״̬,��ȡ��ǿ���ƶ���,�Զ��ָ�����
    u8  LockRotor;   //��ת��1=��ת����,0=����
    u32 Flow;        //�����ۼ�(PWM����(�ж�)�ۼ�)
    u8  FlowErr;            //��:ͨ�������ۼӼ��������쳣(���ת):1=�����쳣
    u32 FlowLockRotor;   //��ת����õ�PWM�������
    u32 LockRotorSpeed;  //��ת����õ�ʵ��ת�ټ��������������̻�HALL�ź�
    float RealSpeed;       //��ʵת��(rpm) - ͨ��ת�ټ���ź�ʵ��õ���ת��
    u32   RealPulse;       //��ʵת�������ۼƣ����ڼ�����ʵת�٣�ÿ������0һ��
    u8  RatioSwitch;      //�ù�ϵ�����Կ���
    u8  Down_Mode;        //������תģʽ(00����,01��·��װ,10�ù�ϵ�����ԣ�11�Լ�ģʽ)
    u8  Up_Mode;
    float  Ratio;            //�ù�ϵ��ml/R
    float  SetFlow;          //�趨����(0~100ml/h*10)�Ŵ�10��
    u32 Step;
    u32 Temp_Circle;        //һ������Ȧ��
    u32 Circle;
    float ReadSpeed;
    u8 CMD;
}StepMotor_WorkInfo_Def;    //�������нṹ���װ
extern StepMotor_WorkInfo_Def StepMotor_Data[];   

typedef enum   
{
  UFP = 0,   //  - ΪMOTOR1    ���˱�
  BPP,       //  - ΪMOTOR2    ������
  SBP,       //    - ΪMOTOR3  ��Һ��
  SFP,       //   - ΪMOTOR4   ���ܱ�
  HP,        //   - ΪMOTOR5
}StepMotor_TypeDef;



#define MOTOR_SPEED_EN      //DCY������Ч
#define SPEED_VAL1  20     //56���С��25RPM  
#define SPEED_VAL2  50    //56���С��50RPM 
#define SPEED_VAL3  80    //56���С��75RPM 
#define SPEED_VAL4  100   //56���С��100RPM

#define MOTOR_Stop_Turn           //�Ƚ�ת�ٸ������������ת
#define HP_INSTALL_EN                   //���رð�װ
#define MOTOR_BRAKING_EN              //ǿ��ɲ��(�øǵȼ��)��Ч
//#define MOTOR_PWM_EN                   //IO����PWMʹ��
//#define MOTOR_PWM_IRQ_EN              //����PWM��TIM�ж���Ч����Ҫ�������������ۼ�
//#define MOTOR_PWM_LOCKROTOR_CHECK_EN  //��PWM�жϼ���ת��Ч(������MOTOR_REALSPEED_CHECK_EN��MOTOR_PWM_IRQ_EN������Ч)
#define MOTOR_REALSPEED_CHECK_EN      //�����ʵת�ټ����Ч(��ⷽʽΪ�����������)

#define STEPMOTOR_COEFFICIENT     168750.0  //���������TIMϵ��337500
#define STEPMOTOR_PWMPULSE        25600     //�������ÿת��PWM������ =(360��/1.8��)*128

#define MOTOR_TOTAL   5                  //ϵͳ��ʵ��ʹ�õĵ���ܸ��� [����һ��]
#define STEPMOTOR_PULSE   256            //


#define STEPMOTOR_ADJSPEED_SPACE    30  //���ټ��ʱ��(��λms)
#define STEPMOTOR_ADJSPEED_STEP     1  //2rmp - ÿ�ε�����/���Ĳ�����ת����

#define STEPMOTOR_IN_DEBOUNCE       5    //������������������������ȥ��������
#define STEPMOTOR_NF_CHECK_10S      1500  //ȱ��������10s: 0.01s*1000
#define STEPMOTOR_MAX_SETSPEED      1000.0 //ת�����ƣ�A,B�����250rpm

// CSƬѡ���ż��˿�����. δʹ�õıõ�ֵ�����һ������ͬ
#define dSPIN_UFP_CS_Pin		GPIO_Pin_7     //UFP Ƭѡ
#define dSPIN_UFP_CS_Port		GPIOE

#define dSPIN_BPP_CS_Pin		GPIO_Pin_12     //BPP Ƭѡ
#define dSPIN_BPP_CS_Port		GPIOB

#define dSPIN_SBP_CS_Pin		GPIO_Pin_15     //SBP Ƭѡ
#define dSPIN_SBP_CS_Port		GPIOD

#define dSPIN_SFP_CS_Pin		GPIO_Pin_9     //SFP Ƭѡ
#define dSPIN_SFP_CS_Port		GPIOA

#define dSPIN_HP_CS_Pin		        GPIO_Pin_11     //HP Ƭѡ
#define dSPIN_HP_CS_Port		GPIOC

// FLAG���ż��˿�����. δʹ�õıõ�ֵ�����һ������ͬ
#define dSPIN_UFP_FLAG_Pin	GPIO_Pin_8     //UFP
#define dSPIN_UFP_FLAG_Port	GPIOE

#define dSPIN_BPP_FLAG_Pin	GPIO_Pin_8     //BPP
#define dSPIN_BPP_FLAG_Port	GPIOD

#define dSPIN_SBP_FLAG_Pin	GPIO_Pin_6    //SBP 
#define dSPIN_SBP_FLAG_Port	GPIOC

#define dSPIN_SFP_FLAG_Pin	GPIO_Pin_10    //SFP 
#define dSPIN_SFP_FLAG_Port	GPIOA

#define dSPIN_HP_FLAG_Pin	GPIO_Pin_12    //HP 
#define dSPIN_HP_FLAG_Port	GPIOC


#define dSPIN_UFP_BUSY_Pin		GPIO_Pin_9
#define dSPIN_UFP_BUSY_Port		GPIOE

#define dSPIN_BPP_BUSY_Pin		GPIO_Pin_9
#define dSPIN_BPP_BUSY_Port		GPIOD

#define dSPIN_SBP_BUSY_Pin		GPIO_Pin_7
#define dSPIN_SBP_BUSY_Port		GPIOC

#define dSPIN_SFP_BUSY_Pin		GPIO_Pin_11
#define dSPIN_SFP_BUSY_Port		GPIOA

#define dSPIN_HP_BUSY_Pin		GPIO_Pin_0
#define dSPIN_HP_BUSY_Port		GPIOD


// PC�øǼ�����ż��˿�����. δʹ�õıõ�ֵ�����һ������ͬ
#ifdef  MOTOR_BRAKING_EN   //ǿ��ɲ��������Ч
#define MOTOR_BRAKING_INVERTER    1    //1=GPIO��Ϊ�ߵ�ƽǿ��ɲ��;0=GPIO��Ϊ�͵�ƽǿ��ɲ��
#define dSPIN_UFP_PC_Pin		GPIO_Pin_12     
#define dSPIN_UFP_PC_Port	        GPIOE

#define dSPIN_BPP_PC_Pin		GPIO_Pin_11     
#define dSPIN_BPP_PC_Port	        GPIOD

#define dSPIN_SBP_PC_Pin		GPIO_Pin_8     
#define dSPIN_SBP_PC_Port	        GPIOA

#define dSPIN_SFP_PC_Pin		GPIO_Pin_10     
#define dSPIN_SFP_PC_Port	        GPIOC
#endif

/**************** ��PWMʹ�õ�ʱ�Ӽ�ͨ���ź궨��  --���޸�, δʹ�õĶ���Ϊ0 **********/
  //ʱ��Ϊ��ʱ������: TIM1,TIM2,TIM3,...,TIM8��. δʹ�õıõ�ֵ�����һ������ͬ
#define  MOTOR1_TIM         TIM5    //  TIM5_CH2 PA1
#define  MOTOR2_TIM         TIM2    //     TIM2-CH3 (�ض���) PB10
#define  MOTOR3_TIM         TIM4    //     TIM4-CH3 (�ض���) PD14
#define  MOTOR4_TIM         TIM3    //     TIM3-CH4 (�ض���) PC9
#define  MOTOR5_TIM         TIM8    //     TIM8_CH1N  PA7
#define  MOTOR6_TIM         0       //
  //ͨ����Ϊ����: 1,2,3,4��δʹ�õıõ�ֵ�����һ������ͬ
#define  MOTOR1_CH          2   //
#define  MOTOR2_CH          3   //
#define  MOTOR3_CH          3   //
#define  MOTOR4_CH          4   //
#define  MOTOR5_CH          1   //
#define  MOTOR6_CH          0   //

  // PWM���ż��˿�����. δʹ�õıõ�ֵ�����һ������ͬ
#define  MOTOR1_PWM_PORT    GPIOA   //PA1
#define  MOTOR2_PWM_PORT    GPIOB   //
#define  MOTOR3_PWM_PORT    GPIOD  //
#define  MOTOR4_PWM_PORT    GPIOC   //
#define  MOTOR5_PWM_PORT    GPIOA   //
#define  MOTOR6_PWM_PORT    0   //
#define  MOTOR1_PWM_PIN     GPIO_Pin_1
#define  MOTOR2_PWM_PIN     GPIO_Pin_10
#define  MOTOR3_PWM_PIN     GPIO_Pin_14
#define  MOTOR4_PWM_PIN     GPIO_Pin_9
#define  MOTOR5_PWM_PIN     GPIO_Pin_7
#define  MOTOR6_PWM_PIN     0


#ifdef MOTOR_REALSPEED_CHECK_EN
//EXTI�������ض���:  EXTI_Trigger_Rising -������;
                  // EXTI_Trigger_Falling -�½���;
                  // EXTI_Trigger_Rising_Falling  - �����غ��½���
#define  MOTOR_REAL_EXTI_TRIGGER    EXTI_Trigger_Falling        //EXTI�������ض���
//EXTI-GPIO����
#define  MOTOR1_REAL_PORT           GPIOE   //PE11??����
#define  MOTOR2_REAL_PORT           GPIOD   //PD10
#define  MOTOR3_REAL_PORT           GPIOC   //PC8
#define  MOTOR4_REAL_PORT           GPIOA   //PA12
#define  MOTOR5_REAL_PORT           GPIOD   //PD5
#define  MOTOR6_REAL_PORT           0   //-
#define  MOTOR1_REAL_PIN            GPIO_Pin_11
#define  MOTOR2_REAL_PIN            GPIO_Pin_10
#define  MOTOR3_REAL_PIN            GPIO_Pin_8
#define  MOTOR4_REAL_PIN            GPIO_Pin_12
#define  MOTOR5_REAL_PIN            GPIO_Pin_5
#define  MOTOR6_REAL_PIN            0
//EXTI-�ⲿ�ж϶���
//  �ж�Դ�˿������Ŷ˿ڶ�Ӧ,��GPIOA����GPIO_PortSourceGPIOA;
//  �ж�Դ�ܽ���GPIO_Pin_x��Ӧ����GPIO_Pin_9����GPIO_PinSource9
//  �ж��߱����GPIO_Pin_x��Ӧ����GPIO_Pin_9����EXTI_Line9   *****Ϊ0,���ʾEXTI��ʹ�� *****
#define  MOTOR1_REAL_SourcePORT     GPIO_PortSourceGPIOE    //EXTI�ж�Դ�˿�         PE9
#define  MOTOR1_REAL_SourcePIN      GPIO_PinSource11          //EXTI�ж�Դ�ܽ�  GPIO_PinSource9
#define  MOTOR1_REAL_EXTI_Line      EXTI_Line11               //EXTI�ж��߱��
#define  MOTOR2_REAL_SourcePORT     GPIO_PortSourceGPIOD    //EXTI�ж�Դ�˿�         PD10 
#define  MOTOR2_REAL_SourcePIN      GPIO_PinSource10          //EXTI�ж�Դ�ܽ�
#define  MOTOR2_REAL_EXTI_Line      EXTI_Line10               //EXTI�ж��߱��
#define  MOTOR3_REAL_SourcePORT     GPIO_PortSourceGPIOC    //EXTI�ж�Դ�˿�         PC8
#define  MOTOR3_REAL_SourcePIN      GPIO_PinSource8          //EXTI�ж�Դ�ܽ�
#define  MOTOR3_REAL_EXTI_Line      EXTI_Line8               //EXTI�ж��߱��
#define  MOTOR4_REAL_SourcePORT     GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�        PA12
#define  MOTOR4_REAL_SourcePIN      GPIO_PinSource12          //EXTI�ж�Դ�ܽ�
#define  MOTOR4_REAL_EXTI_Line      EXTI_Line12               //EXTI�ж��߱��
#define  MOTOR5_REAL_SourcePORT     GPIO_PortSourceGPIOD    //EXTI�ж�Դ�˿�        PD7
#define  MOTOR5_REAL_SourcePIN      GPIO_PinSource7          //EXTI�ж�Դ�ܽ�
#define  MOTOR5_REAL_EXTI_Line      EXTI_Line7              //EXTI�ж��߱��
#define  MOTOR6_REAL_SourcePORT     0    //EXTI�ж�Դ�˿�
#define  MOTOR6_REAL_SourcePIN      0          //EXTI�ж�Դ�ܽ�
#define  MOTOR6_REAL_EXTI_Line      0               //EXTI�ж��߱��
#endif


/**************** ��ת�������ĳ���,��ÿת��������  --���޸�, δʹ�õĶ���Ϊ1 **********/
  
#define  MOTOR1_ENCODER     2    // 
#define  MOTOR2_ENCODER     2    //
#define  MOTOR3_ENCODER     2    // 
#define  MOTOR4_ENCODER     2    // 
#define  MOTOR5_ENCODER     16    // 16
#define  MOTOR6_ENCODER     1     //


extern u32 StepMotor_ADJ_Cnt;
extern void dSPIN_Work();
extern void dSPIN_CS_Configuration(StepMotor_TypeDef Motorx);
extern void dSPIN_FLAG_Configuration(StepMotor_TypeDef Motorx);
extern void dSPIN_PC_Configuration(StepMotor_TypeDef Motorx);
extern void dSPIN_PWM_Configuration(StepMotor_TypeDef Motorx);
extern void dSPIN_PC_Read(StepMotor_TypeDef Motorx);
extern void dSPIN_Status_Parse(u16 *P, u8 i);

extern void StepMotor_Set_DCY(StepMotor_TypeDef Motorx,u16 speed);
extern void StepMotor_Speed_ADJ(StepMotor_TypeDef Motorx,float Turn_speed);  
extern void StepMotor_TIM_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState); 
extern void StepMotor_PWM_TIM_IRQ(StepMotor_TypeDef Motorx);
extern void StepMotor_Init(void);
extern void Delay_Bus(u32 dly);
extern void StepMotor_EXTI_IRQ(StepMotor_TypeDef Motorx);
extern void StepMotor_Stop_Turn(StepMotor_TypeDef Motorx);   
#endif