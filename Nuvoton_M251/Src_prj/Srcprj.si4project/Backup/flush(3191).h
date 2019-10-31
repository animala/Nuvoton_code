#ifndef _FLUSH_H_
#define _FLUSH_H_

#include "Toilet.h"
#include "NuMicro.h"



//=========================================================================================


typedef struct{
	void (*Jet_set_Value)(uint_8 val);     //����ڳ�ˮ����
	void (*Wash_set_Value)(uint_8 val);    //��ϴ��ˮ����
	void (*Washboard_set_Value)(uint_8 val);  //��ˮ������
}Flush_IO_cfg;

extern Flush_IO_cfg Flush_io;

_User_ void Flush_Gpio_init(void);

_User_ void Flush_control_init(Flush_IO_cfg *p);

_Link_ void Flush_IO_init(Flush_IO_cfg *p,void (*jet_set)(uint_8 val),void (*wash_set)(uint_8 val));

_Link_ void Flush_board_init(Flush_IO_cfg *p,void (*jet_set)(uint_8 val));

_Toilet_  void FlushWater_Board_Deal(void);


#endif