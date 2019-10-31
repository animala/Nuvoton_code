#include "control_center.h"
#include "toilet.h"


void delay_us(uint_16 n)
{
	while(n>0)
		n--;
}


uint_16 ReadADC(uint_8 ADCchannel)
{
#ifdef EF_50D       //EF50D µÄAD¶ÁÈ¡º¯Êý
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


void Toilet_driver_init()
{
	AD_fun_Init(&adc_set,ReadADC(1),delay_us(50));


}





