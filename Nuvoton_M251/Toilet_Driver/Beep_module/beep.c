#include "beep.h"

Beep_IO_cfg Beep_io;




void beep_Io_init(Beep_IO_cfg *p,void (*Beep_set_h)(),void (*Beep_set_l)())
{

	p->Beep_set_high = Beep_set_h;
	p->Beep_set_low = Beep_set_l;

}



/*********************************************************/
//	DLYCTR  =  DLYCTR | 0x80 ; 		//----- Buzzer output ON 
//	DLYCTR  =  DLYCTR & 0x7f ; 		//----- Buzzer output ON 
/*******************************************************/
void  buz_set(uint_8 loop,uint_8 delay_H,uint_8 delay_L)
{
	buzzer.ring_times = loop;
	buzzer.ring_time_set = delay_H;
	buzzer.stop_time_set = delay_L;
	buzzer.task_count = 0;
}
//--
//-------------------------------------------
void buz_task(void)
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
			
			Beep_io.Beep_set_high;   //“∆÷≤
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
			Beep_io.Beep_set_low;   //“∆÷≤ IO…Ë÷√µÕ
			if(--buzzer.stop_time == 0)
			{
				if(buzzer.ring_times != 0xff)			//-----Õ£÷π?Ω–
				--buzzer.ring_times;
				buzzer.task_count = 0;
           		}
		}
	}
	else 
	{
		buzzer.task_count = 0;
		Beep_io.Beep_set_low;   //“∆÷≤ IO…Ë÷√µÕ
	}		
}











