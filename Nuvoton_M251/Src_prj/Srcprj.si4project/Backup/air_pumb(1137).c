#include "air_pumb.h"
#include "Pwm_cfg.h"
#include "toilet.h"


//*********************************************
//
//   	ˮѹ������ 
//	����ִ�����ٴΰ��¸���ϴ������λ����ˮѹ
//*******************************************
void Water_press_deal(void)
{
	uint_8 temp = 0;
	
	if((Water.WaterIn_f == 1) && (Status.QiBeng_f == 1))
	{
		
		if(Status.Mode_select > 0)
		{
			temp = Status.Icon.water_yeild;  //ˮѹ��λ����ˮ����λ���Ӷ�����
			
			if(Status.Icon.water_yeild > 2)   //С��2��Ϊ��ѹ�������ѹ
			{
				Set_Pwm_value(ON, temp-1);   //�ѵ�λ����PWM�������õ�pwm����
			}
			else
			{
				Set_Pwm_value(OFF, temp-1);   //�ر�PWM���
			}
			
		}
	}
	else
	{
		Set_Pwm_value(OFF,0);	//�ر�PWM����ѹ  
	}
}






