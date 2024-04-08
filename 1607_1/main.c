# include "bsp_init.h"

# include "bsp_seg.h"
# include "timer.h"
# include "bsp_key.h"
# include "bsp_led.h"

#include "bsp_onewire.h"

# include "stdio.h"


#define  PWM_OUT	P34 //define����ǰ�ߵĴ����ߵ�

//-----------------------------------------------
/* �������� */
//��������ѭ������������Զ����
void Key_Proc(void);//���������ײ����ݱ��
void Seg_Proc(void);//��ʾ������ʾ��Ϣ����
void Led_Proc(void);//LED����LED״̬��Ϣ��ʾ


//-----------------------------------------------
/* ȫ�ֱ������� */

//�������ʾר�ã���������Զ����
unsigned char seg_buf[8];//�����ַ���ת����Ķ��뵽����
unsigned char seg_string[10];//�����ַ���
unsigned char	pos = 0;//�ж���ʾר��

//LED��ʾר�ã���������Զ����
unsigned char ucLed;//��¼LED��״̬��Ϣ

//����ר�ã���������Զ����
unsigned char Key_Value;//��ȡ��������ֵ�洢����
unsigned char Key_Down, Key_Old;//��ȡ��������ֵ�洢����	

//��������ʾ��������ר�ã�������Զ����
unsigned int Key_Slow_Down;//��������
unsigned int Seg_Slow_Down;//��������


//���ݴ����������仯
unsigned char Running_Mode = 0; //����ģʽ,��ʼֵΪģʽ0 
unsigned char Countdown_Sec = 0; //����ʱ�룬��ʼֵΪģʽ0 
bit Temp_Mode = 0;//�¶���ʾģʽ��1ʱ���¶���ʾģʽ����Ϊ0ʱ��������ʾģʽ��

unsigned char Count_100us = 0; //��¼�ڼ��ν��붨ʱ��0��100us�ж�
unsigned char Count_PWM = 0; //Ϊ����PWM��������count
unsigned int Count_1ms = 0; //��¼�ڼ��ν��붨ʱ��1��1ms�ж�

unsigned char S5_Countdown_Sec_Sluggish = 0; //��S5����ȥ֮����ݼ���������ı�����
unsigned int temp;
//-----------------------------------------------
/* main */
void main()
{
	//���ݴ����������仯
	Cls_Peripheral();//�ر�����
	Timer1Init();//��ʱ��1��ʼ��������ʹ�ܶ�ʱ��1�жϣ�1ms����һ��
  Timer0Init();//��ʱ��0��ʼ����	100us����һ���жϺ�����
	EA = 1;//�����ж�


	while(1)
	{
		//��Զ����
		Key_Proc();//���������ײ����ݱ��
		Seg_Proc();//��ʾ������ʾ��Ϣ����
    Led_Proc();//LED����LED״̬��Ϣ��ʾ		
	}


}



//-----------------------------------------------
/* Timer1_interrupt routine 1000us*/
void tm1_isr() interrupt 3
{	
	
	//��Զ����
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;
	

	if(++Count_1ms == 1000) //�붨ʱ
	{
		Count_1ms = 0;
		if(S5_Countdown_Sec_Sluggish) S5_Countdown_Sec_Sluggish--;//������Ч����
		else if( (--Countdown_Sec) > 200) Countdown_Sec = 0;//���ȶ�ʱ�Զ��ݼ�
	}
	
	

	//��Զ����
	Seg_Disp(seg_buf, pos);//�������ʾˢ��
	if( ++pos == 8 ) 	pos = 0;
	

	
	
}


//-----------------------------------------------
/* Timer0_interrupt routine 100us*/
void tm0_isr() interrupt 1
{	
	
	if(Countdown_Sec)
	{
		
		
			if( ++Count_100us== 10) 
			{ 
				Count_100us=0;//�ü�¼�ڼ��εı�������0-9֮��仯��
				PWM_OUT = 1;//ÿ10������һ��
				ucLed |= 0xF8;//���5λ��1�����������
			}
			
			switch(Running_Mode)
			{
				case 0:	Count_PWM = 2; break;
				case 1: Count_PWM = 3; break;
				case 2: Count_PWM = 7; break;
			}
			
			if(Count_100us == Count_PWM) 
			{
				PWM_OUT = 0;//��ģʽ�涨��״̬λ�ã�����һ�Ρ�
				ucLed &= 0x07;//���5λ��0�����������	
			}
		}
	else
	{
				PWM_OUT = 0;//�ظ�����λ״̬��
				ucLed &= 0x07;//���5λ��0�����������			
	}
	
	Led_Disp(ucLed);//LED��ʾ	
	
}




//-----------------------------------------------
/*key_proc */

void Key_Proc(void)//���������ײ����ݱ��
{
	//��Զ����
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//���ٳ���
	
	Key_Value = Key_Read();//��ȡ�������µı��
	Key_Down = Key_Value & (Key_Old ^ Key_Value);// (0000^0101) = 0101     0101 & 0101 =0101  ,��������������½��صı仯���������ͱ��ΰ�����ֵ��ͬ
																					// (0101^0101) = 0000       0101&0000 = 0000  �������һֱ����ͬ����״̬��������Ϊ0
	Key_Old = Key_Value;
	
	
	
	
	//���ݴ����������仯
	switch(Key_Down)
	{
		case 4://ģʽ�л���ť
		  if ( ++Running_Mode == 3)  Running_Mode = 0;//����ģʽ��0-2֮���л�
			break;
			
		case 5://����ʱ���л���ť
			Countdown_Sec+=60;//����ʱ����ÿ��һ��S5Ҫ�Զ�����60s
			if((Countdown_Sec>=60)&(Countdown_Sec<120)) Countdown_Sec = 60;//��ϵͳ����ʱ��0-60�룬����60֮�󣬱��60-120��ǿ�ƽ�����ʱ��λΪ60
			if((Countdown_Sec>=120)&(Countdown_Sec<180)) Countdown_Sec = 120;//��ϵͳ����ʱ��60-120�룬����60֮�󣬱��120-180��ǿ�ƽ�����ʱ��λΪ120
			if(Countdown_Sec ==180) Countdown_Sec = 0;//��120s��ʱ�л���0��ʱ
			S5_Countdown_Sec_Sluggish = 1;//����ݼ����͵�ʱ��
			break;	
		
		case 9:	//ֹͣ��ť
			Countdown_Sec = 0;//������ʱ������
			break;	

			
		case 8:
			Temp_Mode ^= 1;//0-1�ı仯
			break;		

	}
	
	
	
}



//----------------------------------------------
/*seg_proc */
void Seg_Proc(void)//��ʾ������ʾ��Ϣ����
{
	//��Զ����
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//���ٳ���

	
	
	//���ݴ����������仯
  if(Temp_Mode == 0)
		sprintf(seg_string,"-%1d- %04d",(unsigned int)(Running_Mode+1),(unsigned int)Countdown_Sec);
	else 
	{
		temp = rd_temperature()>>4;
		if(temp < 50)
		sprintf(seg_string,"-4-  %02dC",temp);		
	}
	//��Զ����
	Seg_Tran(seg_string, seg_buf);
}

//----------------------------------------------
/*led_proc */
void Led_Proc(void)//��ʾ������ʾ��Ϣ����
{
	
	//���ݴ����������仯
	if(Countdown_Sec)
	{		
	
		switch(Running_Mode)
		{
			case 0:	ucLed |= 0x01; ucLed &= (~0x06); break;//����L1������ͬʱ��Ҫ��L2��L3�رգ�L4��L8��Ӱ��
			case 1: ucLed |= 0x02; ucLed &= (~0x05); break;//����L2������ͬʱ��Ҫ��L1��L3�رգ�L4��L8��Ӱ��
			case 2: ucLed |= 0x04; ucLed &= (~0x03); break;//����L3������ͬʱ��Ҫ��L1��L2�رգ�L4��L8��Ӱ��  
		}
	}
	else 
	{
		ucLed &= (~0x07);
	}
		
		
			
}


