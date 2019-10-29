#include "PID_Lib_usr.h"

PIDDataBuffer_Struct t_PIDDataBuffer[WorkOven_LimitCount];

/*
		����ֵΪ�����

		��������Ϊ�¶ȣ�ѹ����������Һλ��PID���������������һ��0~100%��ֵ;
*/

t_BOOL rPIDTemperAdjust_Proc(t_u16  ConstValue,        // �趨�ĺ㶨�¶�ֵ;
			     float  CenterTemperValue, // ����ֵ,���������������¶�ֵ;		
			     t_u8   WorkModeIndex,     // ����ģʽ����;
			     t_u8   OvenIndex,	       // ǻ������;
			     t_BOOL PIDEnable_Flag)    // ʹ��λ
{
  static float rKi_Ptr=0;

	static t_u8 rCallBackIndex=0;
	static float rCallBackValue[PIDDataBuffer_Size]={0.0,0.0};

	if(PIDEnable_Flag==FALSE)
		return FALSE;

	t_PIDDataBuffer[OvenIndex].PIDPara_T=PID_RUN_PERIOD;/*��������*/

	rCallBackValue[rCallBackIndex]=CenterTemperValue;

	/*		PID�ı�׼����ȷ�� 	*/
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

	/*		��ǰ����ֵ�ͷ���ֵ�������ֵ����,����ϵ��Խ������Խ�� 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_ConstValue=ConstValue;

	t_PIDDataBuffer[OvenIndex].PIDPara_CallBackValue[rCallBackIndex]=\
																rCallBackValue[rCallBackIndex%PIDDataBuffer_Size];

	t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex]=\
		(float)(((float)(t_PIDDataBuffer[OvenIndex].PIDPara_ConstValue))-\
				((float)t_PIDDataBuffer[OvenIndex].PIDPara_CallBackValue[rCallBackIndex]));

	t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[(rCallBackIndex+1)%PIDDataBuffer_Size]=\
		(float)(((float)(t_PIDDataBuffer[OvenIndex].PIDPara_ConstValue))-\
				((float)t_PIDDataBuffer[OvenIndex].PIDPara_CallBackValue[(rCallBackIndex+1)%PIDDataBuffer_Size]));

	/*		��������� Ȩ��0.3 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue=\
		(t_PIDDataBuffer[OvenIndex].PIDPara_Kp)*\
				t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex];
	
	/*		΢������� Ȩ��0.375 	*/
	t_PIDDataBuffer[OvenIndex].PIDPara_OutPutValue+=\
		((t_PIDDataBuffer[OvenIndex].PIDPara_Td)*\
			(t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[rCallBackIndex]-\
					t_PIDDataBuffer[OvenIndex].PIDPara_ErrorGapValue[(rCallBackIndex+1)%PIDDataBuffer_Size]))/\
				(t_PIDDataBuffer[OvenIndex].PIDPara_T);

	/*		��������� Ȩ�� 0.02 */
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

	/*		PID��������� 	*/
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


