#include "Moto_deal.h"


Motor_uion MotorA;
Motor_uion MotorB;
Motor_uion *motor_select[2];



#define arrySize(x)  (sizeof(x) / sizeof(x[0]))

#define	WangFu_18mm		160
#define	WangFu_2mm		20     //��������������ľ���
#define	Reciprocate_add	50  //����������˶���λ�ò���


	
//----------------��˵�� P5.4.3.2.1 ABCD
uint_8 StepMotor_B[8] = {0x10,0x18,0x08,0x0c,0x04,0x06,0x02,0x12};

//-------ˮ·������   P8.7.6.4.3 ABCD
uint_8 StepMotor_A[8] = {0x80,0xc0,0x40,0x50,0x10,0x18 ,0x08,0x88};


const uint_16 PengGang[2][6]={           //����λ�ñ����Ҫ���� 8-10��
//{1170,1296,1422,1548,1674,1800},							//�β���ϴ���λ�� 0��λ 1,2,3,4,5��
{1280,1350,1420,1500,1576,1750},
{1280,1350,1420,1500,1576,1750},						 	//Ů����ϴ���λ�� 0��λ 1,2,3,4,5��
};


//ˮѹ�趨��Χ
const uint_16 WaterPressure[2][6]={
{2250,2350,2445,2540,2680,2790},					//�β���ϴ���λ�� 0��λ 1,2,3,4,5��             {2000,2050,2050,2100,2280,2360},
{750,600,480,350,225,70},							//Ů����ϴ���λ�� 0��λ 1,2,3,4,5��  

};

//------------------------ˮ·������
#define 	CskMin			0
#define		CskZJ			4
#define		CskAddLiquid	9
#define 	CskMax			10

const uint_16 CSK_table[]=
{
	0,		//-------0	��С�г�
	225,	//-------1	Ů��3
	450,	//-------2	Ů��2
	765,	//-------3	Ů��1,
	1559,	//-------5	����1	//-------4	�Խ��
	2340,	//-------6	����2
	2565,	//-------7	����3
	2790,	//-------8	����MAX
	3364,	//-------9	��Һ��
	3544,	//-------10	����г�
};


#define 	SSMin		0
#define 	SSRestIndex	1
#define 	SSMax		2

const uint_16 SS_table[]=
{
	100,				// ԭʼλ��
	120,				// ����������Ĵ���λ��
	1900,				//3332,			// 2					//--��λ	
};


//----------------------------
//--�˿����ýṹ���ʼ��
//--Motor_sum �������
//----------------------------
Port_set p_set[Motor_sum]  =  {	
	{
	    StepMotor_A_IO,  		//��ʼ����IO��	��ˮ���
		StepMotorAStop,			//��ʼ�����0
		StepMotor_A,			//��ʼ���������λ
		CSK_table,				//��Ӧ�����λ��ѡ��
	},
	{
	    StepMotor_B_IO, 		//��ʼ����IO��	��˵��			
		StepMotorBStop,
		StepMotor_B,
		SS_table,
	}
};


//--------------�����������ָ�뺯��ԭ��------------
//
//-���룺 ����������ƽṹ�� Motor_uion���������ѡ�� motor
//
//-����� ��
//---------------------------------------------------
void Motor_Control(Motor_uion *p)
{
	
	if(p->enable_f == 1)						//������б�־
	{
		if(p->Index != p->SumCountSet)			 //�����������ĵ�ǰλ�ò�����Ŀ���趨��λ��
		{			
			GPIO_OUT(p->pInit->port) &= p->pInit->stop;					//���IO�ڳ�ʼ��0
			GPIO_OUT(p->pInit->port) |= p->pInit->phase[p->XiangWei];		//�ٰѵ������λ��ֵ��IO��			
			if(p->cw_f  ==  1)			//�����ת��־λ��1						
			{
				if(p->XiangWei < 7)			//���������λ++				
				p->XiangWei++;
				else						//������λ��Χ�����¸�ֵ
				p->XiangWei = 0;
				p->Index++;
			}
			else						//���������ת
			{
				if(p->XiangWei > 0)			//��λ�Լ�
				{
					p->XiangWei--;
				}
				else						//������¸�ֵ
				{
					p->XiangWei = 7;
				}
				if(p->Index > 0)
				{
					p->Index--;
				}
			}
		}
		else								//���������λ
		{
			p->enable_f = 0;				//���ʧ��
			GPIO_OUT(p->pInit->port) &=  p->pInit->stop;   //���IO���ֹͣ
			p->end_f  =  1;				//��λ
		}
	}
	else
	{
		if(Status.PcbTest_f == 0)	//���PCB�Լ���ɺ�
		{
			GPIO_OUT(p->pInit->port) &=  p->pInit->stop;	//IO���0								
			p->XiangWei  =  0; //��λ��0
		}
	}
}


/********************************/
//	����ѡ������Ż�ȡ����������ֵ
// 0 ��ˮ���   1 ��˵��
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
//		��������ϵ縴λ
//Motor_uion:���벽������ṹ�� 
//count����������ĸ���������Ŀֻ�з�ˮ�������˵��
//**t:  ȡ��ַ�ĵ�ַ->*(Motor[0])->Motor[0] = MortorB => *MortorB  got it!
//��ˮ�����λ�Ĳ������ȸ�λ����ת���Խ��
//��˵����λ����ֱ��������ʼ��
/*********************************************/
void Power_up_reset(Motor_uion **t)
{
	//ѭ������ʼ�����	
	uint_8 i = 0;
	Motor_uion *p = *t;	  //���ﶨ��һ���ɱ����ṹ�壬������Ŵ������ĵ���ṹ��
	//�������10ms��ִ��
	if(Status.PowerEnd_f == 0)   //�ϵ��־λ
	{
		if(timer.t_1s_f == 1)    //10���ʱ
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
				p->pInit = &p_set[i];		//��ʼ������ӿڽṹ��Ķ˿ڡ���λ��														
				p->cw_f = 0;							//ͳһ��ת  ������Ӳ�����䲻ͬ����Щ����Ǹ�λ�Ƿ�ת  ������Ҫע�⣩
				p->Index = p->pInit->position[Slt_Table(i)]; 	//��ǰλ������Ϊ���
				p->SumCountSet = p->pInit->position[0];		//Ŀ��λ��������С ʵ��������λ��
				p->State++;
				p->end_f = 0;			
				p->enable_f = 1;		//ʹ�ܱ�־λ��1													
				break;	
				case 1:
				if(p->end_f == 1 )	//ת��Ŀ��λ��֮��														
				{
					if(i == 0)	//�Է�ˮ�����������
					{
						p->enable_f = 0;
						p->end_f = 0;										
						p->DelayTime = 0;
						p->State++;				
					}
					else		//��˵����ɸ�λ
					{
						p->end_f = 0;										
						p->enable_f = 0;
						p->Compelet_flag = 0x01;
						p->select = 1;             //�������ѡ��1Ϊ��˵��
					}
				}
				break;
				case 2:	//�������÷�ˮ���ֹͣ����100ms
					p->DelayTime++;
					if(p->DelayTime >= 10)
					{
						p->DelayTime = 0;
						p->State++;	
					}
				break;
				case 3:
					//���������Ҫת����λ��				
					p->cw_f = 1;							//��ת				
					p->SumCountSet = p->pInit->position[CskZJ]; 	//Ŀ��λ������Ϊ�Խ��
					p->enable_f = 1;					//ʹ�ܣ���ʼת��
					p->State++;	
				break;
				case 4:
					if(p->end_f == 1)	//ת��Ŀ��λ��֮��														
					{	
						p->end_f = 0;	
						p->enable_f = 0;
						p->Compelet_flag = 0x10;
						p->select = 0;    //�������ѡ��λ0 Ϊ��ˮ���
					}
				break;
			}
			p++;   //����ṹ��+1  //����ѭ����ɺ� P�ǵ���2��
		}		
		if(((p-1)->Compelet_flag || (p-2)->Compelet_flag == 0x11) && (timer.init==10))   //���������λ����Լ�����10��
		{
			Status.PowerEnd_f = 1;				//�ϵ縴λ��ɱ�־λ��1
			(p-1)->Compelet_flag = 0;			//�������ɱ�־λ��1
			(p-2)->Compelet_flag = 0;
			Status.PG_zero_f = 1;	
			Status.Divid_water_f = 1;
			Status.Dry_zero_f=1;		
			buz_set(1,30,30);				//��һ��
		}
	}
}


/*****************************************/
//   ѡ��λ��ˮѹ
//���� :   motor ��0 ��ˮ��� 1 ��˵�� 
// s1�� s2  �� ��Ӧ�ı�������ݵ�����
// s1 ���������� s2 λ������
//���λ��ֵ
//*****************************************/
uint_16 Slet_position(uint_8 motor, uint_8 s1, uint_8 s2, uint_8 s3)
{
	uint_16 temp;
	if(motor == 0)
	{
		if(s3 == 1)   //������ѡ�����Խ��
		{
			temp = CSK_table[s2];    //����ֱ�Ӹ�ֵ�Խ�ڵ�λ��
		}
		else
		{
			temp = WaterPressure[s1][s2];   //����ǵ���ˮѹ
		}
	}
	else
	{
		if(s3 == 1)   //������ѡ�����������
		{
			temp = SS_table[s2];    //����ֱ�Ӹ�ֵ��˵�λ��
		}
		else
		{
			temp = PengGang[s1][s2];		//����ǵ������λ��
		}
		
	}
}

//*****************************************************************/
//��������趨λ�þ��룬���ݹ��ܲ�ͬ���趨
//Motor_uion *t ��������ṹ�壬Status_uion ״̬�趨�ṹ��
//*******************************************************************/
void Motor_set_position(Motor_uion *t, Status_uion *p)
{
	//Ŀ���趨λ�õĻ�ȡ��Slet_position���
	t->SumCountSet = Slet_position(t->select, p->Work_index-1, p->Comsum_set, p->Auto_clear);

	if(t->select == 0)    //���ǶԷ�ˮ�����λ�ÿ��� 
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
	else			//��˵�����ٶȿ���
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
					t->Index += Reciprocate_add;   //���򲹳�
				}
				t->cw_f = 0;
			}
			else
			{
				if(t->cw_f==0)
				{
					t->Index -= Reciprocate_add;  //���򲹳�
				}
				t->cw_f = 1;		
			}
		}
	}
}


/*****************************************************************/
//    �����˶�����ģ��
//˵������˵���������˶����ͷ�ˮ�����ˮѹ��Ħ����������Ľṹ��ѡ��
//���ܣ�ѭ����ģʽװ�����ṹ�壬��ִ��
//�˹��ܶ�Ӧ ��ϴ��Ů��
//��ˮ����� ǰһ��״̬Ӧ��0��ˮѹ
/*******************************************************************/
void Reciprocate_fun(Motor_uion *t, Status_uion *p)
{	
	uint_16 Now_temp = 0;     //����һ����ǰ���趨��Ŀ��λ��
	uint_16 Dis_temp = 0;		//����һ����Ҫ�����Ŀ��λ��
										
	Now_temp = Slet_position(t->select, p->Work_index-1, p->Comsum_set, p->Auto_clear);			//��ֵ��ǰ״̬��Ŀ��λ��
	Dis_temp = Slet_position(t->select, p->Work_index-1, (p->Comsum_set + 2), p->Auto_clear);	            //�趨�����˶���Ŀ��λ��

	/************************************************/
	//�����ǶԽ��������˶�ǰ��λ���ж�

	//����ǰλ��С��ǰ״̬��Ŀ��λ��
	if(((t->Index <= Now_temp - WangFu_2mm) && (t->select == 1)) ||		//������˵��ѡ��
		((t->Index <= Now_temp ) && (t->select == 0)))					//���Ƿ�ˮ���ѡ��
	{
		t->SumCountSet = Dis_temp;   //λ�ø�ֵ�����Ŀ��λ�ñ���
		if((t->cw_f == 0) && (t->select == 1))
		{
			t->Index -= Reciprocate_add;	//��˵����ֵ��������
		}		
		t->cw_f = 1;			//��ת
		t->enable_f = 1;		//��ʼ����
		t->end_f = 0;	
	}
	//����ǰλ�ô���ǰ״̬��Ŀ��λ��
	if(((t->Index >= (Dis_temp)) && (t->select == 1)) ||
		((t->Index >= Dis_temp) && (t->select == 0)))
	{
	
		if((t->cw_f == 1) && (t->select == 1))		//����˵�����в���
		{
			t->SumCountSet = Now_temp - WangFu_2mm;
			t->Index += Reciprocate_add;
		}
		else				//�Է�ˮ������в���
		{
			t->SumCountSet = Now_temp;
		}
		t->cw_f = 0;
		t->enable_f = 1;
		t->end_f = 0;
	}
	//����ǰλ�õ���ǰ״̬��Ŀ��λ��
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
//		�����ϴ��������ģ��
//���룺����ṹ�壬�͹��ܱ�־λ�ṹ��
//
/***************************************************************/
void Motor_work_prosess(Motor_uion **t, Status_uion *s)
{	
	Motor_uion *p = *t;	  //���ﶨ��һ���ɱ����ṹ�壬������Ŵ������ĵ���ṹ��
	//��Ĭ�� p ���� ��ˮ���   p+1���� ��˵��
	switch(work.step)
	{
		case 0:
			s->Work_index = s->Mode_select;
			s->Comsum_set = s->Pres_select;   //�����Ȱ��趨�õ�ֵ��ֵ������������������
			work.steptime = 0;  //��������ʱ���ʱ 
			if ((p+1)->enable_f == 0)
			{
				s->QiBeng_f = 0;
				if (s->QingXiQieHuan_f == 1)
				{
					work.step = 5;		//�������ϴ�л�  ��������һ��
				}
				else
				{
					work.step++;
				}
			}
			break;
		case 1:
			
			Motor_set_position(p,s);    //�����Ƿ�ˮ������÷�ˮ��λ��
			work.step++;
			break;
		case 2:
			//�ȴ���ת��λ
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				work.step++;
			}
			break;
		case 3:
			//-----���ý�ˮ
			Water.WaterIn_f = 1;   	
			Water.Hot_f = 1;
			work.steptime = 0;
			work.step++;
			break;
		case 4://�Խ�2��
			if(++work.steptime > 150)
			{
				work.steptime = 0;
				work.step ++;
			}
			break;
		case 5:  //�������л�����ʱ���л����Խ��
			s->Auto_clear = 1;
			s->Comsum_set = CskZJ;  //ѡ��ˮѹ��λ�Խ൵λ
			Motor_set_position(p,s);
			work.step++;
			break;
		case 6:
			//�ȴ���ˮ����ת��λ
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;   //��ϴ��־λ��0
				Water.WaterIn_f = 1;
				Water.Hot_f = 1;   //��ˮ����
				work.step++;
			}
			break;
		case 7:
			if(s->QingXiQieHuan_f == 1)  //������л���ϴ������
			{
				work.step++;     //ֱ��������һ��
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
			//************�����Ƿ�ˮ�ڳ�ʼ��********************/
		case 8:
			s->Comsum_set = s->Pres_select;  //ѡ��λ�û�ԭΪ�趨ֵ�� Index �� cumsum �������� ����������õ�
			Motor_set_position((p+1),s);   //����ڵ����ʼ��
			work.step++;
		break;
		case 9:
			//---------�ȴ������˵�λ			
			if((p+1)->end_f == 1)
			{
				(p+1)->end_f = 0;
				(p+1)->enable_f = 0;
				work.step++;
			}
			break;
		case 10: //����Ҫת�����λ������ϴ2�����ת����Ӧ��λ����ֹ��λ��С������ģ��ֹͣ����
			s->Auto_clear = 2;   		//������ѡ����ϴ
			s->Comsum_set = 4; 		 	//ѡ��ˮѹ��λΪ���
			Motor_set_position(p,s);    //�����Ƿ�ˮ������÷�ˮ��λ��
			Water.WaterIn_f = 1;
			Water.Hot_f = 1;
			work.step++;
			break;
		case 11:   //�ȴ���ˮ����λ
			//work.time++;
			if(p->end_f==1)
			{
				work.steptime = 0;
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;				//��ϴѡ���0
				s->Comsum_set = s->Pres_select;  //ѡ��λ�û�ԭΪ�趨ֵ
				work.Start_f = 1;
				work.time = Water.WashTimer;
				work.step++;
			}
			break;
		case 12:  //��ϴ2��
			if(++work.steptime > 200)
			{
				work.steptime = 0;
				work.step++;
			}			
			break;
		case 13:  //��ϴ֮���ת����Ӧ�Ĺ��ܿڽ�����ϴ		
			s->Comsum_set = s->Pres_select;  //ѡ��λ�û�ԭΪ�趨ֵ
			Motor_set_position(p,s);    	//�����Ƿ�ˮ������÷�ˮ��λ��
			work.step++;
		break;
		
		case 14: //�ȴ���ת��Ӧ�Ĺ��ܿ�λ��
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->QiBeng_f = 1;   //���￪������
				s->Working_f = 1;   //���ڹ�����־λ��1
				work.step++;
			}
		break;
		//****************************************************************************************************/
		// ----------��������Ӧ�Ĺ�����ϴ ��ˮ�������˵����Ӧ��ִ�в���-------------//


		case 15:   //���λ��΢����ˮѹ΢���������˶���������
			//********ˮѹ΢��*******//
			if(Water.pressure_f == 1)   
			{
				Water.pressure_f = 0;
				s->Comsum_set = s->Icon.water_yeild;   //ˮ����λ��ֵ��ѡ��λ��ѡ��ˮ���Ĵ�С
				Motor_set_position(p,s);    //�����Ƿ�ˮ�������ˮѹλ��
				if(p->end_f == 1)
				{
					p->end_f = 0;
					p->enable_f = 0;
					IR.Last_value = s->Comsum_set;     //����ε�ֵ�����������������ˮ�Ƶ�ʱ��λ��Ҳһ����ڵ�����
				}
				
			}
			//*******���΢��**********//
			if(PenGan.PGmove_f == 1)   
			{					
				s->Comsum_set = s->PG_gear;   //��˵�λ��ֵ��ѡ��λ��ѡ����˵�λ��
				Motor_set_position((p+1),s);    //��������˵���������λ��
				if((p+1)->end_f == 1)
				{
					(p+1)->end_f = 0;
					(p+1)->enable_f = 0;
					PenGan.PGmove_f = 0;     //�ر����΢��
				}
				
			}			
			//********�����˶�********//
			if(s->reciprocate_f == 1)   
			{						
				Reciprocate_fun((p+1),s);   //��˵�����������˶�				
			}
			//********ˮѹ��Ħ**********//
			if(s->ShuiYaAnMo_f == 1)			
			{
				if(++s->ShuiYaAnMo_timer < 150)  //1.5S ��
				{
					s->QiBeng_f = 1;  
				}
				else if((++s->ShuiYaAnMo_timer > 150) && (++s->ShuiYaAnMo_timer < 300))  //1.5 S ��
				{
					s->QiBeng_f = 0;
				}
				else
				{
					s->ShuiYaAnMo_timer = 0;
				}
			}
			else		//���û������������ѹ
			{
				s->ShuiYaAnMo_timer = 0;
				s->QiBeng_f = 1;
			}
			Water.WaterIn_f = 1;
			Water.Hot_f = 1;
			
			if(work.time == 0) //��ϴʱ�����
			{
				s->Mode_select = 0;		//ѡ����0
				s->ShuiYaAnMo_f = 0;	// ��Ħ��־λ��0
				s->reciprocate_f = 0;	//�����˶���0

				s->Comsum_set = 0;  //ѡ��Ŀ��λ�û�ԭΪ�趨ֵ��
				s->Work_index = 0;  //ѡ��λ�û�ԭΪ�趨ֵ������������̲���
				
				work.Start_f = 0;
				work.step = 0;
			}
		break;
		/*****************************************************/
		//**********�����ǰ�Ħ��ز�������***********/		
	}
}

//****************************************************************/
//		�����ϴ���ܽ�������ģ��
//���룺����ṹ�壬�͹��ܱ�־λ�ṹ��
//
//***************************************************************/
void Motor_work_end(Motor_uion **t, Status_uion *s)
{

	Motor_uion *p = *t;	  //���ﶨ��һ���ɱ����ṹ�壬������Ŵ������ĵ���ṹ��
	//Ĭ�� p �Ƿ�ˮ���            p+1����˵�� ��Ӧ�Ľṹ��
	s->QiBeng_f = 0;
	switch (work.step)
	{
	   //---��ˮ�������Խ��
		case 0:	
			s->Auto_clear = 1;		//ѡ���Խ��־λ
			s->Comsum_set = CskZJ;  //ѡ��λ��Ϊ�Խ��
			Water.Hot_f = 0;		//�ر���ˮ����
			Motor_set_position(p,s);    //�����Ƿ�ˮ�������
			work.step++;
		break;
			
		case 1:  //-------�ȴ���ת��λ
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;
				work.steptime = 0;
				work.step++;
			}
		break;

				//���������Сλ��
		case 2:
			s->Auto_clear = 1;			//ѡ����Сλ��
			s->Comsum_set = SSMin;    //ѡ��λ��Ϊ��Сλ��
			Motor_set_position((p+1),s);    //��������˵������
			work.step++;			
		break;

		case 3:
				//-------�ȴ������˵�λ			
			if((p+1)->end_f == 1)
			{
				(p+1)->end_f = 0;
				s->Auto_clear = 0;
				(p+1)->enable_f = 0;	//�رյ��ʹ��
				s->PG_zero_f = 1;     //����ջص�λ
				work.step++;
				work.steptime = 0;
			}
		break;

		case 4://-------�Խ�2��
			if(++ work.steptime >= 200)
			{
				work.steptime = 0;
				work.step++;
			}
		break;
		case 5://�Խ��ȴ�1.5S  �ȴ���ˮ�ų�
			Water.WaterIn_f = 0;  //�رս�ˮ
			Water.Hot_f = 0;		//�رռ���
			if(++work.steptime > 150)
			{
				work.steptime = 0;
				work.step++;
			}
		break;
			//----һ�������Ӹ�λ���ܣ���λ֮���ٴ���
		case 6:
			s->Auto_clear = 1;
			s->Comsum_set = CskMin;
			s->Work_index += 50;
			Motor_set_position(p,s);    //�����Ƿ�ˮ�������

			s->Comsum_set = SSRestIndex;
			s->Work_index = s->Mode_select;
			Motor_set_position((p+1),s);    //��������˵������
			work.step++;
		break;

		case 7:
				//--------------------�ȴ�����	
			if((p->end_f==1)&&((p+1)->end_f==1))
			{
				p->end_f=0;
				p->enable_f=0;
				(p+1)->end_f=0;
				(p+1)->enable_f=0;
				work.step++;
			}
		break;

		case 8://----�ָ�������״̬
			s->Comsum_set = CskZJ;  //ѡ���Խ��
			Motor_set_position(p,s);    //�����Ƿ�ˮ�������
			work.step++;
		break;

		case 9://----�ȴ���λ
			if(p->end_f == 1)
			{
				p->end_f = 0;
				p->enable_f = 0;
				s->Auto_clear = 0;
				s->Comsum_set = 0;  //ѡ��λ�û�ԭΪ�趨ֵ�� 
				work.steptime = 0;
				work.step++;
			}
		break;

		case 10: //-------���̽���
			work.enable_f = 0;  
			s->PG_zero_f = 1;    //��˹���������س�ʼλ�ñ�־λ
			s->Divid_water_f = 1;  //��ˮ����������
			if(work.LianXu_f == 1)  //���ʹ������������
			{
				work.time = Drying.WorkingTimer;  //������ϴ��Ϳ�ʼ����������
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
//  ���������������	
//***********************/
void Motor_A_B_C_program(Motor_uion **P, Status_uion *S)
{
	if(work.enable_f == 1)	
	{
			if(Status.Mode_select != 0)
			{
				//-------------------���ܴ���
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
  �����ϴ��������	
//***********************/
void Spout_clean(Motor_uion **t, Status_uion *s)
{
	uint_8 Index = 0; //����һ����������
	uint_8 Comsum = 0; //����һ��Ŀ��λ�ñ���

	Motor_uion *p = *t;	  //���ﶨ��һ������ṹ�������������Ŵ������ĵ���ṹ��

	if(penkou.enable_f == 1)   //�������ɽ��������ϴ
	{
		CoverSeat.delay = T5Min;
		if(s->penkou_f == 1)
		{
			switch(penkou.step)
			{
				case 0:
					penkou.delay1 = 0;   //----����ʱ���ʱ��0
					penkou.delay2 = 0;
					penkou.step++;
				break;
				case 1:
					Water.WaterIn_f = 1;  //---��ˮ
					Water.Hot_f = 1;    //--����ˮ
					penkou.step++;
				break;
				case 2:
					if(++penkou.delay1 > 300)    //--�Խ�3��
					{
						penkou.delay1 = 0;
						penkou.step++;
					}
				break;
				case 3:   //  1 5 0
					s->Auto_clear = 0;
					s->Work_index = 1;              
					s->Comsum_set = 5;    			//����������
					Motor_set_position((p+1),s);    //��������˵������Ϊ�����ϴλ��
					penkou.step++;	
				break;
				case 4:
					if((p+1)->end_f==1)   //�ȴ�����λ��
					{				
						(p+1)->end_f=0;
						(p+1)->enable_f=0;
						penkou.delay2 = 12000;
						penkou.step++;
					}
				break;
				case 5:
					if(--penkou.delay2 < 0)    //��ϴ2���� 12000 * 10ms /1000ms = 120S
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
					Motor_set_position(p,s);    //�����Ƿ�ˮ�������

					s->Comsum_set = SSMin;
					Motor_set_position((p+1),s);    //��������˵������
					penkou.step++;
				break;
				case 3:
					//--------------------�ȴ�����	
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
					Motor_set_position((p+1),s);    //��������˵������
					penkou.step++;
				break;

				case 5:
					if((p+1)->end_f == 1)
					{
						(p+1)->end_f = 0;
						s->Auto_clear = 0;
						(p+1)->enable_f = 0;	//�رյ��ʹ��					
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












