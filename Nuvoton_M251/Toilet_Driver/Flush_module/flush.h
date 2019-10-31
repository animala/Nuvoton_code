#ifndef _FLUSH_H_
#define _FLUSH_H_

#include "Toilet.h"
#include "NuMicro.h"


typedef struct{
	void (*Deo_set_high)(void);
	void (*Deo_set_low)(void);
}Flush_IO_cfg;

extern Flush_IO_cfg Flush_io;

void Flush_IO_init(Flush_IO_cfg *p,void (*flush_set_h)(),void (*flush_set_l)());

void FlushWater_Board_Deal(void);
void Flush_Deal(void);


#endif