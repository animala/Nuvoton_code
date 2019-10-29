#include "PID_Lib_usr.h"


/*		
	PID的3个标准参数 --> 
	比例系数Kp
	积分系数Ti;
	微分系数Td
*/
	
const PIDKeyParaArray_TypeDef t_PIDKeyParaArray[PIDPara_MaxModeCount]={
	{0.25,5.0,0.1},  // 模式1;
	{0.25,5.0,0.1},  // 模式2;
	{0.25,5.0,0.1},  // 模式3;
};

/*		不同模式的超调值	*/
const t_u16 t_BeyondValue_Array[PIDPara_MaxModeCount]={
	0.1,  // 模式1;
	0.1,  // 模式2;
	0.1,  // 模式3;
};




