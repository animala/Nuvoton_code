#ifndef __ADC_H
#define __ADC_H

#include "NuMicro.h"
#include "toilet.h"

void ADC_Init(void);

uint_16 EADC_READ(uint_8 u32ChannelNum);
//void EADC_FunctionTest(uint32_t u32ChannelNum);


#endif