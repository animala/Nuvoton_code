#ifndef _PID_Lib_usr_h
#define _PID_Lib_usr_h

typedef unsigned char 		t_u8;
typedef unsigned int		t_u16;
typedef unsigned long int 	t_u32;

typedef enum
{
	FALSE=0,
	TRUE
}t_BOOL;

#define PIDDataBuffer_Size				2		      //���㻺���С

#define	WorkOven_LimitCount				2		      //ǻ����Ŀ

#define PIDPara_MaxModeCount				20   		      // 20�ֹ���ģʽ;

#define PID_RUN_PERIOD				       (0.2)		      //��������
#define	PIDAdjust_PeriodTime			       (60*(1/PID_RUN_PERIOD))//PID������������,��Ϊ��λ;


typedef struct
{
	float PIDKeyPara_Kp;		
	float PIDKeyPara_Ti;
	float PIDKeyPara_Td;
}PIDKeyParaArray_TypeDef;

typedef struct
{
	/*		
	PID��3����׼���� --> 

	����ϵ��Kp
	����ϵ��Ti;
	΢��ϵ��Td
	*/
	float PIDPara_Kp;		
	float PIDPara_Ti;
	float PIDPara_Td;

	float  PIDPara_T;  

	t_u16 PIDPara_ConstValue;  // �̶�ֵ,����ģʽ���趨�¶�ֵ;
	float PIDPara_CallBackValue[PIDDataBuffer_Size];  // ����ֵ,�����¶�ֵ;
	float PIDPara_ErrorGapValue[PIDDataBuffer_Size];

  	float PIDPara_OutPutValue;  		// ���ֵ;

	t_u16 rPIDTimeCount;

	t_BOOL RelayHeatStatus;  
}
PIDDataBuffer_Struct;

extern PIDDataBuffer_Struct t_PIDDataBuffer[WorkOven_LimitCount];

extern const PIDKeyParaArray_TypeDef t_PIDKeyParaArray[PIDPara_MaxModeCount];
extern const t_u16 t_BeyondValue_Array[PIDPara_MaxModeCount];

/*		
	����ֵΪTRUE/FALSE

	TRUE -  ��������;
	FALSE - ֹͣ���Ȼ�ֹͣPID����;

	�ú���һ��1s~10s����һ�μ���;
	
*/
t_BOOL rPIDTemperAdjust_Proc(t_u16 ConstValue,        // �趨�ĺ㶨�¶�ֵ;
			     float CenterTemperValue, // ����ֵ,���������������¶�ֵ;		
			     t_u8 WorkModeIndex,      // ����ģʽ����;
			     t_u8 OvenIndex,	      // ǻ������;���ֻ��һ��ǻ�壬Ĭ��Ϊ0����;
			     t_BOOL PIDEnable_Flag) ; // �Ƿ�ʹ��PID����;


#endif

