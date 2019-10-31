#include "sit_check.h"


Sitcheck_IO_cfg Sitcheck_io;


//
//   �����IO��ֲ��ʼ����ֻ�贫���Ӧ��Ƭ����IO�� ���뺯������
// 
void SitCheck_IO_Init(Sitcheck_IO_cfg *p, uint_16 (*check_pin)(void))
{

	p->Sitcheck_pin = check_pin;
	
}



//********************************************/
//
//�������~
//
//******************************************/
void Sitdown_Check(void)
{
	if(Sitcheck_io.Sitcheck_pin == 0)  //��⵽����
	{
		Seat.count1++;
		Seat.count2 = 0;
		if(Seat.count1 > 10)
		{
			//------------100MS������Ч
			Entironment.time = T30Min;     //��ȡ�����¶ȵ�ʱ��
			CoverSeat.delay = T5Min;      // ������һֱ�����ǹرռ�ʱΪ5����
			CoverSeat.enable_f = 1;
			Status.chuchou_f = 1;		//-----��������ʹ��
			Status.jieneng_e = 0;		//�رս���
			Seat.Flag |= 0x01;
			if(Status.seat_f == 0)
			{
				Status.seat_f = 1;
				Status.power_f = 1;		//������־λ��1
				//SpeedinessHeatPowerOn;
				//WifiPowerOn;
				//CO2_clr_seat;
				Status.fangwu_f = 1;	//���۱�־λ��1
				fangwu.step = 0;		
				penkou.step = 0;		//���������ϴ��ͷ������ֹͣ�ջ�
				Status.penkou_f = 0;		
				buz_set(1,30,30);	
			}
			if(Seat.count1 > 1000)		//---------����10S�Զ���ˮ����
			{
				Seat.count1 = 1000;	
				if(Seat.first_f == 0)
				{
					Seat.first_f = 1;
					if(Status.auto_f == 1)
					{
						Status.chongshui_e = 1; 
					}
				}				
			}		
		}
	}
	else
	{
		//------------����2S��Ч
		Seat.count2++;
		Seat.SeatOnTimer = T2Min;
		
		if(Seat.count2 > 200)		//--400*10=4000MS
		{
			Seat.count1 = 0;
			Drying.swash_f = 0;
			Status.chuchou_e = 1;
			Seat.Flag |= 0x02;
			if(Status.seat_f == 1)			//-----
			{
				Status.seat_f = 0;	
				//CO2_set_seat;
				Seat.first_f = 0;
				Status.stop_f = 1;
				Status.Mode_select = 0;
				work.LianXu_f = 0;
				work.LianXuBuf = 0;
				work.step = 0;	
				if(Status.dring_f == 1)   //���ں�ɵ����������
				{
					Status.dring_f = 0;
					Drying.enable_f = 0;
					Drying.step = 0;
					Drying.HotEndTime = T10S;
				}
				work.time = 0;
				Status.ShuiYaAnMo_f = 0;
				Status.reciprocate_f = 0;
				Status.dring_wf_f = 0;
				Status.ZuoYu_f = 0;
				Status.LengReAnMo_f = 0;
				if(penkou.enable_f == 1)  //������ϴ��ڵ����������
				{
					penkou.step = 0;
					Status.penkou_f = 0;
				}					
			}
		}
		//----------����6S�Զ���ˮ
		if(Seat.count2 > 600)		
		{
			if(MachineTest.Flag == 0)
			{
				if(Status.auto_f)
				{
					if(Status.chongshui_e)
					{
						Status.chongshui_e = 0;
						//Status.chongshui_f = 1;
						Status.chongshuiban_f = 1;
					}
				}
			}
		}
		//---����1�ӹس���
		if(Seat.count2 > 6000)		
		{
			Status.chuchou_f = 0;	
		}

		//----------����10���ӽ������ģʽ
		if(Seat.count2 > 60000)			//----10*60*1000/10		
		{
			if(Status.jieneng_f)
			{
				Status.jieneng_e = 1;
			}
			Seat.count2 = 60000;
		}
	}
}


