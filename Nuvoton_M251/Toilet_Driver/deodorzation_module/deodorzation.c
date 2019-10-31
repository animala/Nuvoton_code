#include "deodorzation.h"



Deo_IO_cfg Deo_io;

void Deodorization_IO_init(Deo_IO_cfg *p,void (*deo_set_h)(),void (*deo_set_l)())
{

	p->Deo_set_high = deo_set_h;
	p->Deo_set_low = deo_set_l;
}



//**************************
//
//    除臭功能函数
//
//**********************/

void Deodorization_Deal(void)
{	
	if(Status.test_f == 0)
	{
		if((Status.power_f == 1) && (Status.seat_f == 1))   //上电完成,入座
		{
			if(Status.chuchou_e&&Status.chuchou_f&&(Status.dring_f==0)&&(Status.PowerEnd_f==1)) //除臭使能，不在烘干，复位完成
			{
				Deo_io.Deo_set_high;
			}
			else
			{
				Deo_io.Deo_set_low;
			}
		}
		else
		{
			Deo_io.Deo_set_low;
		}	
	}
}







