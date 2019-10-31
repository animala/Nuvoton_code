#ifndef _REMOTE_H
#define _REMOTE_H

#include "Toilet.h"
#include "NuMicro.h"


typedef struct{
	void (*Remote_set_high)(void);
	void (*Remote_set_low)(void);
}Remote_IO_cfg;

extern Remote_IO_cfg Remote_io;

void Remote_IO_init(Remote_IO_cfg *p,void (*Remote_set_h)(),void (*Remote_set_l)());



#endif