#include "PID_Lib_usr.h"


/*		
	PID��3����׼���� --> 
	����ϵ��Kp
	����ϵ��Ti;
	΢��ϵ��Td
*/
	
const PIDKeyParaArray_TypeDef t_PIDKeyParaArray[PIDPara_MaxModeCount]={
	{0.25,5.0,0.1},  // ģʽ1;
	{0.25,5.0,0.1},  // ģʽ2;
	{0.25,5.0,0.1},  // ģʽ3;
};

/*		��ͬģʽ�ĳ���ֵ	*/
const t_u16 t_BeyondValue_Array[PIDPara_MaxModeCount]={
	0.1,  // ģʽ1;
	0.1,  // ģʽ2;
	0.1,  // ģʽ3;
};




