#ifndef _SIT_MODULE_H
#define _SIT_MODULE_H



#include "Toilet.h"
#include "NuMicro.h"



typedef struct{
//	void (*Sitcheck_set_high)(void);
//	void (*Sitcheck_set_low)(void);
	uint_16 (*Sitcheck_pin)(void);
}Sitcheck_IO_cfg;

extern Sitcheck_IO_cfg Sitcheck_io;

void SitCheck_IO_Init(Sitcheck_IO_cfg *p, uint_16 (*check_pin)(void));
void Sitdown_Check(void);






#endif