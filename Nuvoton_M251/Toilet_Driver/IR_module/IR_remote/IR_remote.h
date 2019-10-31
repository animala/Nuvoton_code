#ifndef _REMOTE_H
#define _REMOTE_H

#include "Toilet.h"

#include "NuMicro.h"

//----����һ������ö��
typedef enum {
 	Stop = 0x00,	//ֹͣ
 	Hip_clean,	 //��ϴ
 	Auto_s,		 //�Զ�
 	Wem_clean,	 //��ϴ
 	Dring,		 //ů����
 	Purge,       //ͨ��
	Massage,	 //��Ħ
	Turn_cover,  //����
	Deodorzation, //����
	Auto_clean,  //�Խ�
	Flush,		 //��ˮ
	Turn_circle, //��Ȧ
	Power,		 //��Դ
	Auto_flush,  //�Զ���ˮ
} Func;


typedef struct{
	void (*Remote_set_high)(void);
	void (*Remote_set_low)(void);
	uint_16 (*Check_pin)(void);
}Remote_IO_cfg;

extern Remote_IO_cfg Remote_io;


//----------��λ����ö��

typedef enum{

	Gear_0 = 0,
	Gear_1,
	Gear_2,
	Gear_3,
	Gear_4,
}Gear;

_User_ void Remote_Gpio_init(void);
_User_ uint_16 read_pin(void);

_Toilet_ void IR_receive(void);

_Toilet_ void IR_Opration(void);

_Link_ void Remote_IO_init(void (*Remote_set_h)(),void (*Remote_set_l)(),uint_16 (* check_pin)());



#endif
