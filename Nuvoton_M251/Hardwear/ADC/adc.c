#include "adc.h"
#include "NuMicro.h"
#include "stdint.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

volatile uint32_t g_u32AdcIntFlag;


//********************************************
//	ADC ��ʼ��
//********************************************

void ADC_Init(void)
{
	CLK_EnableModuleClock(EADC_MODULE);  //ʹ��ADCʱ��

	/* EADC clock source is 48MHz, set divider to 8, EADC clock is 48/8 MHz */
    CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV0_EADC(8));

	/* Set PB.0 and PB.1 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk);   //���ｨ��ʹ�üĴ�������IO��ʼ�������ʹ�ÿ⺯��������ִ��Ч�ʲ��ߵ����

	//PB->MODE = (PB->MODE &~(GPIO_MODE_MODE0_Msk) |(0x0UL << GPIO_MODE_MODE0_Pos));  //����ģʽ

	PB->MODE = (PB->MODE &~(GPIO_MODE_MODE14_Msk) |(0x1UL << GPIO_MODE_MODE14_Pos));  //pin14 ���ģʽ

    /* Configure the EADC analog input pins.  */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB0MFP_Msk) | SYS_GPB_MFPL_PB0MFP_EADC0_CH0;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB1MFP_Msk) | SYS_GPB_MFPL_PB1MFP_EADC0_CH1;

    /* Disable the digital input path to avoid the leakage current for EADC analog input pins. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT0 | BIT1);  /* Disable PB0 and PB1 */


	
}


/*---------------------------------------------------------------------------------------------------------*/
/* EADC function test                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
uint_16 EADC_READ(uint_8 u32ChannelNum)
{
    uint_16 i32ConversionData;
	
	uint_16 u32ModuleNum;
	uint_16 u32ModuleMask;
	
	u32ModuleNum = u32ChannelNum;	/* Use Sample Module 1 */
	u32ModuleMask = (BIT0 << u32ModuleNum);

	/* Set input mode as single-end and enable the A/D converter */
    EADC_Open(EADC, 0);

	/* Configure the sample module for analog input channel and software trigger source. */
	EADC_ConfigSampleModule(EADC, u32ModuleNum, EADC_SOFTWARE_TRIGGER, u32ChannelNum);

	/* Set sample module external sampling time to 10 */
    EADC_SetExtendSampleTime(EADC, u32ModuleNum, 10);

    /* Clear the A/D ADINT0 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);

 
    EADC_START_CONV(EADC, u32ModuleMask);  //��ʼת��

    /* Wait EADC conversion done */

	while(EADC_GET_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk) != 0);

    /* Get the conversion result of the sample module 17 */
    i32ConversionData = EADC_GET_CONV_DATA(EADC, u32ModuleNum);

  	EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);      /* Clear the A/D ADINT0 interrupt flag */
    
	return i32ConversionData;

	EADC_Close(EADC);
}




