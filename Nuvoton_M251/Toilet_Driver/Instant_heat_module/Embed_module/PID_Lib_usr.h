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

#define PIDDataBuffer_Size				2		      //计算缓冲大小

#define	WorkOven_LimitCount				2		      //腔体数目

#define PIDPara_MaxModeCount				20   		      // 20种工作模式;

#define PID_RUN_PERIOD				       (0.2)		      //运算周期
#define	PIDAdjust_PeriodTime			       (60*(1/PID_RUN_PERIOD))//PID调整的总周期,秒为单位;


typedef struct
{
	float PIDKeyPara_Kp;		
	float PIDKeyPara_Ti;
	float PIDKeyPara_Td;
}PIDKeyParaArray_TypeDef;

typedef struct
{
	/*		
	PID的3个标准参数 --> 

	比例系数Kp
	积分系数Ti;
	微分系数Td
	*/
	float PIDPara_Kp;		
	float PIDPara_Ti;
	float PIDPara_Td;

	float  PIDPara_T;  

	t_u16 PIDPara_ConstValue;  // 固定值,各个模式的设定温度值;
	float PIDPara_CallBackValue[PIDDataBuffer_Size];  // 反馈值,中心温度值;
	float PIDPara_ErrorGapValue[PIDDataBuffer_Size];

  	float PIDPara_OutPutValue;  		// 输出值;

	t_u16 rPIDTimeCount;

	t_BOOL RelayHeatStatus;  
}
PIDDataBuffer_Struct;

extern PIDDataBuffer_Struct t_PIDDataBuffer[WorkOven_LimitCount];

extern const PIDKeyParaArray_TypeDef t_PIDKeyParaArray[PIDPara_MaxModeCount];
extern const t_u16 t_BeyondValue_Array[PIDPara_MaxModeCount];

/*		
	返回值为TRUE/FALSE

	TRUE -  启动加热;
	FALSE - 停止加热或停止PID计算;

	该函数一般1s~10s计算一次即可;
	
*/
t_BOOL rPIDTemperAdjust_Proc(t_u16 ConstValue,        // 设定的恒定温度值;
			     float CenterTemperValue, // 反馈值,经过补偿处理后的温度值;		
			     t_u8 WorkModeIndex,      // 工作模式索引;
			     t_u8 OvenIndex,	      // 腔体索引;如果只有一个腔体，默认为0即可;
			     t_BOOL PIDEnable_Flag) ; // 是否使能PID运算;


#endif

