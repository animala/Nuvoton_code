#include "air_pumb.h"
#include "Pwm_cfg.h"
#include "toilet.h"


//*********************************************
//
//   	水压处理函数 
//	功能执行中再次按下该清洗功能则位调节水压
//*******************************************
void Water_press_deal(void)
{
	uint_8 temp = 0;
	
	if((Water.WaterIn_f == 1) && (Status.QiBeng_f == 1))
	{
		
		if(Status.Mode_select > 0)
		{
			temp = Status.Icon.water_yeild;  //水压档位随着水量档位增加而增加
			
			if(Status.Icon.water_yeild > 2)   //小于2档为常压，无须加压
			{
				Set_Pwm_value(ON, temp-1);   //把档位传给PWM进行气泵的pwm控制
			}
			else
			{
				Set_Pwm_value(OFF, temp-1);   //关闭PWM输出
			}
			
		}
	}
	else
	{
		Set_Pwm_value(OFF,0);	//关闭PWM，常压  
	}
}






