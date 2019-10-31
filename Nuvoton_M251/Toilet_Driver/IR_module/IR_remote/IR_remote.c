#include "IR_remote.h"


Remote_IO_cfg Remote_io;

#define	IR_DataSize		6
#define	User_DataSize	5

unsigned char	IR_databuffer[IR_DataSize];
unsigned char	User_databuffer[User_DataSize];

//����һ��ˮ�������¡�ˮ�¡�����ö??
Gear Water_yeild,Water_temp,Seat_temp,Wind_temp;
//����ö��
Func Func_select;




void Remote_IO_init(Remote_IO_cfg *p,void (*Remote_set_h)(),void (*Remote_set_l)(),uint_16 (* check_pin)())
{

	p->Remote_set_high = Remote_set_h;
	p->Remote_set_low  = Remote_set_l;
	p->Check_pin = check_pin;
}





//********************************************************/
//  ������մ��룬���ڶ�ʱ���ж� 
// һ֡��������+ ��ʼ��+ �������� + У��   �ֽڣ������������룩
// 0 : 640us�͵� + 480us �ߵ�ƽ��     1 : 640us�͵� + 1440us�ߵ�ƽ��
//************************************************************/
void IR_receive(void)
{
	switch(IR.step)
	{
		case 0:
			//----���յ��͵�ƽ�źſ�ʼ��ʱ
			if(Remote_io.Check_pin == 0)
			{
				IR.start_f = 1;
				IR.time_h = 0;
				IR.time_l = 0;	
				IR.step++;
			}
			break;
		case 1:	
			if(Remote_io.Check_pin  == 0)
			{
				IR.time_l++;	
				if(IR.time_l > 25)		//---25 * 200= 5000us = 5ms ����͵�ƽ��ʱ�����5ms��֤������������
				{
					IR.step = 0;		//--IR_cuowu();
				}				
			}
			else  //��ʱ���˸ߵ�ƽ����Ҫȥ�ж�ǰ��͵�ƽ��ʱ�䳤���ǲ����������ʱ�� 
			{
				if(IR.time_l < 15)		//����͵�ƽʱ�䲻??ms ֤����������??
				{
					IR.step = 0;		//--IR_cuowu();
				}		
				else	//����ﵽ�ˣ�֤����������ĵ͵�ƽ
				{
					IR.step++;
				}
			}
			break;
		case 2:
			if(Remote_io.Check_pin  == 0)  //��������ߵ�ƽ����һ���͵�ƽ
			{
				if(IR.time_h < 35)		//�жϸղŵĸߵ�ƽ�Ƿ�����������ĸߵ�ƽʱ?? С�� 8ms֤������
				{
					IR.step = 0;		//--IR_cuowu();
				}
				else //�����������ȷ����ʼ�����ֽڽ�??
				{
					IR.step++;	
					IR.first_f = 0;
					IR.Bit_Count = 0;
					IR.Byte_Count = 0;
					IR.already_f = 0;
					IR.time_l = 1;
					IR.time_h = 0;		
				}
			}
			else	//������Ǽ��������ղŵĸߵ�ƽʱ��
			{
				IR.time_h++;		//������ʱ
				if(IR.time_h > 45)			//--�����ʱ���� 9ms ֤����������??
				{
					IR.step = 0;			//---IR_cuowu();
				}
			}
			break;
		case 3:
			//----------------------bit0
			if(Remote_io.Check_pin  == 0)  //��ʱ���ڵ͵�ƽ��ʱ��??
			{
				IR.time_l++;   //������ʱ	
			}
			else		//������˵�һ���ֽڵĸߵ�ƽ���ߵ�ƽʱ���1��ʼ���������
			{
				IR.time_h = 1;
				IR.step++;
			}
			/*****���Ͼ���bit0  0x11�Ľ�??��Ϊ��������ȷ��ֱ�ӽ����ֽڽ���******/
			break;
			//--------------------�ڴ��жϵ�һ��BIT
		case 4:
			if(Remote_io.Check_pin  == 0)  //���ﵽ��BIT1�Ľ��գ����жϵ�һ����??
			{
				IR.time_l++;    //һ���ֽ���??40us�͵�?? ??80us / 1440us���ĸߵ�ƽ��??
				if(IR.time_l > 4)					//����800us�Ͷ�??
				{
					IR.step = 0;				//---IR_cuowu();
				}
				//---------------				
				if(IR.first_f == 0)
				{
					if(IR.time_h >= 7)			//---200 * 7 = 1400us �������֤�� ����ֽ�?? 
					{
						//	IR.Recv_Data_Buf>>=1;
						//	IR.Recv_Data_Buf&=0x7f;
						IR.Recv_Data_Buf <<= 1;			
						IR.Recv_Data_Buf |= 0x01;	   //��¼�ֽ�
						IR.first_f = 1;
						IR.Bit_Count++;
					}

					else if((IR.time_h >= 2)&&(IR.time_h <= 3))		//----??400us ??600us֮����� 0 ����??
					{
						//	IR.Recv_Data_Buf>>=1;			
						//	IR.Recv_Data_Buf|= 0x80;
						IR.Recv_Data_Buf <<= 1;
						IR.Recv_Data_Buf &= 0xfe;
						IR.first_f = 1;
						IR.Bit_Count++;
					}
					else
					//-------ʱ�䶼������������Χ��֤�����ݴ�?? ����
					{
						IR.step = 0;			//---IR_cuowu();
					}
				}
				IR.time_h = 0;	//��¼�ֽ�???? �ȴ���һ�ν�??
				//------------ ------------------
				if(IR.Bit_Count >= 8)  //��¼8��λ�󣬼����յ�һ����??
				{
					IR.Bit_Count = 0;	
					IR_databuffer[IR.Byte_Count] = IR.Recv_Data_Buf;
					IR.Byte_Count++;
					if(IR.Byte_Count >= 6)	//����??���ֽں����
					{
						IR.step++;	
						IR.already_f = 1;						
					}		
				}
					
			}
			else  //�ж��Ƿ�֡
			{
				IR.time_h++;
				if(IR.time_h > 8)
				{
					IR.step = 0;			//----IR_cuowu();
				}
				IR.first_f = 0;
				IR.time_l = 0;
			}
			break;
		case 5:
			//--------------�жϽ���??
			IR.step = 0;
			break;
		default:
			break;
	}
}

//***************************************/
//void Func_deal(uint_8 select)
//	����ģ��ѡ����
//  ����ĳɺ���ָ��������ʽ�����Ч??
//***************************************/
/*
void Func_deal(uint_8 select)
{
	switch (Func_select)
	{
		case Stop:    		//ֹͣ����
			Close_all_function();
		break;
		
		case Hip_clean:		//��??
			Hip_Wash_Enable();
		break;
		
		case Auto_s:		//�Զ�����

		break;
		
		case Wem_clean:		//��ϴ
			Wemen_Wash_Enable();
		break;
		case Dring:  		//��ɹ���ģ��
			if(adc_set.Err == 0)
			{
				Dring_Work_Enable();
			}
		   
		break;
			
		case Purge:			//ͨ��  ��Ŀǰ�ĳ�����spa??
			if(Status.Mode_select != 0 && Status.LengReAnMo_f == 0)
			{
				Status.LengReAnMo_f = 1;
			}
		break;
			
		case Massage:		//��Ħ
			if(Status.reciprocate_f == 0)   //��һ�ο�����
			{
				if(Status.Mode_select != 0 )   //ֻ����ѡ��ģʽ�²��ܽ���������ϴ
				{
					Status.reciprocate_f = 1;
				}				
			}
			//else  						//��һ�ΰ���Ϊ�ر�
			//{
			//	Status.reciprocate_f = 0; 
			//}
		break;
			
		case Turn_cover:	//����
				Cover_status_change();
		break;
		
		case Deodorzation:	//����
				if(Status.chuchou_f == 0)
				{
					Status.chuchou_f = 1;
				}
		break;
				
		case Auto_clean:	//����Խ�
			if((penkou.enable_f == 0) && (Status.seat_f == 0)) //ֻ�����������������ϴ��??
			{
				penkou.enable_f = 1;
				Status.penkou_f = 1;
				penkou.step = 0;
			}
		break;
			
		case Flush:			//��ˮ
			if(Status.chongshuiban_f == 0)
			{
				Status.chongshuiban_f = 1;
			}
			
		break;
			
		case Turn_circle:	//��Ȧ
				Circle_status_change();
		break;	
		
		case Power:			//��Դ
			//Power_Switch_deal();
			if(Status.Power_e == 0)
			{
				Status.Power_e = 1;
				Status.Pwr.step = 0;
			}
		break;
			
		case Auto_flush:	//�Զ���ˮ
			if(Status.auto_f == 0)
			{
				Status.auto_f = 1;
			}
			
		break;	
	}
}
*/



/***************************************/
//
//	���벢���й��ܴ���
//
//***************************************
void IR_Opration(void)
{
	uint_8	i,j;
	uint_8	temp1,temp2,temp3;
	uint_8	checksum;
	uint_8	TempBuf;	
	uint_8	DringFlag;
	if(IR.already_f)
	{	
	//--------�жϽ��յ��������Ƿ�����
	//--------ת�����ݶ�ȡ��ʽ
	//--------�ȷ����ȡ���ݣ�
		for(i=0; i<IR_DataSize; i++ )
		{
			temp1 = IR_databuffer[i];  //ѭ����ֵ���ݵ�ÿһ���ֽڸ� temp1
			for(j=0; j<8; j++)
			{
				if((temp1 & 0x80) == 0)
				{
					temp2 >>= 1;
					temp2 &= 0x7f;
				}
				else
				{
					temp2 >>= 1;
					temp2 |= 0x80;
				}
				temp1 <<= 1;
			}
			IR_databuffer[i] = temp2;
		}
		checksum = 0;
		for(i=1; i<(IR_DataSize-1); i++) //�ۼ�??����??
		{
			checksum += IR_databuffer[i];
		}	
		//����ÿ���ط����Σ�������Ҫ���������ظ���Ӧ2�Σ��������ж�����У��λ�Ƿ����			
		if(checksum == IR_databuffer[IR_DataSize-1])  //�жϵ�һ֡���ݵ�У����Ƿ���??
		{
			if(IR.TFDelay == 0)   //��һ�ν�����ʱ���Ȱ�������0
			{
				for(i=0; i<(IR_DataSize-1); i++)
				{
					User_databuffer[i] = 0;
				}
			}
			else
			{
				//����ڶ��ν�����ʱ��ʱ�仹????��ֱ�ӽ���������ж�
				for(i=0; i<(IR_DataSize-1); i++)  //Ȼ���жϽ��յ��������Ƿ���ϴε�һ??
				{
					if(User_databuffer[i] != IR_databuffer[i])  //ֻ�к͵�һ�εĲ�һ���˲Ž��빦��ִ�У�������Ǹ��ϴ�һ����֤�����ط��ģ��ǾͲ�ִ��
					{
						IR.OK_f = 1;					
					}
				}
			}
			IR.TFDelay = 15;
		}

		if(IR.OK_f == 1)
		{
			IR.OK_f = 0;
			for(i=0;i<IR_DataSize;i++)
			{
				User_databuffer[i]=IR_databuffer[i+1];
			}
			IR.already_f=0;
			buz_set(1,20,20);				//��һ??
			//------------------------------��RF_DataBuffer[]�����ݽ��д�??
			//------------------------------�жϽ��������Ƿ���ȷ
			
			if(Status.PowerEnd_f == 1)  //����Ѿ��Ѿ��ϵ����
			{
				SoftPowerDeal.Delay = T10Min;
				SoftPowerDeal.Softpower_f = 1;
				Func_select = User_databuffer[0]; //����ѡ��
				temp2 = User_databuffer[2];
				temp3 = User_databuffer[3];
				if((Func_select&0x19) == 0x19) //������ڲ�������??
				{
					/*****�������¶ȵ�λ��??****/			
					//Status.Icon.water_temp_gear = Gear_deal(User_databuffer[1]>>4); //�����ˮ��
					//Status.Icon.seat_gear = Gear_deal(User_databuffer[2]);    //���������
					//Status.Icon.wind_gear = Gear_deal(User_databuffer[3]);    //����Ƿ���
					
					if((temp3 &0x80) == 0x80)  //����ҹ��			
					{
						//xiaobianled_set;
					}
					else
					{
						//xiaobianled_clr;
					}	
					
					if((temp2 & 0x80) == 0x80)       //����ͼ��
					{
						Status.Icon.energy_disp = 1;
					}
					else
					{
						Status.Icon.energy_disp = 0;
					}		
				}
				else if((Func_select&0x18) == 0x18) //�������ϴ��??
				{
						
					if((IR.Last_value == (User_databuffer[3]>>4) & 0x0f))   //ͨ��ˮ�����жϵ�ǰң�ط���������ˮ�����ڻ���λ�õ���
					{
						//Spraylance_move_deal(User_databuffer[2]);   //λ�õ���	
					}
					else
					{
						Status.Icon.water_yeild = (User_databuffer[3]>>4) & 0x0f;
						//Water_change(Status.Icon.water_yeild);//�����ִ��ˮ�����ں�??
			
					}
					
				}
				else	//���ֻ�ǹ���ѡ��
				{
					//Func_deal(Func_select&0x0f);    //����ѡ����	
				}
			}
		}		
	}
}







