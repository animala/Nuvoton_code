#include "PID_Lib_usr.h"

PIDDataBuffer_Struct t_PIDDataBuffer[WorkOven_LimitCount];

/*
		返回值为输出量

		无论输入为温度，压力，流量和液位，PID控制器的输出都是一个0~100%的值;
*/

t_BOOL rPIDTemperAdjust_Proc(t_u16  ConstValue,        // 设定的恒定温度值;
			     float  CenterTemperValue, // 反馈值,经过补偿处理后的温度值;		
			     t_u8   WorkModeIndex,     // 工作模式索引;
			     t_u8   OvenIndex,	       // 腔体索引;
			     t_BOOL PIDEnable_Flag)    // 使能位
{
  static float rKi_Ptr=0;

	static t_u8 rCallBackIndex=0;
	static float rCallBackValue[PIDDataBuffer_Size]={0.0,0.0};

	if(PIDEnable_Flag==FALSE)
		return FALSE;

	t_PIDDataBuffer[OvenIndex].PIDPara_T=PID_RUN_PERIOD;/*运行周期*/

	rCallBackValue[rCallBackIndex]=CenterTemperValue;

	/*		PID的标准参数确定 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_Kp=\
									t_PIDKeyParaArray[WorkModeIndex].PIDKeyPara_Kp;
	
	t_PIDDataBuffer[OvenIndex].PIDPara_Ti=\
									t_PIDKeyParaArray[WorkModeIndex].PIDKeyPara_Ti;
	
	t_PIDDataBuffer[OvenIndex].PIDPara_Td=\
									t_PIDKeyParaArray[WorkModeIndex].PIDKeyPara_Td;

	t_PIDDataBuffer[OvenIndex].PIDPara_Ti=\
									t_PIDDataBuffer[OvenIndex].PIDPara_Kp/t_PIDDataBuffer[OvenIndex].PIDPara_Ti;
	
	t_PIDDataBuffer[OvenIndex].PIDPara_Td=\
								(t_PIDDataBuffer[OvenIndex].PIDPara_Kp)*(t_PIDDataBuffer[OvenIndex].PIDPara_Td);

	/*		当前给定值和反馈值及其误差值计算,索引系数越大，数据越新 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_ConstValue=ConstValue;

	t_PIDDataBuffer[OvenIndex].PIDPara_CallBackValue[rCallBackIndex]=\
																rCallBackValue[rCallBackIndex%PIDDataBuffer_Size];

	t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex]=\
		(float)(((float)(t_PIDDataBuffer[OvenIndex].PIDPara_ConstValue))-\
				((float)t_PIDDataBuffer[OvenIndex].PIDPara_CallBackValue[rCallBackIndex]));

	t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[(rCallBackIndex+1)%PIDDataBuffer_Size]=\
		(float)(((float)(t_PIDDataBuffer[OvenIndex].PIDPara_ConstValue))-\
				((float)t_PIDDataBuffer[OvenIndex].PIDPara_CallBackValue[(rCallBackIndex+1)%PIDDataBuffer_Size]));

	/*		比例项计算 权重0.3 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue=\
		(t_PIDDataBuffer[OvenIndex].PIDPara_Kp)*\
				t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex];
	
	/*		微分项计算 权重0.375 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue+=\
		((t_PIDDataBuffer[OvenIndex].PIDPara_Td)*\
			(t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex]-\
					t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[(rCallBackIndex+1)%PIDDataBuffer_Size]))/\
				(t_PIDDataBuffer[OvenIndex].PIDPara_T);

	/*		积分项计算 权重 0.02 */
	rKi_Ptr+=(t_PIDDataBuffer[OvenIndex].PIDPara_Ti)*\
		(t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex])*\
			(t_PIDDataBuffer[OvenIndex].PIDPara_T);

	if(rKi_Ptr>=(float)(t_BeyondValue_Array[WorkModeIndex]*1.0))
	{
		rKi_Ptr=(t_BeyondValue_Array[WorkModeIndex]*1.0);
	}
	else if((rKi_Ptr<=(-((float)(t_BeyondValue_Array[WorkModeIndex]*1.0)))))
	{
		rKi_Ptr=-((float)(t_BeyondValue_Array[WorkModeIndex]*1.0));
	}
	else
	{

	}

	t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue+=rKi_Ptr;

	rCallBackIndex=(rCallBackIndex+1)%PIDDataBuffer_Size;

	/*		PID计算结果输出 	*/
	if(t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue>=0.0)
	{
		if(t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue>1.0)
		{
			t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue=1.0;
		}
		else
		{
			
		}
	}
	else
	{
		t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue=0.0;
	}

	if(t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue>0.0)
	{
		if(t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue<=0.1)
		{
			t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue=0.1;
		}
		else
		{

		}

		t_PIDDataBuffer[OvenIndex].rPIDTimeCount=\
							(t_PIDDataBuffer[OvenIndex].rPIDTimeCount+1)%PIDAdjust_PeriodTime;

		if(t_PIDDataBuffer[OvenIndex].rPIDTimeCount<=\
			(t_u16)((t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue)*PIDAdjust_PeriodTime))
		{
			t_PIDDataBuffer[OvenIndex].RelayHeatStatus=TRUE; 
		}
		else
			t_PIDDataBuffer[OvenIndex].RelayHeatStatus=FALSE; 
	}
	else
	{
		t_PIDDataBuffer[OvenIndex].rPIDTimeCount=0;
		t_PIDDataBuffer[OvenIndex].RelayHeatStatus=FALSE; 
	}

	return t_PIDDataBuffer[OvenIndex].RelayHeatStatus;
}


