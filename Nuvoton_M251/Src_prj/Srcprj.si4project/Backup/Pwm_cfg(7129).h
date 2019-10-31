#ifndef _PWM_H
#define _PWM_H
#include "NuMicro.h"
#include "Toilet.h"

void Pwm_GPIO_Init(void);


void pwm_config_init(void);


void Set_Pwm_value(uint_16 value);

uint_16 CalNewDutyCMR(PWM_T *pwm, uint_16 u32ChannelNum, uint_16 u32DutyCycle, uint_16 u32CycleResolution);


#endif

