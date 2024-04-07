//�������ƣ����Ϲ�������һ�Ƚ���ģ��
//�����ƣ���һģ��
//������汾��V2.0

//***************************
//V1.0�����漰����Դģ��(ʡ����Ƶģ�鿼��)
//--Timer(��ʱ��1)
//--bsp_ds1302(ʱ��)
//--bsp_iic(AD/DA/EEPROM)
//--bsp_init(�ر�����)
//--bsp_key(����)
//--bsp_led(LED)
//--bsp_onewire(�¶ȴ�����)
//--bsp_seg(�����)
//�������ڣ�2021.3.3

//***************************
//V2.0������Դģ�飨ʡ����Ƶ��������Ƶ����)
//--Usart������1��
//--bsp_555_PWM_Freq(555Ƶ�ʣ�
//--bsp_ultrasonic(������)
//�������ڣ�2021.4.4

//�з���λ��������С�Ƽ����޹�˾���Ϲ����Ƽ�ʵ����
//�������ߣ�������
//��Ȩ���У���Ȩ�ؾ�


#include "stdio.h"//��sprintfר��

#include "Timer.h"//�ڲ���һ��

#include "bsp_init.h"//�ⲿͦ�����
#include "bsp_key.h"//������֮1 key ���ִ�����
#include "bsp_seg.h"//������֮2 seg �����ܿ���
#include "bsp_led.h"//������֮3 led �㻹�ֺ���ˣ����ۣ��Ҳ�����



//���ݿ���ģ�����ͷ�ļ�

//--�����¶ȴ�����ʱ��
#include "bsp_onewire.h"

//--����EEPROM/AD/DAʱ��
#include "bsp_iic.h"



//�����պ���������
void Key_Proc(void);
void Seg_Proc(void);
void Led_Proc(void);
void Voltage_Output_Proc(void);

//���ݿ������ݣ���������������

//������
//--��ʱ���δ����
unsigned long ms_Tick = 0;

//--��ʱ�����ٱ���
unsigned int Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned int Led_Slow_Down;
unsigned int Volatge_Output_Slow_Down;

//--����ר�ñ���
unsigned char Key_Value;
unsigned char Key_Old, Key_Down;

//--�����ר�ñ���
unsigned char seg_string[10];
unsigned char seg_buf[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char pos;

//--LEDר�ñ���
unsigned char ucLed = 0x00;



/***�û��Զ����������ʼ��***/
unsigned char Disp_Interface = 0;//��ʾ���棬0-�¶���ʾ��1-�������ã�2-DA�������
unsigned char Temp_Compare_Disp = 25;//�¶ȱȽ�ֵ,����������ʾ������ʱ��ʹ��
unsigned char Temp_Compare_Ctrl = 25;//�¶ȱȽ�ֵ,���������õ�
bit DA_Output_Mode = 0;//DA�����ģʽ��0-ģʽ1��1-ģʽ2
float Temp;//��ʵ���¶�����
unsigned char DA_Inter_Num = 0;//����DA�����ѹ���ڲ�������ֵ��0-255

/***�û��Զ��������������***/


void Delay750ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 35;
	j = 51;
	k = 182;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}




void main(void)
{

	Cls_Peripheral();//�ر�LED/�̵���/������

	Timer1Init();//��ʱ��1��ʼ��
	EA = 1;//�����ж�
	
	rd_temperature();
	Delay750ms();

	
	while(1)
	{
		Key_Proc();//�������Ӻ���
		Seg_Proc();
		Led_Proc();
		Voltage_Output_Proc();
			
	}
}



/* Timer1_interrupt routine */
void tm1_isr() interrupt 3
{
	ms_Tick++;//�δ�ʱ�����ܼ�¼�����ݳ���29��
	
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//���ٱ������������Ӻ�����ˢ��Ƶ��
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;	
	if(++Led_Slow_Down == 100) Led_Slow_Down = 0;	
	if(++Volatge_Output_Slow_Down == 500) Volatge_Output_Slow_Down = 0;	
	

	
	
	
  Seg_Disp(seg_buf, pos);//�����������ʾ
	if(++pos == 8) pos = 0;
	
	Led_Disp(ucLed);//����LED��ʾ
	
}









void	Key_Proc(void)
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	//���д���
	Key_Value = Key_Read();
	Key_Down = Key_Value & (Key_Old ^ Key_Value);//�½��أ������ĸ����������ֵ���Ǽ�
	Key_Old = Key_Value;//����ƽ��һֱ����һֱ����һ����
	
	
	/***�û��Զ����������***/	
	switch(Key_Down)//����ṹ�ܳ������б����ĸ���������ȥ�ˣ����ݰ������µ�����������ڲ����ݱ����ı仯
	{
		case 4://��ʾ�����л�������0-�¶���ʾ��1-�������ã�2-DA�������
				if(++Disp_Interface == 3) Disp_Interface = 0;
				if(Disp_Interface == 2) Temp_Compare_Ctrl = Temp_Compare_Disp;//ʹ���趨�ıȽ�ֵ
				break;//���break����Ҫ
		
		case 8://����������ֵ-
				if(Disp_Interface == 1) 
				{
					Temp_Compare_Disp--;
					if(Temp_Compare_Disp > 200)//����������Ϊ0
						Temp_Compare_Disp = 0;
				}
					break;//���break����Ҫ	
				
		case 9:///����������ֵ+
				if(Disp_Interface == 1) 
				{
					Temp_Compare_Disp++;
					if(Temp_Compare_Disp > 99)//����������Ϊ99
						Temp_Compare_Disp = 99;
				}
					break;//���break����Ҫ		
				
				
		case 5://DA�����ģʽ�л���0-ģʽ1��1-ģʽ2
				DA_Output_Mode ^= 1;
				break;
		
	}
	/***�û��Զ����������***/		

	
}





void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/***�û��Զ����������***/	
	
//	sprintf(seg_string,"%1d  %02d  %1d",(unsigned int)Disp_Interface,(unsigned int)Temp_Compare_Disp,(unsigned int)DA_Output_Mode);	
	
//	if(state_flag == 0)
//	{
//		sprintf(seg_string,"12345678");	
//  }		
//	else //��Ļ����ʾ�����ݣ�������ط����͵�ͬ��seg_string�з���ʲô���ݣ���ҷ�ʲô���Ҿ͸���ʾʲô������װ�ƣ�ֻ������seg����ʾ�ķ�Χ��
//	{
//		//--�¶ȴ�����ר��
//		sprintf(seg_string,"     %03d",(unsigned int)(rd_temperature()/16.0));	
//		
//		//--DA����
//		Pcf8591_Dac(255);

//	}
	
	Temp = rd_temperature()/16.0;//�ɼ��¶�����
	
	switch(Disp_Interface)//��ʾ�����л�������0-�¶���ʾ��1-�������ã�2-DA�������
	{
		case 0://�¶���ʾ
 	  	sprintf(seg_string,"C   %5.2f",Temp);
			break;		
		
		case 1://��������
			sprintf(seg_string,"P     %02d",(unsigned int)Temp_Compare_Disp);	
			break;			
		
		case 2://DA�������
			sprintf(seg_string,"A    %4.2f",DA_Inter_Num/51.0);	
			break;		
	}
	
	
	
	
	/***�û��Զ����������***/			
	
	Seg_Tran(seg_string, seg_buf);
}






void	Led_Proc(void)
{
	if(Led_Slow_Down) return;
	Led_Slow_Down = 1;
	
	
	/***�û��Զ����������***/		
 	if(DA_Output_Mode == 0)//0-ģʽ1��L1������1-ģʽ2��L1Ϩ��
		ucLed |= 0x01;			
	else 
	 	ucLed &= (~0x01);	
	
	
	switch(Disp_Interface)//��ʾ�����л�������0-�¶���ʾ��1-�������ã�2-DA�������
	{
		case 0://�¶���ʾ,L2��
			ucLed &= (~0x0E);
			ucLed |= 0x02;		
			break;		
		case 1://��������,L3��
			ucLed &= (~0x0E);			
			ucLed |= 0x04;
			break;				
		case 2://DA�������,L4��
			ucLed &= (~0x0E);			
			ucLed |= 0x08;
			break;		
	}	
	
	/***�û��Զ����������***/			

}


void Voltage_Output_Proc(void)
{
	if(Volatge_Output_Slow_Down) return;
	Volatge_Output_Slow_Down = 1;	
	
	if(DA_Output_Mode == 0)//0-ģʽ1��1-ģʽ2
	{
		if(Temp < Temp_Compare_Ctrl)
			DA_Inter_Num = 0;
		else 
			DA_Inter_Num = 255;			
	}
	else
	{
		if(Temp < 20)
			DA_Inter_Num = 51;	
		else if(Temp > 40)
			DA_Inter_Num = 204;	
		else
			DA_Inter_Num = 7.65*Temp - 102;
	}	


	Pcf8591_Dac(DA_Inter_Num);	
		

}


