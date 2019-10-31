#ifndef _BEEP_H
#define _BEEP_H

#include "Toilet.h"
#include "NuMicro.h"


typedef struct{

	void (*Beep_set_high)(void);
	void (*Beep_set_low)(void);
}Beep_IO_cfg;

extern Beep_IO_cfg Beep_io;

void buz_set(uint_8 loop,uint_8 delay_H,uint_8 delay_L);
void buz_task(void);

void beep_Io_init(Beep_IO_cfg *p,void (*Beep_set_h)(),void (*Beep_set_l)());


#endif