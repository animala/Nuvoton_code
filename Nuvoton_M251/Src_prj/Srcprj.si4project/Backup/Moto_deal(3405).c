#include "Moto_deal.h"


Motor_uion MotorA;
Motor_uion MotorB;
Motor_uion *motor_select[2];



#define arrySize(x)  (sizeof(x) / sizeof(x[0]))

#define	WangFu_18mm		160
#define	WangFu_2mm		20     //这里是喷杆往复的距离
#define	Reciprocate_add	50  //这个是往复运动的位置补偿


	
//----------------喷杆电机 P5.4.3.2.1 ABCD
uint_8 StepMotor_B[8] = {0x10,0x18,0x08,0x0c,0x04,0x06,0x02,0x12};

//-------水路分配电机   P8.7.6.4.3 ABCD
uint_8 StepMotor_A[8] = {0x80,0xc0,0x40,0x50,0x10,0x18 ,0x08,0x88};


const uint_16 PengGang[2][6]={           //调节位置表格需要调整 8-10改
//{1170,1296,1422,1548,1674,1800},							//臀部清洗喷杆位置 0归位 1,2,3,4,5档
{1280,1350,1420,1500,1576,1750},
{1280,1350,1420,1500,1576,1750},						 	//女性清洗喷杆位置 0归位 1,2,3,4,5档
};


//水压设定范围
const uint_16 WaterPressure[2][6]={
{2250,2350,2445,2540,2680,2790},					//臀部清洗喷杆位置 0归位 1,2,3,4,5档             {2000,2050,2050,2100,2280,2360},
{750,600,480,350,225,70},							//女性清洗喷杆位置 0归位 1,2,3,4,5档  

};

//------------------------水路分配器
#define 	CskMin			0
#define		CskZJ			4
#define		CskAddLiquid	9
#define 	CskMax			10

const uint_16 CSK_table[]=
{
	0,		//-------0	最小行程
	225,	//-------1	女性3
	450,	//-------2	女性2
	765,	//-------3	女性1,
	1559,	//-------5	男用1	//-------4	自洁口
	2340,	//-------6	男用2
	2565,	//-------7	男用3
	2790,	//-------8	男用MAX
	3364,	//-------9	加液口
	3544,	//-------10	最大行程
};


#define 	SSMin		0
#define 	SSRestIndex	1
#define 	SSMax		2

const uint_16 SS_table[]=
{
	100,				// 原始位置
	120,				// 工作结束后的待机位置
	1900,				//3332,			// 2					//--复位	
};


//----------------------------
//--端口设置结构体初始化
//--Motor_sum 电机总数
//----------------------------
Port_set p_set[Motor_sum]  =  {	
	{
	    StepMotor_A_IO,  		//初始化的IO口	分水电机
		StepMotorAStop,			//初始化输出0
		StepMotor_A,			//初始化电机的相位
		CSK_table,				//对应电机的位置选择
	},
	{
	    StepMotor_B_IO, 		//初始化的IO口	喷杆电机			
		StepMotorBStop,
		StepMotor_B,
		SS_table,
	}
};


//--------------步进电机控制指针函数原型------------
//
//-输入： 步进电机控制结构体 Motor_uion，步进电机选择 motor
//
//-输出： 无
//---------------------------------------------------
void Motor_Control(Motor_uion *p)
{
	
	if(p->enable_f == 1)						//电机运行标志
	{
		if(p->Index != p->SumCountSet)			 //如果步进电机的当前位置不等于目标设定的位置
		{			
			GPIO_OUT(p->pInit->port) &= p->pInit->stop;					//电机IO口初始化0
			GPIO_OUT(p->pInit->port) |= p->pInit->phase[p->XiangWei];		//再把电机的相位赋值给IO口			
			if(p->cw_f  ==  1)			//如果正转标志位置1						
			{
				if(p->XiangWei < 7)			//步进电机相位++				
				p->XiangWei++;
				else						//超过相位范围则重新赋值
				p->XiangWei = 0;
				p->Index++;
			}
			else						//步进电机反转
			{
				if(p->XiangWei > 0)			//相位自减
				{
					p->XiangWei--;
				}
				else						//溢出重新赋值
				{
					p->XiangWei = 7;
				}
				if(p->Index > 0)
				{
					p->Index--;
				}
			}
		}
		else								//步进电机到位
		{
			p->enable_f = 0;				//电机失能
			GPIO_OUT(p->pInit->port) &=  p->pInit->stop;   //电机IO输出停止
			p->end_f  =  1;				//到位
		}
	}
	else
	{
		if(Status.PcbTest_f == 0)	//如果PCB自检完成后
		{
			GPIO_OUT(p->pInit->port) &=  p->pInit->stop;	//IO输出0								
			p->XiangWei  =  0; //相位清0
		}
	}
}


/********************************/
//	根据选择电机标号获取数组最大的数值
// 0 分水电机   1 喷杆电机
/********************************/
uint_16 Slt_Table(uint_8 num)
{
	uint_16 temp;
	if(num == 0)
	{
		temp = arrySize(CSK_table) - 1;
	}
	else
	{
		temp = arrySize(SS_table) - 1; 
	}
	return temp;
}

/********************************************/
//		步进电机上电复位
//Motor_uion:传入步进电机结构体 
//count：步进电机的个数，本项目只有分水电机和喷杆电机
//**t:  取地址的地址->*(Motor[0])->Motor[0] = MortorB => *MortorB  got it!
//分水电机复位的步骤是先复位，再转到自洁口
//喷杆电机复位则是直接缩到起始点
/*********************************************/
void Power_up_reset(Motor_uion **t)
{
	//循环，初始化电机	
	uint_8 i = 0;
	Motor_uion *p = *t;	  //这里定义一个可变电机结构体，用来存放传进来的电机结构体
	//程序放在10ms中执行
	if(Status.PowerEnd_f == 0)   //上电标志位
	{
		if(timer.t_1s_f == 1)    //10秒计时
		{
			timer.init++;	
			timer.t_1s_f = 0;
		}		
		if(timer.init > 10)
		{
			timer.init = 10;   
		}	
		for (i = 0; i < 2; i++)
		{		
			switch(p->State)
			{
				case 0:							
				p->pInit = &p_set[i];		//初始化电机接口结构体的端口、相位等														
				p->cw_f = 0;							//统一反转  （根据硬件搭配不同，有些电机是复位是反转  这里需要注意）
				p->Index = p->pInit->position[Slt_Table(i)]; 	//当前位置设置为最大
				p->SumCountSet = p->pInit->position[0];		//目标位置设置最小 实现伸缩回位置
				p->State++;
				p->end_f = 0;			
				p->enable_f = 1;		//使能标志位置1													
				break;	
				case 1:
				if(p->end_f == 1 )	//转到目标位置之后														
				{
					if(i == 0)	//对分水电机继续操作
					{
						p->enable_f = 0;
						p->end_f = 0;										
						p->DelayTime = 0;
						p->State++;				
					}
					else		//喷杆电机完成复位
					{
						p->end_f = 0;										
						p->enable_f = 0;
						p->Compelet_flag = 0x01;
						p->select = 1;             //步进电机选择1为喷杆电机
					}
				}
				break;
				case 2:	//这里是让分水电机停止运行100ms
					p->DelayTime++;
					if(p->DelayTime >= 10)
					{
						p->DelayTime = 0;
						p->State++;	
					}
				break;
				case 3:
					//这里给定需要转到的位置				
					p->cw_f = 1;							//正转				
					p->SumCountSet = p->pInit->position[CskZJ]; 	//目标位置设置为自洁口
					p->enable_f = 1;					//使能，开始转动
					p->State++;	
				break;
				case 4:
					if(p->end_f == 1)	//转到目标位置之后														
					{	
						p->end_f = 0;	
						p->enable_f = 0;
						p->Compelet_flag = 0x10;
						p->select = 0;    //步进电机选择位0 为分水电机
					}
				break;
			}
			p++;   //电机结构体+1  //两次循环完成后 P是等于2；
		}		
		if(((p-1)->Compelet_flag || (p-2)->Compelet_flag == 0x11) && (timer.init==10))   //两个电机复位完成以及到达10秒
		{
			Status.PowerEnd_f = 1;				//上电复位完成标志位置1
			(p-1)->Compelet_flag = 0;			//各电机完成标志位置1
			(p-2)->Compelet_flag = 0;
			Status.PG_zero_f = 1;	
			Status.Divid_water_f = 1;
			Status.Dry_zero_f=1;		
			buz_set(1,30,30);				//叮一下
		}
	}
}


/*****************************************/
//   选择位置水压
//输入 :   motor ：0 分水电机 1 喷杆电机 
// s1、 s2  ： 对应的表格里数据的索引
// s1 功能索引， s2 位置索引
//输出位置值
//*****************************************/
uint_16 Slet_position(uint_8 motor, uint_8 s1, uint_8 s2, uint_8 s3)
{
	uint_16 temp;
	if(motor == 0)
	{
		if(s3 == 1)   //这里是选择了自洁口
		{
			temp = CSK_table[s2];    //这里直接赋值自洁口的位置
		}
		else
		{
			temp = WaterPressure[s1][s2];   //这个是调节水压
		}
	}
	else
	{
		if(s3 == 1)   //这里是选择了喷杆缩回
		{
			temp = SS_table[s2];    //这里直接赋值喷杆的位置
		}
		else
		{
			temp = PengGang[s1][s2];		//这个是调节喷杆位置
		}
		
	}
}

//*****************************************************************/
//步进电机设定位置距离，根据功能不同来设定
//Motor_uion *t 步进电机结构体，Status_uion 状态设定结构体
//*******************************************************************/
void Motor_set_position(Motor_uion *t, Status_uion *p)
{
	//目标设定位置的获取由Slet_position获得
	t->SumCountSet = Slet_position(t->select, p->Work_index-1, p->Comsum_set, p->Auto_clear);

	if(t->select == 0)    //这是对分水电机的位置控制 
	{
		if(t->Index == t->SumCountSet)
		{
			t->enable_f = 0;
			t->end_f = 1;
		}
		else
		{
			t->enable_f = 1;
			t->end_f = 0;
			if(t->Index > t->SumCountSet)
				t->cw_f = 0;
			else
				t->cw_f = 1;
		}
	}
	else			//喷杆电机有速度控制
	{
		if(t->Index == t->SumCountSet)
		{
			t->enable_f = 0;
			t->end_f = 1;
		}
		else
		{
			t->enable_f = 1;
			t->end_f = 0;
			if(t->Index > t->SumCountSet)
			{
				if(t->cw_f == 1)
				{
					t->Index += Reciprocate_add;   //反向补偿
				}
				t->cw_f = 0;
			}
			else
			{
				if(t->cw_f==0)
				{
					t->Index -= Reciprocate_add;  //反向补偿
				}
				t->cw_f = 1;		
			}
		}
	}
}


/*****************************************************************/
//    往复运动功能模块
//说明：喷杆电机的往复运动，和分水电机的水压按摩，根据输入的结构体选择
//功能：循环把模式装入电机结构体，并执行
//此功能对应 臀洗和女性
//分水电机下 前一个状态应是0档水压
/*******************************************************************/
void Reciprocate_fun(Motor_uion *t, Status_uion *p)
{	
	uint_16 Now_temp = 0;     //定义一个当前已设定的目标位置
	uint_16 Dis_temp = 0;		//定义一个需要到达的目标位置
										
	Now_temp = Slet_position(t->select, p->Work_index-1, p->Comsum_set, p->Auto_clear);			//赋值当前状态的目标位置
	Dis_temp = Slet_position(t->select, p->Work_index-1, (p->Comsum_set + 2), p->Auto_clear);	            //设定往复运动的目标位置

	/************************************************/
	//以下是对进入往复运动前的位置判定

	//处理当前位置小于前状态的目标位置
	if(((t->Index <= Now_temp - WangFu_2mm) && (t->select == 1)) ||		//这是喷杆电机选择
		((t->Index <= Now_temp ) && (t->select == 0)))					//这是分水电机选择
	{
		t->SumCountSet = Dis_temp;   //位置赋值给电机目标位置变量
		if((t->cw_f == 0) && (t->select == 1))
		{
			t->Index -= Reciprocate_add;	//喷杆电机赋值索引变量
		}		
		t->cw_f = 1;			//正转
		t->enable_f = 1;		//开始运作
		t->end_f = 0;	
	}
	//处理当前位置大于前状态的目标位置
	if(((t->Index >= (Dis_temp)) && (t->select == 1)) ||
		((t->Index >= Dis_temp) && (t->select == 0)))
	{
	
		if((t->cw_f == 1) && (t->select == 1))		//对喷杆电机进行操作
		{
			t->SumCountSet = Now_temp - WangFu_2mm;
			t->Index += Reciprocate_add;
		}
		else				//对分水电机进行操作
		{
			t->SumCountSet = Now_temp;
		}
		t->cw_f = 0;
		t->enable_f = 1;
		t->end_f = 0;
	}
	//处理当前位置等于前状态的目标位置
	if(t->Index == t->SumCountSet)
	{
		t->SumCountSet = Dis_temp;
		if((t->cw_f == 1) && (t->select == 1))
		{
			t->Index += Reciprocate_add;
		}	
		t->cw_f = 0;
		t->enable_f = 1;
		t->end_f = 0;
	}
}




/****************************************************************/
//		电机清洗功能流程模块
//输入：电机结构体，和功能标志位结构体
//
/***************************************************************/
void Motor_work_prosess(Motor_uion **t, Status_uion *s)
{	
	Motor_uion *p = *t;	  //这里定义一个可变电机结构体，用来存放传进来的电机结构体
	//则默认 p 就是 分水电机   p+1就是 喷杆电机
	switch(work.step)
	{
		case 0:
			s->Work_index = s->Mode_select;
			s->Comsum_set = s->Pres_select;   //这里先把设定好的值赋值给变量，带后面再用
			work.steptime = 0;  //单步工作时间计时 
			if ((p+1)->enable_f == 0)
			{
				s->QiBeng_f = 0;
				if (s->QingXiQieHuan_f == 1)
				{
					work.step = 5;		//如果是清洗切换  则跳到下一步
				}
				else
				{
					work.step++;
				}
			}
			break;
		case 1:
			
			Motor_set_position(p,s);    //这里是分水电机设置分水口位置
			work.step++;
			break;
		case 2:
			//等待旋转到位
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				work.step++;
			}
			break;
		case 3:
			//-----开泵进水
			Water.WaterIn_f = 1;   	
			Water.Hot_f = 1;
			work.steptime = 0;
			work.step++;
			break;
		case 4://自洁2秒
			if(++work.steptime > 150)
			{
				work.steptime = 0;
				work.step ++;
			}
			break;
		case 5:  //这里是切换功能时先切换到自洁口
			s->Auto_clear = 1;
			s->Comsum_set = CskZJ;  //选择水压档位自洁档位
			Motor_set_position(p,s);
			work.step++;
			break;
		case 6:
			//等待分水口旋转到位
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;   //清洗标志位清0
				Water.WaterIn_f = 1;
				Water.Hot_f = 1;   //把水加热
				work.step++;
			}
			break;
		case 7:
			if(s->QingXiQieHuan_f == 1)  //如果是切换清洗功能了
			{
				work.step++;     //直接跳到下一步
			}
			else
			{
				if(++work.steptime >= 10)    //-------0.1S
				{
					work.steptime = 0;
					work.step ++;
				}
			}
			break;
			//************以上是分水口初始化********************/
		case 8:
			s->Comsum_set = s->Pres_select;  //选择位置还原为设定值， Index 和 cumsum 这两个是 两个电机共用的
			Motor_set_position((p+1),s);   //对喷口电机初始化
			work.step++;
		break;
		case 9:
			//---------等待伸缩杆到位			
			if((p+1)->end_f == 1)
			{
				(p+1)->end_f = 0;
				(p+1)->enable_f = 0;
				work.step++;
			}
			break;
		case 10: //这里要转到最大档位进行清洗2秒后再转到相应档位，防止档位过小，即热模块停止加热
			s->Auto_clear = 2;   		//这里表格选择清洗
			s->Comsum_set = 4; 		 	//选择水压档位为最大
			Motor_set_position(p,s);    //这里是分水电机设置分水口位置
			Water.WaterIn_f = 1;
			Water.Hot_f = 1;
			work.step++;
			break;
		case 11:   //等待分水器到位
			//work.time++;
			if(p->end_f==1)
			{
				work.steptime = 0;
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;				//清洗选择归0
				s->Comsum_set = s->Pres_select;  //选择位置还原为设定值
				work.Start_f = 1;
				work.time = Water.WashTimer;
				work.step++;
			}
			break;
		case 12:  //清洗2秒
			if(++work.steptime > 200)
			{
				work.steptime = 0;
				work.step++;
			}			
			break;
		case 13:  //清洗之后就转到相应的功能口进行清洗		
			s->Comsum_set = s->Pres_select;  //选择位置还原为设定值
			Motor_set_position(p,s);    	//这里是分水电机设置分水口位置
			work.step++;
		break;
		
		case 14: //等待旋转相应的功能口位置
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->QiBeng_f = 1;   //这里开启气泵
				s->Working_f = 1;   //正在工作标志位置1
				work.step++;
			}
		break;
		//****************************************************************************************************/
		// ----------以上是相应的功能清洗 分水电机和喷杆电机相应的执行步骤-------------//


		case 15:   //喷杆位置微调和水压微调和往复运动操作步骤
			//********水压微调*******//
			if(Water.pressure_f == 1)   
			{
				Water.pressure_f = 0;
				s->Comsum_set = s->Icon.water_yeild;   //水量档位赋值给选择档位，选择水量的大小
				Motor_set_position(p,s);    //这里是分水电机设置水压位置
				if(p->end_f == 1)
				{
					p->end_f = 0;
					p->enable_f = 0;
					IR.Last_value = s->Comsum_set;     //把这次的值保存起来，处理调节水势的时候位置也一起调节的问题
				}
				
			}
			//*******喷杆微调**********//
			if(PenGan.PGmove_f == 1)   
			{					
				s->Comsum_set = s->PG_gear;   //喷杆档位赋值给选择档位，选择喷杆的位置
				Motor_set_position((p+1),s);    //这里是喷杆电机设置喷杆位置
				if((p+1)->end_f == 1)
				{
					(p+1)->end_f = 0;
					(p+1)->enable_f = 0;
					PenGan.PGmove_f = 0;     //关闭喷杆微调
				}
				
			}			
			//********往复运动********//
			if(s->reciprocate_f == 1)   
			{						
				Reciprocate_fun((p+1),s);   //喷杆电机进行往复运动				
			}
			//********水压按摩**********//
			if(s->ShuiYaAnMo_f == 1)			
			{
				if(++s->ShuiYaAnMo_timer < 150)  //1.5S 开
				{
					s->QiBeng_f = 1;  
				}
				else if((++s->ShuiYaAnMo_timer > 150) && (++s->ShuiYaAnMo_timer < 300))  //1.5 S 关
				{
					s->QiBeng_f = 0;
				}
				else
				{
					s->ShuiYaAnMo_timer = 0;
				}
			}
			else		//如果没有则持续输出气压
			{
				s->ShuiYaAnMo_timer = 0;
				s->QiBeng_f = 1;
			}
			Water.WaterIn_f = 1;
			Water.Hot_f = 1;
			
			if(work.time == 0) //清洗时间结束
			{
				s->Mode_select = 0;		//选择清0
				s->ShuiYaAnMo_f = 0;	// 按摩标志位清0
				s->reciprocate_f = 0;	//往复运动清0

				s->Comsum_set = 0;  //选择目标位置还原为设定值，
				s->Work_index = 0;  //选择位置还原为设定值，方便结束流程操作
				
				work.Start_f = 0;
				work.step = 0;
			}
		break;
		/*****************************************************/
		//**********以上是按摩相关操作步骤***********/		
	}
}

//****************************************************************/
//		电机清洗功能结束流程模块
//输入：电机结构体，和功能标志位结构体
//
//***************************************************************/
void Motor_work_end(Motor_uion **t, Status_uion *s)
{

	Motor_uion *p = *t;	  //这里定义一个可变电机结构体，用来存放传进来的电机结构体
	//默认 p 是分水电机            p+1是喷杆电机 对应的结构体
	s->QiBeng_f = 0;
	switch (work.step)
	{
	   //---分水阀旋回自洁口
		case 0:	
			s->Auto_clear = 1;		//选择自洁标志位
			s->Comsum_set = CskZJ;  //选择位置为自洁口
			Water.Hot_f = 0;		//关闭热水进入
			Motor_set_position(p,s);    //这里是分水电机设置
			work.step++;
		break;
			
		case 1:  //-------等待旋转到位
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;
				work.steptime = 0;
				work.step++;
			}
		break;

				//喷杆缩回最小位置
		case 2:
			s->Auto_clear = 1;			//选择最小位置
			s->Comsum_set = SSMin;    //选择位置为最小位置
			Motor_set_position((p+1),s);    //这里是喷杆电机设置
			work.step++;			
		break;

		case 3:
				//-------等待伸缩杆到位			
			if((p+1)->end_f == 1)
			{
				(p+1)->end_f = 0;
				s->Auto_clear = 0;
				(p+1)->enable_f = 0;	//关闭电机使能
				s->PG_zero_f = 1;     //喷杆收回到位
				work.step++;
				work.steptime = 0;
			}
		break;

		case 4://-------自洁2秒
			if(++ work.steptime >= 200)
			{
				work.steptime = 0;
				work.step++;
			}
		break;
		case 5://自洁后等待1.5S  等待残水排出
			Water.WaterIn_f = 0;  //关闭进水
			Water.Hot_f = 0;		//关闭加热
			if(++work.steptime > 150)
			{
				work.steptime = 0;
				work.step++;
			}
		break;
			//----一下是增加复位功能，复位之后再待机
		case 6:
			s->Auto_clear = 1;
			s->Comsum_set = CskMin;
			s->Work_index += 50;
			Motor_set_position(p,s);    //这里是分水电机设置

			s->Comsum_set = SSRestIndex;
			s->Work_index = s->Mode_select;
			Motor_set_position((p+1),s);    //这里是喷杆电机设置
			work.step++;
		break;

		case 7:
				//--------------------等待归零	
			if((p->end_f==1)&&((p+1)->end_f==1))
			{
				p->end_f=0;
				p->enable_f=0;
				(p+1)->end_f=0;
				(p+1)->enable_f=0;
				work.step++;
			}
		break;

		case 8://----恢复到待机状态
			s->Comsum_set = CskZJ;  //选择自洁口
			Motor_set_position(p,s);    //这里是分水电机设置
			work.step++;
		break;

		case 9://----等待归位
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;
				s->Comsum_set = 0;  //选择位置还原为设定值， 
				work.steptime = 0;
				work.step++;
			}
		break;

		case 10: //-------流程结束
			work.enable_f = 0;  
			s->PG_zero_f = 1;    //喷杆工作完成缩回初始位置标志位
			s->Divid_water_f = 1;  //分水电机进入待命
			if(work.LianXu_f == 1)  //如果使能了连续工作
			{
				work.time = Drying.WorkingTimer;  //结束清洗后就开始进入烘干流程
				s->stop_f = 0;
				s->Mode_select = 0;
				s->QingXiQieHuan_f = 0;
				work.step = 0;
				s->dring_f = 1;
				Drying.enable_f = 1;
				s->DringTiaoJie_f = 1;
				Drying.step = 0;
				s->ZuoYu_f = 0;
				s->LengReAnMo_f = 0;
				s->ShuiYaAnMo_f = 0;
			}
		break;
		default:break;
	}
}

//************************
//  步进电机工作函数	
//***********************/
void Motor_A_B_C_program(Motor_uion **P, Status_uion *S)
{
	if(work.enable_f == 1)	
	{
			if(Status.Mode_select != 0)
			{
				//-------------------功能处理
				if((Status.auto_f == 1) && (Status.test_f == 0))
				{
					Status.chongshui_e = 1;
				}	
				Status.PG_zero_f = 0;
				Motor_work_prosess(P,S);
			}
			else
			{
				Motor_work_end(P,S);
			}
	}
	else
	{
		if(Status.Mode_select == 0)
		{
			Status.stop_f = 0;
		}
	}	
}

//************************
  喷口清洗工作函数	
//***********************/
void Spout_clean(Motor_uion **t, Status_uion *s)
{
	uint_8 Index = 0; //定义一个索引变量
	uint_8 Comsum = 0; //定义一个目标位置变量

	Motor_uion *p = *t;	  //这里定义一个电机结构体变量，用来存放传进来的电机结构体

	if(penkou.enable_f == 1)   //入座不可进行喷口清洗
	{
		CoverSeat.delay = T5Min;
		if(s->penkou_f == 1)
		{
			switch(penkou.step)
			{
				case 0:
					penkou.delay1 = 0;   //----工作时间计时清0
					penkou.delay2 = 0;
					penkou.step++;
				break;
				case 1:
					Water.WaterIn_f = 1;  //---进水
					Water.Hot_f = 1;    //--进热水
					penkou.step++;
				break;
				case 2:
					if(++penkou.delay1 > 300)    //--自洁3秒
					{
						penkou.delay1 = 0;
						penkou.step++;
					}
				break;
				case 3:   //  1 5 0
					s->Auto_clear = 0;
					s->Work_index = 1;              
					s->Comsum_set = 5;    			//喷杆设置最大
					Motor_set_position((p+1),s);    //这里是喷杆电机设置为喷口清洗位置
					penkou.step++;	
				break;
				case 4:
					if((p+1)->end_f==1)   //等待缩回位置
					{				
						(p+1)->end_f=0;
						(p+1)->enable_f=0;
						penkou.delay2 = 12000;
						penkou.step++;
					}
				break;
				case 5:
					if(--penkou.delay2 < 0)    //清洗2分钟 12000 * 10ms /1000ms = 120S
					{
						penkou.delay2 = 0;
						s->penkou_f = 0;
						penkou.step = 0;
					}
				break;
				default:break;
			}
		}
		else
		{
			switch(penkou.step)
			{
				case 0:
					Water.WaterIn_f = 1;
					Water.Hot_f = 0;
					penkou.delay1 = 0;
					penkou.step++;
				break;
				case 1:
					if(++penkou.delay1 > 100)
					{
						penkou.delay1 = 0;
						penkou.step++;
					}
				break;
				case 2:
					s->Auto_clear = 1;
					s->Comsum_set = CskZJ;
					Motor_set_position(p,s);    //这里是分水电机设置

					s->Comsum_set = SSMin;
					Motor_set_position((p+1),s);    //这里是喷杆电机设置
					penkou.step++;
				break;
				case 3:
					//--------------------等待归零	
					if((p->end_f==1)&&((p+1)->end_f==1))
					{
						p->end_f=0;
						p->enable_f=0;
						(p+1)->end_f=0;
						(p+1)->enable_f=0;
						penkou.step++;
					}
				break;
				case 4:
					s->Work_index = SSRestIndex;
					s->Comsum_set = SSMin;
					Motor_set_position((p+1),s);    //这里是喷杆电机设置
					penkou.step++;
				break;

				case 5:
					if((p+1)->end_f == 1)
					{
						(p+1)->end_f = 0;
						s->Auto_clear = 0;
						(p+1)->enable_f = 0;	//关闭电机使能					
						penkou.step++;
					}
				break;
				case 6:
					penkou.enable_f = 0;
					Water.WaterIn_f = 0;
					Water.Hot_f = 0;
				break;
				default: break;					
			}
		}
	}	
}












