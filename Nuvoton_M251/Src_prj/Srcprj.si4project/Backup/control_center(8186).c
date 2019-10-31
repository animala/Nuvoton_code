#include "control_center.h"
#include "toilet.h"
#include "DWT_Delay.h"
#include "adc.h"



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





