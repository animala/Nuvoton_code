#include "timer_deal.h"


void Timer0_Init(void)
{
	CLK_EnableModuleClock(TMR0_MODULE);
	
	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);

	SystemCoreClockUpdate();

	// Set timer frequency to 1HZ
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 10);  //10赫兹，100ms
	// Enable timer interrupt
	TIMER_EnableInt(TIMER0);
	
	NVIC_EnableIRQ(TMR0_IRQn);

		// Start Timer 0
	TIMER_Start(TIMER0);
}


void Timer1_Init(void)
{
	CLK_EnableModuleClock(TMR1_MODULE);
	
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0);

	SystemCoreClockUpdate();

	// Set timer frequency to 1HZ
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);  //10赫兹，100ms
	// Enable timer interrupt
	TIMER_EnableInt(TIMER1);
	
	NVIC_EnableIRQ(TMR1_IRQn);

		// Start Timer 1
	TIMER_Start(TIMER1);
}



//定时器0中断服务函数
void TMR0_IRQHandler(void)
{
    static uint_16 s_u32Sec = 1;
    printf("TIMER0 %d sec\r\n", s_u32Sec++);

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);
}


//定时器1中断服务函数
void TMR1_IRQHandler(void)
{
    static uint_16 s_u32Sec = 1;
    printf("TIMER1 %d sec\r\n", s_u32Sec++);

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER1);
}






