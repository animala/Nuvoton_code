//  Function		:   Moto control head file
//  Date			:   2019/7/15
//  Author			:	Gray Huang
#ifndef _MOTO_DEAL_H
#define _MOTO_DEAL_H

#include "Toilet.h"
#include "NuMicro.h"

#ifdef EF_50D
#define GPIO_OUT_ADDRBASE	0x3E70
#define	GPIO_OUT(x)	__UMEM8(GPIO_OUT_ADDRBASE + x)     //定义IO口输出，X为第几组IO口
#else   //新塘M251
#define	GPIO_OUT(Port, pin)	GPIO_SET_OUT_DATA(Port,pin)     //定义IO口输出，X为第几组IO口
#endif






#define Motor_sum  2      //电机总数

#define	Motor_ShuntValve	0    //分水电机
#define	Motor_SprayBoom		1    //喷杆电机
#define	Motor_Drying	 	2	  //烘杆电机

//-----------步进电机端口控制
//-----------------------冲洗杆-----------PD
#define 	StepMotor_B_IO		PD		//PD0，PD1,PD2,PD3	 新唐M251方案		
#define 	StepMotorBStop		0xf0   //停止控制，4个IO都输出0

#if 0  // 未使用烘干杆
//------------------------烘干杆---P62---P65
#define 	StepMotor_C_IO		PB		//P6口			
#define 	StepMotorCStop		0xC3		
#endif
//------------------------分水阀-----------
#define 	StepMotor_A_IO		PC		//PC口 PC0,PC1，PC2,PC3		新唐M251方案			
#define 	StepMotorAStop		0xf0


//--声明一个IO口控制结构体
typedef struct{
	GPIO_T  *port;			//输出 PORT  移植需要看情况更改这里的IO口结构体指针  
	uint_8  stop;			//停止位
	uint_8  *phase;			//相位
	uint_16 *position;		//对应电机的初始化位置
}Port_set;

//--声明一个电机控制结构体
typedef struct{
	uint_16	Index;				//--------步进电极实际位置偏移量
	uint_16 SumCountSet;		//--------步进电极设置目标位置
	uint_8 	XiangWei;			//--------步进电极相位位置
	uint_8 	State;				//--------步进电极工作步骤
	uint_8 	DelayTime;			//--------延时时间
	uint_8	enable_f:1;			//--------使能标志	
	uint_8	cw_f:1;				//--------方向标志
	uint_8	end_f:1;			//--------结束标志
	uint_8  select;				//--------电机选择
	uint_8  Compelet_flag;		//--------完成标志
	Port_set *pInit;			//--------IO 相位 初始化结构体指针
	//Post_set *position;			//--------位置		
}Motor_uion;

extern Motor_uion *motor_select[2];
extern Motor_uion MotorA;
extern Motor_uion MotorB;


//--声明一个功能位置选择结构体
typedef struct{
	uint_8 Warter_Presser;		//水压选择
	uint_8 Pressure_turning;	//水压微调	
	uint_8 SprayBoom_position;  //喷杆位置
	uint_8 SprayBoom_turning;   //喷杆微调
}Posotion_Select;
//--声明一个功能模式选择结构体
typedef struct{
	uint_8 Mode;   //功能模式
	uint_8 Mode_Delay;  //功能时间
	uint_8 Wash_time;
	Posotion_Select *position;
}Washe_fun;

typedef uint_16 (*slt_fun)(uint_8 x1, uint_8 x2);   //声明一个函数指针

void Motor_Control(Motor_uion *p);   //声明一个指针函数
void Power_up_reset(Motor_uion **t);
void DringPwmDeal(void);
void Motor_A_B_C_program(Motor_uion **P, Status_uion *S);
void PowerUp_StepMotorA_B_C(void);
void penkouqingxi(void);
void DealCSKandSS_end(void);
void DealCSKandSS(void);
void Motor_set_position(Motor_uion *t, Status_uion *p);
void Reciprocate_fun(Motor_uion *t, Status_uion *p);
void Spout_clean(Motor_uion **t, Status_uion *s);


#endif
