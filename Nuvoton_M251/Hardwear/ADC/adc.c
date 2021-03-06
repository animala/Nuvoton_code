#include "adc.h"
#include "NuMicro.h"
#include "stdint.h"

//********************************************
//	ADC 初始化
// 初始化AD接口 分别为 座温 环境温度 光照 电机状态
// 对应 PB0 PB1 PB2 PB3
//********************************************

void ADC_Init(void)
{
	CLK_EnableModuleClock(EADC_MODULE);  //使能ADC时钟

	/* EADC clock source is 48MHz, set divider to 8, EADC clock is 48/8 MHz */
    CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV0_EADC(8));  //时钟设置

	/* Set PB.0 PB.1  PB.2 PB.3 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk | GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);   //这里建议使用寄存器进行IO初始化

	//PB->MODE = (PB->MODE &~(GPIO_MODE_MODE0_Msk) |(0x0UL << GPIO_MODE_MODE0_Pos));  //输入模式	

    /* Configure the EADC analog input pins.  */  //复用为ADC通道
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB0MFP_Msk) | SYS_GPB_MFPL_PB0MFP_EADC0_CH0;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB1MFP_Msk) | SYS_GPB_MFPL_PB1MFP_EADC0_CH1;	
	SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk) | SYS_GPB_MFPL_PB2MFP_EADC0_CH2;
	SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB3MFP_Msk) | SYS_GPB_MFPL_PB3MFP_EADC0_CH3;
    /* Disable the digital input path to avoid the leakage current for EADC analog input pins. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT0 | BIT1 | BIT2 | BIT3);  /* Disable PB0 and PB1 PB2 PB3 */


	
}


/*---------------------------------------------------------------------------------------------------------*/
/*   ADC通道读取ADC的值    
/*   输入通道号，返回该通道的AD值																	     */                                                                      										
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

 
    EADC_START_CONV(EADC, u32ModuleMask);  //开始转换

    /* Wait EADC conversion done */

	while(EADC_GET_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk) != 0);

    /* Get the conversion result of the sample module 17 */
    i32ConversionData = EADC_GET_CONV_DATA(EADC, u32ModuleNum);

  	EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);      /* Clear the A/D ADINT0 interrupt flag */
    
	return i32ConversionData;

	EADC_Close(EADC);  //关闭ADC
}




