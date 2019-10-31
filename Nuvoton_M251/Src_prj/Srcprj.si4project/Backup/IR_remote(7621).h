#ifndef _REMOTE_H
#define _REMOTE_H

#include "Toilet.h"
#include "NuMicro.h"

//----定义一个功能枚举
typedef enum {
 	Stop = 0x00,	//停止
 	Hip_clean,	 //臀洗
 	Auto_s,		 //自动
 	Wem_clean,	 //妇洗
 	Dring,		 //暖风烘干
 	Purge,       //通便
	Massage,	 //按摩
	Turn_cover,  //翻盖
	Deodorzation, //除臭
	Auto_clean,  //自洁
	Flush,		 //冲水
	Turn_circle, //翻圈
	Power,		 //电源
	Auto_flush,  //自动冲水
} Func;


typedef struct{
	void (*Remote_set_high)(void);
	void (*Remote_set_low)(void);
	uint_16 (*Check_pin)(void);
}Remote_IO_cfg;

extern Remote_IO_cfg Remote_io;


//----------档位调节枚举

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
