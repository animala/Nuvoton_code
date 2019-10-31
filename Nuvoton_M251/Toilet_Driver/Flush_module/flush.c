#include "flush.h"

Flush_IO_cfg Flush_io;


void Flush_IO_init(Flush_IO_cfg *p,void (*flush_set_h)(),void (*flush_set_l)())
{
	p->Deo_set_high = flush_set_h;
	p->Deo_set_low = flush_set_l;	

}


//---------------------------
//----冲水板冲水处理
//--------------------------
void FlushWater_Board_Deal	(void)
{
	if(Status.chongshuiban_f)	
	{		
		Flush_io.Deo_set_high;
		chongshui.delay1++;
		if(chongshui.delay1 > 200)		//----200*10ms=2000ms
		{
			chongshui.delay1 = 0;
			Status.chongshuiban_f = 0;
		}
	}
	else
	{
		Flush_io.Deo_set_low;
	}
}



