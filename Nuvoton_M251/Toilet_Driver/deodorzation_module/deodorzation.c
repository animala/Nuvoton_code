#include "deodorzation.h"



Deo_IO_cfg Deo_io;

void Deodorization_IO_init(Deo_IO_cfg *p,void (*deo_set_h)(),void (*deo_set_l)())
{

	p->Deo_set_high = deo_set_h;
	p->Deo_set_low = deo_set_l;
}



//**************************
//
//    �������ܺ���
//
//**********************/

void Deodorization_Deal(void)
{	
	if(Status.test_f == 0)
	{
		if((Status.power_f == 1) && (Status.seat_f == 1))   //�ϵ����,����
		{
			if(Status.chuchou_e&&Status.chuchou_f&&(Status.dring_f==0)&&(Status.PowerEnd_f==1)) //����ʹ�ܣ����ں�ɣ���λ���
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







