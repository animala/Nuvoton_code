#include "stdio.h"
#include "NuMicro.h"
#include "tos.h"  
#include "adc.h"
#include "toilet.h"
#include "control_center.h"
#include "DWT_Delay.h"

buzzer_uion buzzer;
timer_uion timer; 
Status_uion Status;
Water_uion Water;
Seat_uion Seat;
Drying_uion Drying;
TunBu_uion TunBu;
NvXing_uion NvXing;
SoftPowerDeal_CB SoftPowerDeal;
Entironment_CB Entironment;
Enfant_uion Enfant;	
MachineTest_CB MachineTest;
chongshui_uion chongshui;
LiquidDeal_CB LiquidDeal;
work_uion work;
CoverSeat_uion CoverSeat;
Error_uion Error;
IR_uion IR;
fangwu_uion fangwu;
Rolling_CB Rolling;
KEY_uion KEY;
PenGan_uion PenGan;
penkou_uion penkou;
FGIR_CB FGIR;
SendData_CB SendData;

#define  Tos

#ifdef Tos
#define task1_size   240  // size of task1 stack
#define task2_size 	 240	
k_task_t task1;   				//structure of task1
k_task_t task2;      	   

k_stack_t task1_stack[task1_size];   //stack of task1
k_stack_t task2_stack[task2_size];   

#endif

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
		/* Unlock protected registers */
    SYS_UnlockReg();
	
		/* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode to prevent leakage  */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Disable digital input path of analog pin XT1_OUT to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PF, BIT2 | BIT3);
   
	  /* Enable， HXT  clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);		
	
    /* Switch HCLK clock source to external  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK(1));  //here switch zhe MCU clock to external
	  
	  /*Set the core clock freq*/
		CLK_SetCoreClock(FREQ_32MHZ);   //set the core clock freq,should not higher than your HCLK clock freq
			
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
		
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                     */
    /*---------------------------------------------------------------------------------------------------------*/
    Uart0DefaultMPF();

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                   */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART module */
    SYS_ResetModule(UART0_RST);
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

#ifdef Tos
//task1 function
void task1_fun(void *Parameter)
{

	while(1)
	{
			//Seat_temp_deal(&adc_set);
			

			//Fan_Temp_Deal(&adc_set);
			printf("Task1 ：Seat temp is %d\r\n",EADC_FunctionTest(0));	
			//printf("Task1 ：Thansplant CORTEX-M23 Succeed\r\n");	
			PB14 =~ PB14;  
			tos_task_delay(1000); 
	}

}

//task2 function 
void task2_task(void *Parameter)
{

		k_err_t err;
		while(1)
		{	
			//EADC_FunctionTest(1);
			//Fan_Temp_Deal(&adc_set);
			printf("Task2 ：Fan temp is %d\r\n",EADC_FunctionTest(1));
			//printf("Task2: hello world, hello Nuvoton \r\n");		
			tos_task_delay(2000);   
		}
}

#endif





int main()
{
		k_err_t err;
	 /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

	DWT_INIT(MY_MCU_SYSCLK);
	ADC_Init();    //ADC硬件驱动  

#ifdef Tos
	tos_knl_init();   //tos init ,include SystickInit,should init after protected registers unlock
#endif
	Toilet_driver_init();  //马桶底层硬件驱动初始化，主要是用户层和底层连接初始化
    /* Lock protected registers */
    SYS_LockReg();
		
    /* Init UART0 for printf */
    UART0_Init();

	printf("system clock to %d Hz, PLL clock to %d Hz...................... ", SystemCoreClock, CLK_SetCoreClock(FREQ_32MHZ));
  	
#ifdef Tos

		err = tos_task_create(
				&task1,
				"Task1",
				task1_fun,
				NULL,
				2,
				task1_stack,
				task1_size,
				100
			);
	  
		err = tos_task_create(
				&task2,						
				"display",				
				task2_task,				
				NULL,							 
				2,              	
				task2_stack,		   	
				task2_size,			
				100);							
		
		if(err != K_ERR_NONE)
		printf("TenentOS creat task fail! code is %d\r\n",err);
	
		tos_knl_start(); //Start TOS TINY
#endif



/*
	while(1)
	{

		Seat_temp_deal(&adc_set);

		//Fan_Temp_Deal(&adc_set);
		
		PB14 =~ PB14;  
		

		
	//	DWT_DELAY_uS(50000);
	}

*/

}
