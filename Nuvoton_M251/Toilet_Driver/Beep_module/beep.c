#include "beep.h"

Beep_IO_cfg Beep_io;

//===========================================用户处理层================================================

_User_ void  Beep_high   (void)
{
	PD15 = 1;
}
_User_ void  Beep_low  (void)
{
	PD15 = 0;
}


//初始化对应平台的IO口
_User_ void Beep_Gpio_init(void)
{
	PD->MODE = (PD->MODE &~(GPIO_MODE_MODE15_Msk) |(0x1UL << GPIO_MODE_MODE15_Pos));  //PD15 输出模式

	beep_Io_init(Beep_high,Beep_low);
}




//=============================================底层抽象层===============================================
_Link_ void beep_Io_init(void (*Beep_set_h)(),void (*Beep_set_l)())
{
	Beep_io.Beep_set_high = Beep_set_h;
	Beep_io.Beep_set_low  = Beep_set_l;
}

//==========================================功能处理层==================================================

/*********************************************************/
//	声音设置，设置回音的时长
//	
/*******************************************************/
_Toilet_ void buz_set(uint_8 loop,uint_8 delay_H,uint_8 delay_L)
{
	buzzer.ring_times = loop;
	buzzer.ring_time_set = delay_H;
	buzzer.stop_time_set = delay_L;
	buzzer.task_count = 0;
}

//--  蜂鸣器处理函数
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
			
			Beep_io.Beep_set_high;   //移植
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
			Beep_io.Beep_set_low;   //移植 IO设置低
			if(--buzzer.stop_time == 0)
			{
				if(buzzer.ring_times != 0xff)			//-----停止?叫
				--buzzer.ring_times;
				buzzer.task_count = 0;
           		}
		}
	}
	else 
	{
		buzzer.task_count = 0;
		Beep_io.Beep_set_low;   //移植 IO设置低
	}		
}











