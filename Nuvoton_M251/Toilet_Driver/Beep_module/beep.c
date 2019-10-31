#include "beep.h"

Beep_IO_cfg Beep_io;

//===========================================�û������================================================

_User_ void  Beep_high   (void)
{
	PD15 = 1;
}
_User_ void  Beep_low  (void)
{
	PD15 = 0;
}


//��ʼ����Ӧƽ̨��IO��
_User_ void Beep_Gpio_init(void)
{
	PD->MODE = (PD->MODE &~(GPIO_MODE_MODE15_Msk) |(0x1UL << GPIO_MODE_MODE15_Pos));  //PD15 ���ģʽ

	beep_Io_init(Beep_high,Beep_low);
}




//=============================================�ײ�����===============================================
_Link_ void beep_Io_init(void (*Beep_set_h)(),void (*Beep_set_l)())
{
	Beep_io.Beep_set_high = Beep_set_h;
	Beep_io.Beep_set_low  = Beep_set_l;
}

//==========================================���ܴ����==================================================

/*********************************************************/
//	�������ã����û�����ʱ��
//	
/*******************************************************/
_Toilet_ void buz_set(uint_8 loop,uint_8 delay_H,uint_8 delay_L)
{
	buzzer.ring_times = loop;
	buzzer.ring_time_set = delay_H;
	buzzer.stop_time_set = delay_L;
	buzzer.task_count = 0;
}

//--  ������������
//-------------------------------------------
_Toilet_ void buz_task(void)
{
	if(buzzer.ring_times != 0)
	{
		if(buzzer.task_count == 0)
		{
			buzzer.ring_time = buzzer.ring_time_set;
			++buzzer.task_count;
       	} 
		else if(buzzer.task_count == 1) 
		{
			
			Beep_io.Beep_set_high;   //��ֲ
			if(--buzzer.ring_time == 0)
			++buzzer.task_count;
       	}
    		else if(buzzer.task_count == 2) 
       	{
			buzzer.stop_time = buzzer.stop_time_set;
			++buzzer.task_count;
		} 
		else if(buzzer.task_count == 3) 
		{
			Beep_io.Beep_set_low;   //��ֲ IO���õ�
			if(--buzzer.stop_time == 0)
			{
				if(buzzer.ring_times != 0xff)			//-----ֹͣ?��
				--buzzer.ring_times;
				buzzer.task_count = 0;
           		}
		}
	}
	else 
	{
		buzzer.task_count = 0;
		Beep_io.Beep_set_low;   //��ֲ IO���õ�
	}		
}











