#ifndef __FLAG_H
#define __FLAG_H

extern u8  Type;

typedef struct
{
    float Ratio;            //泵管系数
    u32  Flow;               //总的累计流量
    u32 Old_Flow;          //上一次累计的流量
    u32 Circle;           //上一次累计的圈数
}Flow;
extern Flow Integrated[];


#define CODE             1023      //部件标识
#define H_VER            0100      //硬件版本
#define S_VER            0100      //软件版本   

#define HP_Value  13500

#define  NEW_CMD       BUS_Buffer.WR0[0] //公共信息(1:有新命令，0：没有新命令)
#define  NEW_CMD_SFP   BUS_Buffer.WR1[0] //新命令标志(1:有新命令，0：没有新命令)
#define  NEW_CMD_SBP   BUS_Buffer.WR2[0] //新命令标志(1:有新命令，0：没有新命令)
#define  NEW_CMD_UFP   BUS_Buffer.WR3[0] //新命令标志(1:有新命令，0：没有新命令)
#define  NEW_CMD_BPP   BUS_Buffer.WR4[0] //新命令标志(1:有新命令，0：没有新命令)
#define  NEW_CMD_HP    BUS_Buffer.WR5[0] //新命令标志(1:有新命令，0：没有新命令)
#define  NEW_CMD2_HP   BUS_Buffer.WR6[0] //新命令标志(1:有新命令，0：没有新命令)

#define INSTALL_Speed   50      //管路安装时泵转速
#define INSTALL_Count  0.5    //管路安装时泵转半圈
#define SELF_Speed   10        //泵自检时转速除HP,慢力矩大  泵的转速范围0~105 rpm/min



#define NORMAL_RUN   0       //正常运转模式
#define SLOW_RUN     1       //管路安装缓动模式,针对肝素泵是快进模式
#define C_TEST       2       //泵管系数测试模式
#define SELF_TEST    3       //自检模式
#define SELF_Finish  4       //自检完成

#define SELF_TEST_NORMAL    0    //正常状态
#define SELF_TEST_RUNING    1    //泵正在自检
#define SELF_TEST_PASS      2    //泵完成自检
#define SELF_TEST_ERROR     3    //泵自检异常

extern u16 Motor_Time;
extern u16 HP_Time ;

extern void Down_flag();
extern void Up_flag();
extern void Motor_Run_Mode();
extern void FlowClearZero(u8 i);

#endif 