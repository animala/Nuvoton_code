//  Function		:   Moto control head file
//  Date			:   2019/7/15
//  Author			:	Gray Huang
#ifndef _MOTO_DEAL_H
#define _MOTO_DEAL_H
#include "MN101AF50D.h"
#include "main.h"

#define GPIO_OUT_ADDRBASE	0x3E70
#define	GPIO_OUT(x)	__UMEM8(GPIO_OUT_ADDRBASE + x)     //����IO�������XΪ�ڼ���IO��

#define Motor_sum  2      //�������

#define	Motor_ShuntValve	0    //��ˮ���
#define	Motor_SprayBoom		1    //��˵��
#define	Motor_Drying	 	2	  //��˵��

//-----------��������˿ڿ���
//-----------------------��ϴ��----p51---p54
#define 	StepMotor_B_IO		5		//P5��			
#define 	StepMotorBStop		0xe1
//------------------------��ɸ�---P62---P65
#define 	StepMotor_C_IO		6		//P6��			
#define 	StepMotorCStop		0xC3					
//------------------------��ˮ��---P83,P84,P86,P87
#define 	StepMotor_A_IO		8		//P8��					
#define 	StepMotorAStop		0x27


//--����һ��IO�ڿ��ƽṹ��
typedef struct{
	uint_16 port;			//���IO
	uint_8  stop;			//ֹͣλ
	uint_8  *phase;			//��λ
	uint_16 *position;		//��Ӧ����ĳ�ʼ��λ��
}Port_set;

//--����һ��������ƽṹ��
typedef struct{
	uint_16	Index;				//--------�����缫ʵ��λ��ƫ����
	uint_16 SumCountSet;		//--------�����缫����Ŀ��λ��
	uint_8 	XiangWei;			//--------�����缫��λλ��
	uint_8 	State;				//--------�����缫��������
	uint_8 	DelayTime;			//--------��ʱʱ��
	uint_8	enable_f:1;			//--------ʹ�ܱ�־	
	uint_8	cw_f:1;				//--------�����־
	uint_8	end_f:1;			//--------������־
	uint_8  select;				//--------���ѡ��
	uint_8  Compelet_flag;		//--------��ɱ�־
	Port_set *pInit;			//--------IO ��λ ��ʼ���ṹ��ָ��
	//Post_set *position;			//--------λ��		
}Motor_uion;

extern Motor_uion *motor_select[2];
extern Motor_uion MotorA;
extern Motor_uion MotorB;


//--����һ������λ��ѡ��ṹ��
typedef struct{
	uint_8 Warter_Presser;		//ˮѹѡ��
	uint_8 Pressure_turning;	//ˮѹ΢��	
	uint_8 SprayBoom_position;  //���λ��
	uint_8 SprayBoom_turning;   //���΢��
}Posotion_Select;
//--����һ������ģʽѡ��ṹ��
typedef struct{
	uint_8 Mode;   //����ģʽ
	uint_8 Mode_Delay;  //����ʱ��
	uint_8 Wash_time;
	Posotion_Select *position;
}Washe_fun;

typedef uint_16 (*slt_fun)(uint_8 x1, uint_8 x2);   //����һ������ָ��

void Motor_Control(Motor_uion *p);   //����һ��ָ�뺯��
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
