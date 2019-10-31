#include "control_center.h"
#include "toilet.h"
#include "temp_deal.h"
#include "DWT_Delay.h"
#include "adc.h"
#include "Pwm_cfg.h"
#include "beep.h"
#include "air_pumb.h"
#include "Moto_deal.h"
//
//	时间处理模块
//
void Timing_deal(void)
{
	timer.count_10ms++;
	if(IR.TFDelay>0)
	{
		IR.TFDelay--;
	}	
	
	if(SendData.TF>0)
	{
		SendData.TF--;
	}
	if(CoverSeat.SpaceTime>0)
	{
		CoverSeat.SpaceTime--;
	}
	if((timer.count_10ms%10)==0)
	{
		timer.t_100ms_f=1;				//	100 ms 
	}
	if((timer.count_10ms%50)==0)
	{
		timer.t_500ms_f=1;				//	500 ms 
		timer.count_flash++;
		if(timer.count_flash > 9)
		{
			timer.count_flash = 0;
		}
	}
	if(timer.count_10ms>=100)
	{
		timer.t_1s_f = 1;
		timer.t_power_1s = 1;
		timer.t_receive_1s = 1;
		timer.count_10ms=0; 			//	1s

		
		if(SoftPowerDeal.Delay>0)
		{
			SoftPowerDeal.Delay--;
		}
		if(Drying.HotStartTime > 0)
		{
			Drying.HotStartTime--;
		}

		if(Drying.HotEndTime > 0)
		{
			Drying.HotEndTime--;
		}
		if(LiquidDeal.SetOff1Min>0)
		{
			LiquidDeal.SetOff1Min--;
		}
	/*	if(LightInduce.Timer>0)
		{
			LightInduce.Timer--;
		}*/
		if(Entironment.time>0)
		{
			Entironment.time--;
		}
		if(Seat.SeatOnTimer>0)
		{
			Seat.SeatOnTimer--;
		}
		if(CoverSeat.delay>0)
		{
			CoverSeat.delay--;
		}
		if(work.Start_f)
		{
			if(work.time>0)
			{
				work.time--;
			}
			else
			{
				work.Start_f=0;
			}
		}
	}

}



uint_16 ReadADC(uint_8 ADCchannel)
{
#ifdef EF_50D       //EF50D 的AD读取函数
	signed int ADC;
	ANCTR1 = ADCchannel;					//select channel
	delay_us(5);							//delay
	ANCTR2 = 0x80;							//START
	while((ANCTR2&0x80) == 0x80);				
	ADC = (uint_16)ANBUF1 << 2;
	ADC = ADC + (ANBUF0 >> 6);
	return ADC; 
#endif
}


void Toilet_driver_init(void)
{
#ifdef EF_50D
	AD_fun_Init(&adc_set,ReadADC,delay_Us);   //初始化ADC
#else
	AD_fun_Init(&adc_set,EADC_READ,DWT_DELAY_uS);   //初始化ADC TOS操作系统
#endif
	




}





