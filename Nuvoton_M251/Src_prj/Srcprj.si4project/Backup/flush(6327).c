#include "flush.h"


Flush_IO_cfg Flush_io;

//============================================用户层==============================================================

//初始化对应平台的IO口
_User_ void Flush_Gpio_init(void)
{
	PB->MODE = (PB->MODE &~(GPIO_MODE_MODE10_Msk) |(0x1UL << GPIO_MODE_MODE10_Pos));  //pin10 输出模式 M1A
	PB->MODE = (PB->MODE &~(GPIO_MODE_MODE9_Msk) |(0x1UL << GPIO_MODE_MODE9_Pos));    //pin9 输出模式  M1B
	PB->MODE = (PB->MODE &~(GPIO_MODE_MODE8_Msk) |(0x1UL << GPIO_MODE_MODE8_Pos));    //pin8 输出模式  M2A
	PB->MODE = (PB->MODE &~(GPIO_MODE_MODE7_Msk) |(0x1UL << GPIO_MODE_MODE7_Pos));    //pin7 输出模式  M2B
}

_User_ void Jet_io_set(uint_8 x) 
{
	PB10 = x; 
	PB9 = ~x;
}

_User_ void Wash_io_set(uint_8 x)  
{
	PB8 = x; 
	PB7 = ~x;
}


//初始化用户层和连接层的对接
_User_ void Flush_control_init(Flush_IO_cfg *p)
{
	
    Flush_IO_init(p,Jet_io_set,Wash_io_set);    
}





//============================================底层抽象层===============================================================

//=============集成方案
_Link_  void Flush_IO_init(Flush_IO_cfg *p,void (*jet_set)(uint_8 val),void (*wash_set)(uint_8 val))
{
	p->Jet_set_Value = jet_set;
	p->Wash_set_Value = wash_set;	

}

//============冲水板控制
_Link_ void Flush_board_init(Flush_IO_cfg *p,void (*borad_set)(uint_8 val))
{

	p->Washboard_set_Value = borad_set;

}

//============================================功能处理层=================================================================

//---------------------------
//----冲水板冲水处理
//--------------------------
_Toilet_ void FlushWater_Board_Deal	(void)
{
	if(Status.chongshuiban_f)	
	{		
		Flush_io.Washboard_set_Value(1);
		chongshui.delay1++;
		if(chongshui.delay1 > 200)		//----200*10ms=2000ms
		{
			chongshui.delay1 = 0;
			Status.chongshuiban_f = 0;
		}
	}
	else
	{
		Flush_io.Washboard_set_Value(0);
	}
}



