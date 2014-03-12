#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H
typedef struct       
{
    u8  Enable;     //电机启/停控制命令： DISABLE=关闭电机；ENABLE=使能电机工作
    u8  Status;
    u8  SetDir;     //设定转动方向:1=反转;0=正转
    float SetSpeed;   //设定转速(rpm)
    float CurrSpeed;  //当前转速(rpm)
    u8  ToBottom;    //1=到底;0=未到底
    u8  ToTop;       //1=到顶;0=未到顶
    u8  Alert;       //过热、过流：1=报警,0=正常
    u8  NF;          //缺相：1=报警,0=正常
    u8  Braking;     //强制制动(泵盖等)引脚的状态：1=制动有效;0=不制动,正常运行
    u8  RunSta_Brak; //暂存强制制动(泵盖等)前泵的工作状态,待取消强制制动后,自动恢复运行
    u8  LockRotor;   //堵转：1=堵转报警,0=正常
    u32 Flow;        //流量累计(PWM脉冲(中断)累计)
    u8  FlowErr;            //读:通过流量累加检测的流量异常(或堵转):1=流量异常
    u32 FlowLockRotor;   //堵转检测用的PWM脉冲计数
    u32 LockRotorSpeed;  //堵转检测用的实际转速检测计数，即编码盘或HALL信号
    float RealSpeed;       //真实转速(rpm) - 通过转速检测信号实测得到的转速
    u32   RealPulse;       //真实转速脉冲累计，用于计算真实转速，每分钟清0一次
    u8  RatioSwitch;      //泵管系数测试开关
    u8  Down_Mode;        //下行运转模式(00正常,01管路安装,10泵管系数测试，11自检模式)
    u8  Up_Mode;
    float  Ratio;            //泵管系数ml/R
    float  SetFlow;          //设定流量(0~100ml/h*10)放大10倍
    u32 Step;
    u32 Temp_Circle;        //一周期内圈数
    u32 Circle;
    float ReadSpeed;
    u8 CMD;
}StepMotor_WorkInfo_Def;    //上行下行结构体封装
extern StepMotor_WorkInfo_Def StepMotor_Data[];   

typedef enum   
{
  UFP = 0,   //  - 为MOTOR1    超滤泵
  BPP,       //  - 为MOTOR2    分流泵
  SBP,       //    - 为MOTOR3  补液泵
  SFP,       //   - 为MOTOR4   功能泵
  HP,        //   - 为MOTOR5
}StepMotor_TypeDef;



#define MOTOR_SPEED_EN      //DCY功能有效
#define SPEED_VAL1  20     //56电机小于25RPM  
#define SPEED_VAL2  50    //56电机小于50RPM 
#define SPEED_VAL3  80    //56电机小于75RPM 
#define SPEED_VAL4  100   //56电机小于100RPM

#define MOTOR_Stop_Turn           //比较转速跟霍尔反馈测堵转
#define HP_INSTALL_EN                   //肝素泵安装
#define MOTOR_BRAKING_EN              //强制刹车(泵盖等检测)有效
//#define MOTOR_PWM_EN                   //IO产生PWM使能
//#define MOTOR_PWM_IRQ_EN              //配置PWM的TIM中断有效，主要用于流量脉冲累计
//#define MOTOR_PWM_LOCKROTOR_CHECK_EN  //在PWM中断检测堵转有效(条件：MOTOR_REALSPEED_CHECK_EN和MOTOR_PWM_IRQ_EN必须有效)
#define MOTOR_REALSPEED_CHECK_EN      //马达真实转速检测有效(检测方式为霍尔或编码盘)

#define STEPMOTOR_COEFFICIENT     168750.0  //步进脉冲的TIM系数337500
#define STEPMOTOR_PWMPULSE        25600     //步进电机每转的PWM脉冲数 =(360度/1.8度)*128

#define MOTOR_TOTAL   5                  //系统中实际使用的电机总个数 [必须一致]
#define STEPMOTOR_PULSE   256            //


#define STEPMOTOR_ADJSPEED_SPACE    30  //调速间隔时间(单位ms)
#define STEPMOTOR_ADJSPEED_STEP     1  //2rmp - 每次调速增/减的步长（转数）

#define STEPMOTOR_IN_DEBOUNCE       5    //步进电机的所有相关输入引脚去抖动次数
#define STEPMOTOR_NF_CHECK_10S      1500  //缺相检测周期10s: 0.01s*1000
#define STEPMOTOR_MAX_SETSPEED      1000.0 //转速限制，A,B泵最大250rpm

// CS片选引脚及端口设置. 未使用的泵的值与最后一个泵相同
#define dSPIN_UFP_CS_Pin		GPIO_Pin_7     //UFP 片选
#define dSPIN_UFP_CS_Port		GPIOE

#define dSPIN_BPP_CS_Pin		GPIO_Pin_12     //BPP 片选
#define dSPIN_BPP_CS_Port		GPIOB

#define dSPIN_SBP_CS_Pin		GPIO_Pin_15     //SBP 片选
#define dSPIN_SBP_CS_Port		GPIOD

#define dSPIN_SFP_CS_Pin		GPIO_Pin_9     //SFP 片选
#define dSPIN_SFP_CS_Port		GPIOA

#define dSPIN_HP_CS_Pin		        GPIO_Pin_11     //HP 片选
#define dSPIN_HP_CS_Port		GPIOC

// FLAG引脚及端口设置. 未使用的泵的值与最后一个泵相同
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


// PC泵盖检测引脚及端口设置. 未使用的泵的值与最后一个泵相同
#ifdef  MOTOR_BRAKING_EN   //强制刹车功能有效
#define MOTOR_BRAKING_INVERTER    1    //1=GPIO端为高电平强制刹车;0=GPIO端为低电平强制刹车
#define dSPIN_UFP_PC_Pin		GPIO_Pin_12     
#define dSPIN_UFP_PC_Port	        GPIOE

#define dSPIN_BPP_PC_Pin		GPIO_Pin_11     
#define dSPIN_BPP_PC_Port	        GPIOD

#define dSPIN_SBP_PC_Pin		GPIO_Pin_8     
#define dSPIN_SBP_PC_Port	        GPIOA

#define dSPIN_SFP_PC_Pin		GPIO_Pin_10     
#define dSPIN_SFP_PC_Port	        GPIOC
#endif

/**************** 泵PWM使用的时钟及通道号宏定义  --可修改, 未使用的定义为0 **********/
  //时钟为定时器代号: TIM1,TIM2,TIM3,...,TIM8等. 未使用的泵的值与最后一个泵相同
#define  MOTOR1_TIM         TIM5    //  TIM5_CH2 PA1
#define  MOTOR2_TIM         TIM2    //     TIM2-CH3 (重定义) PB10
#define  MOTOR3_TIM         TIM4    //     TIM4-CH3 (重定义) PD14
#define  MOTOR4_TIM         TIM3    //     TIM3-CH4 (重定义) PC9
#define  MOTOR5_TIM         TIM8    //     TIM8_CH1N  PA7
#define  MOTOR6_TIM         0       //
  //通道号为数字: 1,2,3,4。未使用的泵的值与最后一个泵相同
#define  MOTOR1_CH          2   //
#define  MOTOR2_CH          3   //
#define  MOTOR3_CH          3   //
#define  MOTOR4_CH          4   //
#define  MOTOR5_CH          1   //
#define  MOTOR6_CH          0   //

  // PWM引脚及端口设置. 未使用的泵的值与最后一个泵相同
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
//EXTI触发边沿定义:  EXTI_Trigger_Rising -上升沿;
                  // EXTI_Trigger_Falling -下降沿;
                  // EXTI_Trigger_Rising_Falling  - 上升沿和下降沿
#define  MOTOR_REAL_EXTI_TRIGGER    EXTI_Trigger_Falling        //EXTI触发边沿定义
//EXTI-GPIO定义
#define  MOTOR1_REAL_PORT           GPIOE   //PE11??飞线
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
//EXTI-外部中断定义
//  中断源端口与引脚端口对应,如GPIOA的是GPIO_PortSourceGPIOA;
//  中断源管脚与GPIO_Pin_x对应，如GPIO_Pin_9的是GPIO_PinSource9
//  中断线编号与GPIO_Pin_x对应，如GPIO_Pin_9的是EXTI_Line9   *****为0,则表示EXTI不使用 *****
#define  MOTOR1_REAL_SourcePORT     GPIO_PortSourceGPIOE    //EXTI中断源端口         PE9
#define  MOTOR1_REAL_SourcePIN      GPIO_PinSource11          //EXTI中断源管脚  GPIO_PinSource9
#define  MOTOR1_REAL_EXTI_Line      EXTI_Line11               //EXTI中断线编号
#define  MOTOR2_REAL_SourcePORT     GPIO_PortSourceGPIOD    //EXTI中断源端口         PD10 
#define  MOTOR2_REAL_SourcePIN      GPIO_PinSource10          //EXTI中断源管脚
#define  MOTOR2_REAL_EXTI_Line      EXTI_Line10               //EXTI中断线编号
#define  MOTOR3_REAL_SourcePORT     GPIO_PortSourceGPIOC    //EXTI中断源端口         PC8
#define  MOTOR3_REAL_SourcePIN      GPIO_PinSource8          //EXTI中断源管脚
#define  MOTOR3_REAL_EXTI_Line      EXTI_Line8               //EXTI中断线编号
#define  MOTOR4_REAL_SourcePORT     GPIO_PortSourceGPIOA    //EXTI中断源端口        PA12
#define  MOTOR4_REAL_SourcePIN      GPIO_PinSource12          //EXTI中断源管脚
#define  MOTOR4_REAL_EXTI_Line      EXTI_Line12               //EXTI中断线编号
#define  MOTOR5_REAL_SourcePORT     GPIO_PortSourceGPIOD    //EXTI中断源端口        PD7
#define  MOTOR5_REAL_SourcePIN      GPIO_PinSource7          //EXTI中断源管脚
#define  MOTOR5_REAL_EXTI_Line      EXTI_Line7              //EXTI中断线编号
#define  MOTOR6_REAL_SourcePORT     0    //EXTI中断源端口
#define  MOTOR6_REAL_SourcePIN      0          //EXTI中断源管脚
#define  MOTOR6_REAL_EXTI_Line      0               //EXTI中断线编号
#endif


/**************** 旋转编码器的齿数,即每转的脉冲数  --可修改, 未使用的定义为1 **********/
  
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