#include "temp_deal.h"
#include "NuMicro.h"
#include "adc.h"

ADC_Config adc_set;
uint_16 temp_save[5];    //保存的AD值，对应每个通道的

void delay_Us(uint_16 n)
{
	while(n>0)
		n--;
}


//************************************************
//函数名称：AD_fun_Init
//功能描述：初始化用户层接口函数
//输入参数：AD结构体、返回u16的AD读取函数，延时us函数
//返回参数：0
//************************************************/
void AD_fun_Init(ADC_Config *p, uint_16(*readADC)(uint_8),void (*delay_us)(uint_16))
{
	p->ReadADC = readADC;
	p->Delay_us = delay_us;	
}



//***********************************************
//函数名称：ReadADC
//功能描述：获取AD值，移植的时候更换这个AD值即可
//输入参数：通道口
//返回参数：该通道口的AD值
//************************************************/
/*
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
#else




	return 0; 
#endif
}
*/


void Temp_test(ADC_Config *p,uint_16 adval)
{
	

	printf("data  is %d\r\n",Temp_deal(p,adval));

}



//***********************************************
//函数名称：Temp_deal
//功能描述：AD值滤波，采集5次，去掉最大最小，求平均值
//输入参数：AD值
//返回参数：处理后的AD值
//************************************************/
uint_16 Temp_deal(ADC_Config *p,uint_16 adval)
{
	uint_8 AdcCount = 0;	
	uint_16 AdcBuf[5];
 	uint_16	AdcBufMax;
	uint_16	AdcBufMin;
	uint_16	AdcBufSum;
	uint_16	AdcBufVale;
	uint_8 i;
	uint_16 temp_dis = 0;

	AdcBuf[AdcCount] = p->ReadADC(adval);	//处理一个通道的AD值
	AdcCount++;
	if(AdcCount >= 5)  //取5次AD值
	{
		AdcCount = 0;
		AdcBufSum = 0;
		AdcBufMax = AdcBuf[0];
		AdcBufMin = AdcBuf[0];   //清0
		for(i=0; i<5; i++)		//遍历获取最大值和最小值，同时求和
		{
			if(AdcBufMax < AdcBuf[i])
			{
				AdcBufMax = AdcBuf[i];
			}
			if(AdcBufMin > AdcBuf[i])
			{
				AdcBufMin = AdcBuf[i];
			}
			AdcBufSum += AdcBuf[i];	    
		}
		AdcBufVale = (AdcBufSum - AdcBufMax - AdcBufMin) / 3;  //求平均  去掉最大去掉最小求平均
		return AdcBufVale;   //返回处理好的AD值		
	}
}



//**********************************
//
//  把AD值处理，批量处理AD，检测完并处理
//  不建议使用，这里的算法时间复杂度为O(n^2),执行时间比较长
//**********************************

void Get_All_AD(ADC_Config *p)
{
	static uint_8 AdcCount = 0; 
	static uint_16 AdcBuf[5];
	uint_16 AdcBufMax;
	uint_16 AdcBufMin;
	uint_16 AdcBufSum;
	uint_16 AdcBufVale;
	uint_8	i;
	uint_8 j,k;		
	for(j=0; j<4; j++)  //遍历所有的AD检测通道
	{
		for(k=0; k<6; k++)
		{
			AdcBuf[k] = p->ReadADC(AD_addr[j]); // 把对应的通道放进去
			if(k == 5)  //取5次AD值
			{
				AdcCount = 0;
				AdcBufSum = 0;
				AdcBufMax = AdcBuf[0];
				AdcBufMin = AdcBuf[0];	 //清0
				for(i=0; i<5; i++)		//遍历获取最大值和最小值，同时求和
				{
					if(AdcBufMax < AdcBuf[i])
					{
						AdcBufMax = AdcBuf[i];
					}
					if(AdcBufMin > AdcBuf[i])
					{
						AdcBufMin = AdcBuf[i];
					}
					AdcBufSum += AdcBuf[i]; 	
				}
				AdcBufVale = (AdcBufSum - AdcBufMax - AdcBufMin) / 3;  //求平均	去掉最大去掉最小求平均			
			}
		}
		temp_save[j] = AdcBufVale;   //保存AD值
	}  
}


//**********************************
//
//  座温处理函数 
//
//**********************************

void Seat_temp_deal(ADC_Config *p)
{
	uint_16 ADtemp;
	uint_8	JieNengTemp = 36;
	uint_8  Adj_temp = 0;
	//ADtemp = Temp_deal(p, 0);  // 单独输入一个AD通道进行AD采集 ，此处的AD通道为通道0
	
	printf("SEAT Temp is %d V\r\n",Temp_deal(p, 0));
/*	if((ADtemp < 30) || (ADtemp > 994))   		 // 短路或者断路
	{
		p->Err |= 0x80;   // 一个字节的 bit 7 bit6 为座温错误位  bit7 位硬件断路开路错误  bit6 为高温错误
	}
	else
	{
		p->Err &= 0x7f;	
		Seat.temperature = 0;
		while(ADtemp < Seat_temp_table[Seat.temperature])  //获取实际温度值
		{
			Seat.temperature++;
		}
		printf("Seat Temp is %d \r\n",Seat.temperature);
		
		if(Seat.temperature >= 55 &&   Seat.temperature <= 60)					//高温报警
		{
			p->Err |= 0x40;
		}
		else if(Seat.temperature > 60    &&   Seat.temperature <= 60)
		{
			;
		}
		else
		{
			p->Err &= 0xbf;
		}
	}
	if((p->Err != 0) && (MachineTest.Flag == 0))  //有错误报警
	{
		if(Status.Mode_select != 0)
		{
			Status.Mode_select = 0;
			work.step = 0;
			work.enable_f = 1;  //还需要再加其他标志位清0	
		}
		if(Status.penkou_f == 1)
		{
			Status.penkou_f = 0;
			penkou.step = 0;
		}
	//	WaterOut;
	}		
	
	if((Status.PcbTest_f == 0)&&(p->Err == 0))    //不在PCB测试下
	{
		if((Status.jieneng_e == 1)&&(Seat.DangWei != 0))
		{
			//------根据环境温度设置节能温度比较值

			while(Entironment.temperature > Seat_Envir_table[Adj_temp] )
			{
				Adj_temp++;
			}
			JieNengTemp -= (2 * Adj_temp);   //获取节能温度比较值
			
			if(Seat.temperature <= JieNengTemp)
			{
				//SeatHeat_set;							//--- turn on		
			}
			else if(Seat.temperature >= JieNengTemp+1)
			{
				//SeatHeat_clr;							//----turn off
			}
			Seat.Heat_f = 1;
		}
		else
		{
			if(Seat.temperature <= Seat_gear_table[Status.Icon.seat_gear])  //当前温度对比档位温度
			{
				//SeatHeat_set; 	
			}
			else if(Seat.temperature >= Seat_gear_table[Status.Icon.seat_gear] + 1)
			{
				//SeatHeat_clr; 	
			}
			Seat.Heat_f = 1;
		}
	}*/
}


//**********************************
//
// 		风温处理函数 
//
//**********************************

void Fan_Temp_Deal(ADC_Config *p)
{
	uint_16 ADtemp;
	uint_8 Envirt_index;
	//ADtemp = Temp_deal(p, 1);   // 单独输入一个AD通道进行AD采集 ，此处的AD通道为通道1

	printf("FAN Temp is %d V\r\n",Temp_deal(p, 1));
	
/*	if((ADtemp < 10) || (ADtemp>1000))
	{
		p->Err |= 0x20;   // 一个字节的 bit 5 bit4 为座温错误位  bit5 位硬件断路开路错误  bit4 为高温错误
	}
	else
	{
		p->Err &= 0xdf;	

		Drying.temperature = 0;
		
		while(ADtemp < Wind_temp_table[Drying.temperature])
		{
			Drying.temperature++;
		}

		printf("Fan Temp is %d \r\n",Drying.temperature);
		if(Drying.temperature >= 65)					//座温过高
		{
			p->Err |= 0x10;					//高温报警
		}
		else
		{
			p->Err &= 0xef;
		}		
	}	
	
	//-------环境温度设定，决定不同环境温度下的烘干功率
	if((Entironment.time==0)||(Status.PowerEnd_f==0))   //环境测温时间到，以及不在工作状态下
	{
 		Entironment.temperature = Drying.temperature;   //风温温度就是环境温度
 	}

	Entironment.Index = 0;
	while(Entironment.temperature > Environment_table[Entironment.Index])  //根据环境温度得出的索引值
	{
		Entironment.Index ++;
	}*/
}



//***********************
//
//	温度相关处理函数
//
//************************
void All_Temp_Deal(ADC_Config *p)
{
	Get_All_AD(p);
	Seat_temp_deal(p);
	Fan_Temp_Deal(p);

}







