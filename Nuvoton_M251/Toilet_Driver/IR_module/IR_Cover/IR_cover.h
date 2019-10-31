#ifndef _COVER_H
#define _COVER_H


#include "Toilet.h"
#include "NuMicro.h"


#define	FG_DataSize			5
#define	User_FG_DataSize	5
#define SendDataBuffSize    5

extern unsigned char   Send_Data_buff[SendDataBuffSize];


typedef struct{
	void (*Cover_set_high)(void);
	void (*Cover_set_low)(void);
	uint_16 (*Check_pin)(void);
}Cover_IO_cfg;

extern Cover_IO_cfg Cover_io;

void Cover_IO_init(Cover_IO_cfg *p,void (*Cover_set_h)(),void (*Cover_set_l)(),uint_16 (* check_pin)());



void Cover_receive(void);

void Cover_Opreation(void);

void Cover_SendData(void);

void Cover_send_datadeal(void);


#endif
