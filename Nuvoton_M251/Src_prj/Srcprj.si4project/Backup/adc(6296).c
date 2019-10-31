#include "adc.h"
#include "NuMicro.h"
#include "stdint.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

volatile uint32_t g_u32AdcIntFlag;


//********************************************
//	ADC 初始化
//********************************************

void ADC_Init(void)
{
	CLK_EnableModuleClock(EADC_MODULE);  //使能ADC时钟

	/* EADC clock source is 48MHz, set divider to 8, EADC clock is 48/8 MHz */
    CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV0_EADC(8));

	/* Set PB.0 and PB.1 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk);   //这里建议使用寄存器进行IO初始化，如果使用库函数，存在执行效率不高的情况

	//PB->MODE = (PB->MODE &~(GPIO_MODE_MODE0_Msk) |(0x0UL << GPIO_MODE_MODE0_Pos));  //输入模式

	PB->MODE = (PB->MODE &~(GPIO_MODE_MODE14_Msk) |(0x1UL << GPIO_MODE_MODE14_Pos));  //pin14 输出模式

    /* Configure the EADC analog input pins.  */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB0MFP_Msk) | SYS_GPB_MFPL_PB0MFP_EADC0_CH0;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB1MFP_Msk) | SYS_GPB_MFPL_PB1MFP_EADC0_CH1;

    /* Disable the digital input path to avoid the leakage current for EADC analog input pins. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT0 | BIT1);  /* Disable PB0 and PB1 */


	
}


/*---------------------------------------------------------------------------------------------------------*/
/* EADC function test                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t EADC_FunctionTest(uint32_t u32ChannelNum)
{
    uint32_t i32ConversionData;
	
	uint32_t u32ModuleNum;
	uint32_t u32ModuleMask;
	
	u32ModuleNum = u32ChannelNum + 1;	/* Use Sample Module 1 */
	u32ModuleMask = (BIT0 << u32ModuleNum);

	/* Set input mode as single-end and enable the A/D converter */
    EADC_Open(EADC, 0);

	/* Configure the sample module for analog input channel and software trigger source. */
	EADC_ConfigSampleModule(EADC, u32ModuleNum, EADC_SOFTWARE_TRIGGER, u32ChannelNum);

	/* Set sample module external sampling time to 10 */
    EADC_SetExtendSampleTime(EADC, u32ModuleNum, 10);

    /* Clear the A/D ADINT0 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);

    /* Enable the sample module 0  interrupt.  */
    EADC_ENABLE_INT(EADC, BIT0);//Enable sample module A/D ADINT0 interrupt.

	EADC_ENABLE_SAMPLE_MODULE_INT(EADC, 0, u32ModuleMask);

    NVIC_EnableIRQ(EADC_INT0_IRQn);

    /* Reset the ADC interrupt indicator and trigger sample module 17 to start A/D conversion */
    g_u32AdcIntFlag = 0;
	
    EADC_START_CONV(EADC, u32ModuleMask);  //开始转换

    /* Wait EADC conversion done */
    while(g_u32AdcIntFlag == 0);

    /* Get the conversion result of the sample module 17 */
    i32ConversionData = EADC_GET_CONV_DATA(EADC, 1);
  //  printf("Conversion result of temperature sensor: 0x%X (%d)\n\n", i32ConversionData, i32ConversionData);
	
	 /* Disable the ADINT0 interrupt */
    EADC_DISABLE_INT(EADC, BIT0);
    EADC_DISABLE_SAMPLE_MODULE_INT(EADC, 0, 1);
    NVIC_DisableIRQ(EADC_INT0_IRQn);    //关闭中断

	EADC_Close(EADC);   //关闭ADC
	return i32ConversionData;
}

/*---------------------------------------------------------------------------------------------------------*/
/* EADC interrupt handler    ADC中断处理函数                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void EADC_INT0_IRQHandler(void)
{
    g_u32AdcIntFlag = 1;    //中断处理标志位置1
    EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);      /* Clear the A/D ADINT0 interrupt flag */
}




