#ifndef _BEEP_H
#define _BEEP_H

#include "Toilet.h"
#include "NuMicro.h"


typedef struct{

	void (*Beep_set_high)(void);
	void (*Beep_set_low)(void);
}Beep_IO_cfg;

extern Beep_IO_cfg Beep_io;


_User_ void Beep_Gpio_init(void);


_Toilet_ void buz_set(uint_8 loop,uint_8 delay_H,uint_8 delay_L);
_Toilet_ void buz_task(void);

_Link_ void beep_Io_init(void (*Beep_set_h)(),void (*Beep_set_l)());

#endif