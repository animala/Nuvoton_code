#include "sit_check.h"


Sitcheck_IO_cfg Sitcheck_io;


//
//   抽象层IO移植初始化，只需传入对应单片机的IO口 输入函数即可
// 
void SitCheck_IO_Init(Sitcheck_IO_cfg *p, uint_16 (*check_pin)(void))
{

	p->Sitcheck_pin = check_pin;
	
}



//********************************************/
//
//入座检测~
//
//******************************************/
void Sitdown_Check(void)
{
	if(Sitcheck_io.Sitcheck_pin == 0)  //检测到入座
	{
		Seat.count1++;
		Seat.count2 = 0;
		if(Seat.count1 > 10)
		{
			//------------100MS着座有效
			Entironment.time = T30Min;     //获取环境温度的时间
			CoverSeat.delay = T5Min;      // 入座后一直给翻盖关闭计时为5分钟
			CoverSeat.enable_f = 1;
			Status.chuchou_f = 1;		//-----着座除臭使能
			Status.jieneng_e = 0;		//关闭节能
			Seat.Flag |= 0x01;
			if(Status.seat_f == 0)
			{
				Status.seat_f = 1;
				Status.power_f = 1;		//入座标志位置1
				//SpeedinessHeatPowerOn;
				//WifiPowerOn;
				//CO2_clr_seat;
				Status.fangwu_f = 1;	//防污标志位置1
				fangwu.step = 0;		
				penkou.step = 0;		//如果正在清洗喷头，立即停止收回
				Status.penkou_f = 0;		
				buz_set(1,30,30);	
			}
			if(Seat.count1 > 1000)		//---------入座10S自动冲水开启
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
		//------------离座2S有效
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
				if(Status.dring_f == 1)   //正在烘干的情况下离座
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
				if(penkou.enable_f == 1)  //正在清洗喷口的情况下离座
				{
					penkou.step = 0;
					Status.penkou_f = 0;
				}					
			}
		}
		//----------离座6S自动冲水
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
		//---离座1钟关除臭
		if(Seat.count2 > 6000)		
		{
			Status.chuchou_f = 0;	
		}

		//----------离座10分钟进入节能模式
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


