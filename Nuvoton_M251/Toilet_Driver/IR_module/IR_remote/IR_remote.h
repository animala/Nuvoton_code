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


void IR_receive(void);

void IR_Opration(void);

void Remote_IO_init(Remote_IO_cfg *p,void (*Remote_set_h)(),void (*Remote_set_l)(),uint_16 (* check_pin)());



#endif
