#ifndef  _TOILET_H_
#define  _TOILET_H_

#define uint_8	unsigned char
#define uint_16	unsigned int
#define int_8	signed char

#include <temp_deal.h>
#include <table.h>
#include <deodorzation.h>
#include <flush.h>
#include <beep.h>



//----------------------旋转按键板定义
#define 	ReadKey		0x41		//-----读按键
#define 	WriteLed	0x42		//-----写LED
#define		StingBrightness		0x43		//-----设置亮度
#define		StingSpeed			0x44		//-----设置跑马灯速度。



typedef struct{
		uint_16	LedBuf[3];	
		uint_8	Brightness;		//-------分10级
		uint_8	SpeedCount;		//-------分10级		
		uint_8	DispalyMode;	//-----选择那种方式亮LED灯
								//----=0，可以同时点亮每一个灯。
								//----=1，选择渐亮渐灭模式，只能点亮同一色灯
								//----=2,跑马灯
		uint_8	ChoiceRGB;		//-----选择那种颜色
								//----=0红色
								//----=1绿色
								//----=2兰色

		uint_8	BitCount;
		uint_8	BtyeCount;
		uint_8	Index;
		uint_8	Data;
		uint_8	Buffer[7];
				//-------写LED	数据缓存
				//-------Buffer[0]	命令
				//-------Buffer[2]	LED1
				//-------Buffer[3]	LED2
		uint_8	Step;
		uint_8	ReadOrWrite;		//---=0写LED， =1读按键
		uint_8	Delay;
		uint_8	KeyData;
		
		}Rolling_CB;
extern  Rolling_CB Rolling;

extern	uint_8 eeprom[8];

typedef struct{

		uint_8	count_200u;
		uint_8	count_1ms;
		uint_8	count_10ms;
		uint_8	init;					//------上点初始化时间
		uint_8 	t_10ms_f:1;
		uint_8 	t_100ms_f:1;
		uint_8 	t_500ms_f:1;
		uint_8 	t_1s_f:1;
		uint_8  t_power_1s:1;
		uint_8 t_receive_1s;
		uint_8  count_flash;
		}timer_uion;
extern  timer_uion timer;

typedef struct{
		uint_8	temperature;		//--------水温
		uint_8	DangWei;			//--------档位
		uint_8	pressure_f;			//--------水压使能
		uint_8	WaterIn_f:1;			//--------进水使能
		uint_8	shuiwei_count;		//--------水位检测计数
		uint_16	jinshuishijian;
		uint_8	TempDeal_time;
		uint_8	AnMoData;
		uint_8	AnMoCount;
		uint_8	JiRe_In_f:1;
		uint_16	delay1;
		uint_16	RX_delay;			//---------即热试接受信号时间超出时间无接受信号报警

		uint_8  flow_count;     //流量
		uint_8	InputTemp;
		uint_8	OutputTemp;
		uint_8	OutputTempBack;
		uint_8	OutputTempCount;		

		uint_16	StopHotDelay;
		uint_8	Hot_f:1;
		uint_8  Error_flag;
		uint_8	WashTimer;			//-----清洗时间
		uint_8	WaterInTime;
		}Water_uion;
extern  Water_uion Water;

typedef struct{
		uint_8	temperature;		//-------座温实际温度	
		uint_8	DangWei;			//------座温档位
		uint_16	count1;				//------着座记时
		uint_16	count2;				//------着座记时
		uint_8	first_f:1;			//------
		uint_8	Heat_f:1;
		uint_16	SeatOnTimer;		//-------落座时间
		uint_8	Flag;				//---BIT0=1 表示着座过， BIT1=1 表示离座过
		
		}Seat_uion;
extern  Seat_uion Seat;


typedef struct{
		uint_16	Vale;				//------羽ｒ请D值	
		uint_8	Lamp_f:1;			//-------灯亮，
		uint_16	Timer;				//-------10分钟控制延时。	
		}LightInduce_CB;
extern  LightInduce_CB LightInduce;


typedef struct{
		uint_8	temperature;
		uint_8	DangWei;
		uint_8	DutyPwm;
		uint_8	DutyCount;
		
		uint_8	step;
		uint_16	time;
		uint_8	enable_f:1;
		//-----烘干冲洗为0烘干时冲水为1表示连续按烘干键不冲水
		uint_8	swash_f:1;	

		uint_8	WorkingTimer;
		uint_8	HotEndTime;	

		uint_8	HotStartTime;

		
		}Drying_uion;
extern  Drying_uion Drying;

typedef struct{
		uint_8	temperature;
		uint_16	time;
		uint_8	Index;

		}Entironment_CB;
extern  Entironment_CB Entironment;



typedef struct{
		uint_8	PGmove_f;
		uint_8	PGmove_dir:1;
		uint_8	PGanmo_dir:1;
		
		}PenGan_uion;
extern  PenGan_uion PenGan;
typedef struct{
		uint_16	value;
		uint_16	Delay;
		uint_8	MotorA_reset;
		uint_8	MotorB_reset;
		}Error_uion;
extern  Error_uion Error;

//-------------------------------
typedef struct{
//		uint_8	DangWei;	//-----水温		
		uint_8	pressure;	//----水压
		uint_8	PGLevel;	//----喷杆位置

		}TunBu_uion;
extern  TunBu_uion TunBu;

typedef struct{
//		uint_8	DangWei;
		uint_8	pressure;
		uint_8	PGLevel;
		
		}NvXing_uion;
extern  NvXing_uion NvXing;

typedef struct{
		uint_8	DangWei;
		uint_8	pressure;
		uint_8	PGLevel;
		uint_8	Dring_f:1;
		}Enfant_uion;
extern  Enfant_uion Enfant;			//----儿童
typedef struct{
		uint_8	DangWei;
		uint_8	pressure;
		uint_8	PGLevel;
				
		}TongBian_uion;
extern  TongBian_uion TongBian;
typedef struct{
		uint_8	step;
		uint_16		delay;
		uint_8	PGLevel;
		uint_16		delay1;
		}chongshui_uion;
extern  chongshui_uion chongshui;

typedef struct{
		uint_8	step;
		uint_16	delay1;
		uint_16	delay2;
		uint_8	enable_f:1;
		}penkou_uion;
extern  penkou_uion penkou;

typedef struct{
		uint_8	step;
		uint_16	delay;
		}fangwu_uion;
extern  fangwu_uion fangwu;

typedef struct{
		uint_8	step;
		uint_16	delay;
		uint_8	enable_f:1;	
		uint_8	SeatOpen_f:1;
		uint_8	CoverOpen_f:1;
		uint_8	SeatDirStatus;		//---座圈翻动方向	
		uint_8	CoverDirStatus;		//---座盖翻动方向
		uint_8	SpaceTime;			//---清状态间隔时间
		uint_8 Cover_select;

		
		}CoverSeat_uion;
extern  CoverSeat_uion CoverSeat;

typedef struct{
		long int	vale;
		long int	back;
		uint_8	indx;
		uint_16	count1;		
		uint_8	count2;
		uint_8	flag1:1;
		uint_8	flag2:1;
		uint_8	updata_f:1;				
		}KEY_uion;
extern  KEY_uion KEY;

typedef struct{
		uint_8	ring_times;		
		uint_8	ring_time_set;
		uint_8	stop_time_set;
		uint_8	task_count;
		uint_8	ring_time;
		uint_8	stop_time;				
		}buzzer_uion;
extern  buzzer_uion buzzer;

typedef struct{
		uint_8	enable_f:1;	
		uint_8	LianXu_f:1;	
		uint_8	LianXuBuf;		//----=0取消连续清洗了
								//---bit0=1儿童转烘干
								//---bit1=1男用转烘干
								//---bit2=1女性转烘干		
		uint_8	step;
		uint_8	dring_step;
		uint_16	steptime;
		uint_8	Start_f:1;
		uint_16	time;
					
		}work_uion;
extern  work_uion work;


typedef struct{
		uint_8	Softpower_f:1;
		uint_16	Delay;

		}SoftPowerDeal_CB;
extern  SoftPowerDeal_CB SoftPowerDeal;
typedef struct{
		uint_8 water_press_gear;    //水压档位
		uint_8 seat_gear;			//座温档位
		uint_8 water_yeild;			//水量档位
		uint_8 water_temp_gear;   	//水温档位 
		uint_8 wind_gear;			//风温档位
		uint_8 energy_disp:1;		
		uint_8 power_disp:1;
}Icon_;

typedef struct{
		uint_8 power_time;
		uint_8 power_on:1;
		uint_8 power_off:1;
		uint_8 step;
}Power_union;


typedef struct{
		uint_8	power_f:1;
		uint_8	PowerEnd_flag;
		uint_8	PowerEnd_f:1;		
		uint_8	seat_f:1;
		uint_8	stop_f:1;
		uint_8	tunbu_f:1;
		uint_8	nvxing_f:1;
		uint_8	enfant_f:1;
		uint_8	QingXiQieHuan_f:1;
		uint_8	QiBeng_f:1;				//----气泵
		uint_8	dring_f:1;
		uint_8	DringTiaoJie_f:1;
		uint_8	penkou_f:1;
		uint_8	ShuiWenSet_f:1;
		uint_8	ZuoWenSet_f:1;
		uint_8	ShuiYaAnMo_f:1;
		uint_16	ShuiYaAnMo_timer;

		uint_8 	Auto_clear;
		uint_8	TunBuShuiYaTiaoJie_f:1;	
		uint_8	NvXingShuiYaTiaoJie_f:1;	
		uint_8	EnfantShuiYaTiaoJie_f:1;
		
		uint_8  Mode_select;			//妯″紡閫夋嫨鏍囧織浣?
		uint_8  Pres_select;			//妯″紡姘村帇閫夋嫨鏍囧織浣?
		uint_8  Work_index;
		uint_8  Comsum_set;
		uint_8  Working_f:1;      //正在工作
		uint_8 receive_delay;    //接收延时
		uint_8  PG_gear;				//鍠锋潌浣嶇疆寰皟
		uint_8  Water_gear;				//s姘村帇寰皟
		uint_8	ZuoYu_f:1;			//-----座浴		
		uint_8	jieneng_f:1;			//------节能标志
		uint_8	jieneng_e:1;			//------节能使能标志
		uint_8	auto_f:1;
		uint_8	reciprocate_f:1;
		uint_8	dring_wf_f:1;
		uint_8	chongshuiban_f:1;
		uint_8	chongshui_f:1;
		uint_8	chongshui_e:1;		//----自动冲水使能		
		uint_8	cover_f:1;
		uint_8	zuoquan_f:1;
		uint_8	LengReAnMo_f:1;
		uint_8	dengkong_f:1;
		uint_8	ganying_f:1;
		uint_8	PG_zero_f:1;
		uint_8  Divid_water_f:1;    //分水电机进入待命标志位
		uint_8	Dry_zero_f:1;
		uint_8	chuchou_e:1;
		uint_8	chuchou_f:1;
		uint_8	fangwu_f:1;
		uint_8  Power_e:1;
		uint_8	test_f:1;
		uint_8	PcbTest_f:1;
		NvXing_uion Woem;      //  2019-7-19 鎶婄浉搴旂殑妯″紡缁撴瀯浣撴斁鍏ヨ繖閲岋紝缁熶竴鐢ㄨ繖涓粨鏋勪綋璁块棶
		TunBu_uion  Hip;
		Icon_   Icon;
		Power_union Pwr;
		//Posotion_ pst;   //浣嶇疆缁撴瀯浣?
		} Status_uion;
extern  Status_uion Status;


typedef struct{
		uint_8	step;
		uint_16	time_h;
		uint_16	time_l;
		uint_8	start_f:1;
		uint_8	first_f:1;
		uint_8	already_f:1;
		uint_8	OK_f:1;
		uint_8	Bit_Count;
		uint_8	Byte_Count;
		uint_8	Recv_Data_Buf;
		uint_8	TFDelay;
		uint_8 Rec_delay;
		uint_8 Last_value;
		}IR_uion;
extern  IR_uion IR;


typedef struct{
		uint_8 ack_flag:1;
		}iic_uion;
extern  iic_uion iic;

//------------------液体箱处理


#define  LiquidCount	10
typedef struct{

		uint_8	Enable_f:1;		
		//------停止/使能标志
		uint_8	Auto_f:1;


		uint_8	Flag;
		uint_8	Start_f:1;		
		//-----=0初始化处理，=1正常加液处理
		uint_8	Frist_f:1;		
		//-----=1第一次初始化 =0已初始化过。
		
		uint_8	Step;
		uint_16	Delay;
		//------------	
		uint_8	Level_f;			
		//------液体箱液位检测
		uint_8	LevelCount;		
		//------液位检测次数
		uint_8	LevelFlash_f:1;	
		//------闪烁处理
		uint_8	ADDLiquidCount;		
		//------检测到无液提示后还可以清洗次数
		//--------
		uint_8	PenGanLeve;	
		//------喷杆档位	
		uint_8	TimeCount;		
		//------间隔计时
		uint_16		Time1SCount;	
		//------间隔计时
		//---------
		uint_8	WaterWash_f:1;	
		//------冲过水标志	
		uint_8	SetOff1Min;		
		//------1分钟计时
		uint_8	PumbEable_f:1;	
		//------气泵启动标志
		uint_16	PumbDuty;		
		//------气泵占空比
		uint_8	Stop_f:1;
			
		}LiquidDeal_CB;
extern  LiquidDeal_CB LiquidDeal;

typedef struct{
		uint_8	step;
		uint_16	time_h;
		uint_16	time_l;
		uint_8	start_f:1;
		uint_8	first_f:1;
		uint_8	already_f:1;
		uint_8	OK_f:1;
		uint_8	Bit_Count;
		uint_8	Byte_Count;
		uint_8	Recv_Data_Buf;
		uint_8	JpioTest;
		}FGIR_CB;
extern  FGIR_CB FGIR;

typedef struct{
		uint_8	Step;
		uint_8	Enable;
		uint_8	Start_f:1;
		uint_8	Count;
		uint_8	Bit_Count;
		uint_8	Byte_Count;
		uint_8	Data_Buf;
		uint_8	TF;	
		uint_8	Ok_f:1;			//----发送完数据

		uint_8 SaveData_f:1;
		uint_8 Change_f:1;
		
		}SendData_CB;
extern  SendData_CB SendData;



typedef struct{
		uint_8 	TxLong;
		uint_8	TxIndex;
		}Uart0Deal_CB;
extern  Uart0Deal_CB Uart0Deal;




//-------------------PCB出厂测试
typedef struct{
		uint_8 	step;
		uint_8	MotorCount;
		uint_8	LoadCount;

		
		uint_16 delay;
		uint_16 delay2;
		}PcbZiJian_uion;
extern  PcbZiJian_uion PcbZiJian;
//-----------------老化测试
typedef struct{
		uint_8 step;
		uint_16 delay;

		uint_8	LedLoop;
		uint_8	LedFlashTime;	
		uint_8	Count;		//---老化次数

		uint_16	Zyj1;
		uint_16	Zyj2;
		
		}test_uion;
extern  test_uion test;
//----------------整机出厂测试
typedef struct{
		uint_8 	Flag;
		uint_8 	Step;
		uint_16 	Delay;
		uint_8	CheckWifiSignal_f:1;

		}MachineTest_CB;
extern  MachineTest_CB MachineTest;



#endif

