# include "bsp_init.h"

# include "bsp_seg.h"
# include "timer.h"
# include "bsp_key.h"
# include "bsp_led.h"

#include "bsp_iic.h"

# include "stdio.h"
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

//�δ�ʱר��
unsigned long ms_Tick=0;//�ϵ�֮���һֱ������ȥ ��49��Ż������� 

//���ݴ����������仯
bit Vol_Fre_Disp_Mode = 0;//0-��ѹ����ģʽ��1-Ƶ�ʲ���ģʽ
unsigned int Frequent = 0;//Ƶ��ֵ

bit Seg_Dis_or_Die = 1;//�������������0-��1-��
bit LED_Dis_or_Die = 1;//����LED����0-��1-��

bit DAC_Output_Ctrl = 0;//����DAC�����0-�̶�2v��1-����Rb2�仯��ģʽ
float AD_Num = 0;//ָʾ��ר�õ�AD��ȡ����ֵ	
	
	
//-----------------------------------------------
/* main */
void main()
{
	//���ݴ����������仯
	Cls_Peripheral();//�ر�����
	Timer1Init();//��ʱ��1��ʼ��������ʹ�ܶ�ʱ��1�жϣ�1ms����һ��
	EA = 1;//�����ж�

	Timer0Init_Count();//��ʱ��0���ó��ⲿ����ģʽ�����ⲿ�������Ƶ�ʼ���

	while(1)
	{
		//��Զ����
		Key_Proc();//���������ײ����ݱ��
		Seg_Proc();//��ʾ������ʾ��Ϣ����
    Led_Proc();//LED����LED״̬��Ϣ��ʾ		
		
	
	}


}



//-----------------------------------------------
/* Timer1_interrupt routine */
void tm1_isr() interrupt 3
{
	ms_Tick++;//�ϵ��Զ�++���������� 
		
	
	//��Զ����
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;
	
	//ȡʱ����
	if(!(ms_Tick%1000)) //ÿ1s����
	{
		Frequent = ((TH0<<8)|TL0);
		TL0 = 0;		//���ö�ʱ��ֵ
		TH0 = 0;		//���ö�ʱ��ֵ	
	}
	




	//��Զ����
	Seg_Disp(seg_buf, pos);//�������ʾˢ��
	if( ++pos == 8 ) 	pos = 0;
	
	Led_Disp(ucLed);//LED��ʾ
	
	
}



//-----------------------------------------------
/*key_proc */

void Key_Proc(void)//���������ײ����ݱ��
{
	//��Զ����
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//���ٳ���
	
	Key_Value = Key_Read_BTN();//��ȡ�������µı��
	Key_Down = Key_Value & (Key_Old ^ Key_Value);// (0000^0101) = 0101     0101 & 0101 =0101  ,��������������½��صı仯���������ͱ��ΰ�����ֵ��ͬ
																					// (0101^0101) = 0000       0101&0000 = 0000  �������һֱ����ͬ����״̬��������Ϊ0
	Key_Old = Key_Value;
	
	
	
	
	//���ݴ����������仯
	switch(Key_Down)//�����׽���½�������
	{
		case 4://��ѹģʽ��Ƶ��ģʽ���л�
			Vol_Fre_Disp_Mode ^= 1;//0-��ѹģʽ��1-Ƶ��ģʽ
			break;
			
		case 7://��������ܹرջ��ߴ�
			Seg_Dis_or_Die ^= 1;//0-��1-��
			break;		
		
		case 5://����DAC���		
			DAC_Output_Ctrl^= 1;//0-�̶�2v��1-����Rb2�仯��ģʽ
			break;			
		
		case 6://����LED�رջ��ߴ�
			LED_Dis_or_Die ^= 1;//0-��1-��
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

	//������ĵط�����ʵ��DAC����,0-�̶�2v��1-����Rb2�仯��ģʽ
	if(DAC_Output_Ctrl == 0)
		Pcf8591_Dac(0x66);//�̶����2v
	else
	{
		Pcf8591_Dac(Pcf8591_Adc(Adj_Res_Channel));//�ȶ���������ת����ȥ
	}
	

	
	//���ݴ����������仯

	if(Seg_Dis_or_Die)
	{
		if(Vol_Fre_Disp_Mode == 0)
			sprintf(seg_string,"U    %3.2f",Pcf8591_Adc(Adj_Res_Channel)/51.0);
		else 
			sprintf(seg_string,"F %6d",Frequent);//��ȡƵ��ֵ	
	}	
	else 
			sprintf(seg_string,"        ");//����ʾ����	
	

	
	//��Զ����
	Seg_Tran(seg_string, seg_buf);
}

//----------------------------------------------
/*led_proc */
void Led_Proc(void)//��ʾ������ʾ��Ϣ����
{
	
	if(LED_Dis_or_Die)//����LED����0-��1-��
	{	
			//L1��L2�����ƿ��Ƶ�ѹģʽ����Ƶ��ģʽ
			if(Vol_Fre_Disp_Mode == 0)//0-��ѹģʽ L1����L2��
			{
				ucLed &= (~0x03);//����λ����
				ucLed |= 0x01; //L1��
			}
			else//1-Ƶ��ģʽ L2����L1��
			{
				ucLed &= (~0x03);//����λ����
				ucLed |= 0x02; //L1��
			}		
			
			//L3������ʾʵʱ��ѹ�Ĵ�С��ƥ��ƣ�1.5-2.5-3.5����1.5-2.5����>3.5��
			AD_Num = Pcf8591_Adc(Adj_Res_Channel)/51.0;
			if(((AD_Num>=1.5)&&(AD_Num<2.5))||(AD_Num>=3.5))
				ucLed |= 0x04; //L3��	
			else 
				ucLed &= (~0x04); //L3��			

			
			//L4��ʾƵ�ʴ�С��ƥ��ƣ����ڵ���1000��С�ڵ���5000������ڵ���10000HZ��
			if(((Frequent>1000)&&(Frequent<5000))||(Frequent>10000))
				ucLed |= 0x08; //L4��	
			else 
				ucLed &= (~0x08); //L4��	
			
			//L5���ģʽ��0-�̶�2v��Ϩ��1-����Rb2�仯ģʽ������
			if(DAC_Output_Ctrl)
				ucLed |= 0x10; //L5��			
			else
				ucLed &= (~0x10); //L5��		
	}
	else//��������������Ϩ��LED
		ucLed = 0;
	
	
	
	
	
}


