#include "Pwm_cfg.h"

const uint_8 pwm_table[5] = {0, 25, 50, 75, 100};


void Pwm_GPIO_Init(void)
{

	/* Enable PWM0 module clock */
    CLK_EnableModuleClock(PWM0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* PWM clock frequency configuration                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* PWM clock frequency can be set equal to HCLK by choosing case 1 */
    /* case 1.PWM clock frequency is set equal to PCLK */
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0);
    /*---------------------------------------------------------------------------------------------------------*/
	/* Reset PWM0 module */
	SYS_ResetModule(PWM0_RST);
    /* Set PB multi-function pins for PWM0 Channel 0 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB5MFP_Msk)) | SYS_GPB_MFPL_PB5MFP_PWM0_CH0;

}

void pwm_config_init(void)
{
	/*
	Configure PWM0 channel 0 init period and duty(up counter type).
	Period is PLL / (prescaler * (CNR + 1))
	Duty ratio = (CMR) / (CNR + 1)
	Period = 48 MHz / (1 * (38399 + 1)) = 1250 Hz
	Duty ratio = (19200) / (38399 + 1) = 50%
	*/

	/* PWM0 channel 0 frequency is 1250Hz, duty 50%, */
	PWM_ConfigOutputChannel(PWM0, 0, 1250, 50);

	/* Enable output of PWM0 channel 0 */
	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);

	/* Start PWM counter */
	PWM_Start(PWM0, PWM_CH_0_MASK);

}


void Set_Pwm_value(uint_8 switch        , uint_16 value)
{
	static uint_16 u32NewCMR = 0;
	if(switch == 1)
	{
		/* Get new comparator value by call CalNewDutyCMR() */
		u32NewCMR = CalNewDutyCMR(PWM0, 0, pwm_table[value], 100);
		/* Set new comparator value to register */
		PWM_SET_CMR(PWM0, 0, u32NewCMR);	
	}

	else
	{
		 /* Stop PWM counter */
	    PWM_Stop(PWM0, PWM_CH_0_MASK);
	    /* Disable output of PWM0 channel 0 */
	    PWM_DisableOutput(PWM0, PWM_CH_0_MASK);
	}

}



/**
 * @brief       Calculate the comparator value of new duty by configured period
 *
 * @param       pwm                  The pointer of the specified PWM module
 *
 * @param       u32ChannelNum        PWM channel number. Valid values are between 0~5
 *
 * @param       u32DutyCycle         Target generator duty cycle percentage. Valid range are between 0 ~ u32CycleResolution.
 *                                   If u32CycleResolution is 100, and u32DutyCycle is 10 means 10%, 20 means 20% ...
 *
 * @param       u32CycleResolution   Target generator duty cycle resolution. The value in general is 100.
 *
 * @return      The compatator value by new duty cycle
 */
uint_16 CalNewDutyCMR(PWM_T *pwm, uint_16 u32ChannelNum, uint_16 u32DutyCycle, uint_16 u32CycleResolution)
{
    if (u32DutyCycle >= u32CycleResolution)
        return PWM_GET_CNR(pwm, u32ChannelNum);

    return (u32DutyCycle * (PWM_GET_CNR(pwm, u32ChannelNum) + 1) / u32CycleResolution);
}


