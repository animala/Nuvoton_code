#ifndef _DEODORZATION_H
#define _DEODORZATION_H


#include "Toilet.h"
#include "NuMicro.h"


typedef struct{
	void (*Deo_set_high)(void);
	void (*Deo_set_low)(void);
}Deo_IO_cfg;

extern Deo_IO_cfg Deo_io;

void Deodorization_IO_init(Deo_IO_cfg *p,void (*deo_set_h)(),void (*deo_set_l)());


void Deodorization_Deal(void);
#endif 
