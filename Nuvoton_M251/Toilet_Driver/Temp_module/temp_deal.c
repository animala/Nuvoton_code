#include "temp_deal.h"
#include "NuMicro.h"
#include "adc.h"

ADC_Config adc_set;
uint_16 temp_save[5];    //�����ADֵ����Ӧÿ��ͨ����


void delay_Us(uint_16 n)
{
	while(n>0)
		n--;
}


//************************************************
//�������ƣ�AD_fun_Init
//������������ʼ���û���ӿں���
//���������AD�ṹ�塢����u16��AD��ȡ��������ʱus����
//���ز�����0
//************************************************/
void AD_fun_Init(ADC_Config *p, uint_16(*readADC)(uint_8),void (*delay_us)(uint_16))
{
	p->ReadADC = readADC;
	p->Delay_us = delay_us;	
}




//**********************************
//
//  ��ADֵ������������AD������겢����
//  ������ʹ�ã�������㷨ʱ�临�Ӷ�ΪO(n^2),ִ��ʱ��Ƚϳ�
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
	for(j=0; j<4; j++)  //�������е�AD���ͨ��
	{
		for(k=0; k<6; k++)
		{
			AdcBuf[k] = p->ReadADC(AD_addr[j]); // �Ѷ�Ӧ��ͨ���Ž�ȥ
			if(k == 5)  //ȡ5��ADֵ
			{
				AdcCount = 0;
				AdcBufSum = 0;
				AdcBufMax = AdcBuf[0];
				AdcBufMin = AdcBuf[0];	 //��0
				for(i=0; i<5; i++)		//������ȡ���ֵ����Сֵ��ͬʱ���
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
				AdcBufVale = (AdcBufSum - AdcBufMax - AdcBufMin) / 3;  //��ƽ��	ȥ�����ȥ����С��ƽ��			
			}
		}
		temp_save[j] = AdcBufVale;   //����ADֵ
	}  
}


//**********************************
//
//  ���´����� 
//
//**********************************

void Seat_temp_deal(ADC_Config *p)
{
	uint_8	JieNengTemp = 36;
	uint_8  Adj_temp = 0;
	
	static uint_8 AdcCount = 0;	
	static uint_16 AdcBuf[5];
 	uint_16	AdcBufMax;
	uint_16	AdcBufMin;
	uint_16	AdcBufSum;
	uint_16	AdcBufVale;
	uint_8 i;

	AdcBuf[AdcCount] = p->ReadADC(0);	//����һ��ͨ����ADֵ
	AdcCount++;
	if(AdcCount >= 5)  //ȡ5��ADֵ
	{
		AdcCount = 0;
		AdcBufSum = 0;
		AdcBufMax = AdcBuf[0];
		AdcBufMin = AdcBuf[0];   //��0
		for(i=0; i<5; i++)		//������ȡ���ֵ����Сֵ��ͬʱ���
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
		AdcBufVale = (AdcBufSum - AdcBufMax - AdcBufMin) / 3;  //��ƽ��  ȥ�����ȥ����С��ƽ��	
	}
	//printf("Seat temp is %d\r\n",AdcBufVale);
/*	if((ADtemp < 30) || (ADtemp > 994))   		 // ��·���߶�·
	{
		p->Err |= 0x80;   // һ���ֽڵ� bit 7 bit6 Ϊ���´���λ  bit7 λӲ����·��·����  bit6 Ϊ���´���
	}
	else
	{
		p->Err &= 0x7f;	
		Seat.temperature = 0;
		while(ADtemp < Seat_temp_table[Seat.temperature])  //��ȡʵ���¶�ֵ
		{
			Seat.temperature++;
		}
		printf("Seat Temp is %d \r\n",Seat.temperature);
		
		if(Seat.temperature >= 55 &&   Seat.temperature <= 60)					//���±���
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
	if((p->Err != 0) && (MachineTest.Flag == 0))  //�д��󱨾�
	{
		if(Status.Mode_select != 0)
		{
			Status.Mode_select = 0;
			work.step = 0;
			work.enable_f = 1;  //����Ҫ�ټ�������־λ��0	
		}
		if(Status.penkou_f == 1)
		{
			Status.penkou_f = 0;
			penkou.step = 0;
		}
	//	WaterOut;
	}		
	
	if((Status.PcbTest_f == 0)&&(p->Err == 0))    //����PCB������
	{
		if((Status.jieneng_e == 1)&&(Seat.DangWei != 0))
		{
			//------���ݻ����¶����ý����¶ȱȽ�ֵ

			while(Entironment.temperature > Seat_Envir_table[Adj_temp] )
			{
				Adj_temp++;
			}
			JieNengTemp -= (2 * Adj_temp);   //��ȡ�����¶ȱȽ�ֵ
			
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
			if(Seat.temperature <= Seat_gear_table[Status.Icon.seat_gear])  //��ǰ�¶ȶԱȵ�λ�¶�
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
// 		���´����� 
//
//**********************************

void Fan_Temp_Deal(ADC_Config *p)
{
	uint_8 Envirt_index;
	static uint_8 AdcCount = 0;	
	static uint_16 AdcBuf[5];
 	uint_16	AdcBufMax;
	uint_16	AdcBufMin;
	uint_16	AdcBufSum;
	uint_16	AdcBufVale;
	uint_8 i;

	AdcBuf[AdcCount] = p->ReadADC(1);	//����һ��ͨ����ADֵ
	AdcCount++;
	if(AdcCount >= 5)  //ȡ5��ADֵ
	{
		AdcCount = 0;
		AdcBufSum = 0;
		AdcBufMax = AdcBuf[0];
		AdcBufMin = AdcBuf[0];   //��0
		for(i=0; i<5; i++)		//������ȡ���ֵ����Сֵ��ͬʱ���
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
		
		AdcBufVale = (AdcBufSum - AdcBufMax - AdcBufMin) / 3;  //��ƽ��  ȥ�����ȥ����С��ƽ��	
	}

	if(i > 4)
	printf("Fan temp is %d\r\n",AdcBufVale);
	
/*	if((ADtemp < 10) || (ADtemp>1000))
	{
		p->Err |= 0x20;   // һ���ֽڵ� bit 5 bit4 Ϊ���´���λ  bit5 λӲ����·��·����  bit4 Ϊ���´���
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
		if(Drying.temperature >= 65)					//���¹���
		{
			p->Err |= 0x10;					//���±���
		}
		else
		{
			p->Err &= 0xef;
		}		
	}	
	
	//-------�����¶��趨��������ͬ�����¶��µĺ�ɹ���
	if((Entironment.time==0)||(Status.PowerEnd_f==0))   //��������ʱ�䵽���Լ����ڹ���״̬��
	{
 		Entironment.temperature = Drying.temperature;   //�����¶Ⱦ��ǻ����¶�
 	}

	Entironment.Index = 0;
	while(Entironment.temperature > Environment_table[Entironment.Index])  //���ݻ����¶ȵó�������ֵ
	{
		Entironment.Index ++;
	}*/
}



//***********************
//
//	�¶���ش�����
//
//************************
void All_Temp_Deal(ADC_Config *p)
{
	Get_All_AD(p);
	Seat_temp_deal(p);
	Fan_Temp_Deal(p);

}







