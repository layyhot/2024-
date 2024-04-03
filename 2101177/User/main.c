#include "stdio.h"//��sprintfר��

#include "Timer.h"//�ڲ���һ��

#include "bsp_init.h"//�ⲿͦ�����
#include "bsp_key.h"//������֮1 key ���ִ�����
#include "bsp_seg.h"//������֮2 seg �����ܿ���
#include "bsp_led.h"//������֮3 led �㻹�ֺ���ˣ����ۣ��Ҳ�����



//���ݿ���ģ�����ͷ�ļ�

//--�����¶ȴ�����ʱ��
//#include "bsp_onewire.h"

//--����ʱ��ʱר��
//#include "bsp_ds1302.h"

//--����EEPROM/AD/DAʱ��
//#include "bsp_iic.h"

//--����555Ƶ�ʲ���ʱ��ʹ��
#include "bsp_555_PWM_Freq.h"

//--���촮��ʱ��ʹ��
//#include "Usart.h"

//--���쳬������ʱ��ʹ��
//#include "bsp_ultrasonic.h"



//�����պ���������
void Key_Proc(void);
void Seg_Proc(void);
void Led_Proc(void);


//���ݿ������ݣ���������������
//--���촮��ʱ��ʹ��
//void Uart_Proc(void);


//������
//--��ʱ���δ����
unsigned long ms_Tick = 0;

//--��ʱ�����ٱ���
unsigned int Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned int Led_Slow_Down;


//--����ר�ñ���
unsigned char Key_Value;
unsigned char Key_Old, Key_Down;

//--�����ר�ñ���
unsigned char seg_string[10];
unsigned char seg_buf[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char pos;

//--LEDר�ñ���
unsigned char ucLed;



/***�û��Զ����������ʼ��***/

bit state_flag = 0;//����ģ��û���Ҫ�õ�һ������

//--EEPROM����ר�ñ���
//unsigned char eeprom_string[4] = {11,12,13,14};
//unsigned char eeprom_string_null[4];

//--ʱ��ר�ñ���
//unsigned char Rtc[3] = {23,59,55};

//--555����Ƶ��ר�ñ���
unsigned int freq;

//--����ר�ñ���
//unsigned int Usart_Slow_Down;
//unsigned char Uart_Buf[8];//���ݷ��ͺͽ��յĻ�����
//unsigned char Uart_Buf_Index = 0;

/***�û��Զ��������������***/



void main(void)
{

	Cls_Peripheral();//�ر�LED/�̵���/������
	Timer1Init();//��ʱ��1��ʼ��
	EA = 1;//�����ж�

	//--EEPROM����
	//EEPROM_Write(eeprom_string, 0, 4);	
	
	//--ʱ�Ӳ���
	//Set_Rtc(Rtc);
	
	//--555����Ƶ��ר�õĳ�ʼ������
  PWM_555_Freq_Timer0Init();		//���ó��˼�����ģʽ�����Զ���װ
	
	//--����ר��
	//UsartInit();
	
	//--������ר��
  //Ultrasonic_Timer0Init();		
	
	while(1)
	{
		
		Key_Proc();//�������Ӻ���
		Seg_Proc();
		Led_Proc();
		
		//--����ר��
//    Uart_Proc();
		
	}
}



/* Timer1_interrupt routine */
void tm1_isr() interrupt 3
{
	ms_Tick++;//�δ�ʱ�����ܼ�¼�����ݳ���29��
	
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//���ٱ������������Ӻ�����ˢ��Ƶ��
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;	
	if(++Led_Slow_Down == 100) Led_Slow_Down = 0;	
//--����ר�ü���
//	if(++Usart_Slow_Down == 100) Usart_Slow_Down = 0;	
	
	
//--555����Ƶ��ר�õĲ�������
//		if((ms_Tick%1000) == 0)
//	{
// 		freq = ((TH0<<8)|TL0);
// 		TH0 = 0;
// 		TL0 = 0;
//	}
	
	
	
	
	
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
	Key_Down = Key_Value & (Key_Value ^ Key_Old);//�½��أ������ĸ����������ֵ���Ǽ�
	Key_Old = Key_Value;//����ƽ��һֱ����һֱ����һ����
	
	
	/***�û��Զ����������***/	
	switch(Key_Down)//����ṹ�ܳ������б����ĸ���������ȥ�ˣ����ݰ������µ�����������ڲ����ݱ����ı仯
	{
		case 4:
				state_flag ^= 1;//0-����������ܺͶ�ʱ������ʾ12345678�������Եƣ�������1-�¶Ȼ�������ֵ
				break;//���break����Ҫ
		case 5:
				state_flag ^= 1;//0-����������ܺͶ�ʱ������ʾ12345678�������Եƣ�������1-�¶Ȼ�������ֵ
				break;//���break����Ҫ
		case 6:
				state_flag ^= 1;//0-����������ܺͶ�ʱ������ʾ12345678�������Եƣ�������1-�¶Ȼ�������ֵ
				break;//���break����Ҫ
		case 7:
				state_flag ^= 1;//0-����������ܺͶ�ʱ������ʾ12345678�������Եƣ�������1-�¶Ȼ�������ֵ
				break;//���break����Ҫ

	}
	/***�û��Զ����������***/		

	
}





void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/***�û��Զ����������***/	
	if(state_flag == 4)
	{
		sprintf(seg_string,"12345678");	
  }		
	else //��Ļ����ʾ�����ݣ�������ط����͵�ͬ��seg_string�з���ʲô���ݣ���ҷ�ʲô���Ҿ͸���ʾʲô������װ�ƣ�ֻ������seg����ʾ�ķ�Χ��
	{
		//--�¶ȴ�����ר��
		sprintf(seg_string,"%-4-  02dC",(unsigned int)(rd_temperature()/16.0));	
		
			
		//--555Ƶ�ʲ���
    //sprintf(seg_string,"---%5d",freq);			
	}
	
	/***�û��Զ����������***/			
	
	Seg_Tran(seg_string, seg_buf);
}






void	Led_Proc(void)
{
	if(Led_Slow_Down) return;
	Led_Slow_Down = 1;
	
	
	/***�û��Զ����������***/		
 	if(state_flag == 0)
		ucLed = 0xff;			
	else 
	 	ucLed = 0;	
	/***�û��Զ����������***/			

}



/*----------------------------
����ר�ã�UART�жϷ������
-----------------------------*/
//void Uart() interrupt 4
//{
//	if (RI)
//	{
//		RI = 0;                 //���RIλ
//		Uart_Buf[Uart_Buf_Index] = SBUF;              //P0��ʾ��������
//		Uart_Buf_Index++;//��n�����ݹ���֮�󣬱����n
//	}
//}

/*----------------------------
����ר�ã����ڴ����ӳ���
-----------------------------*/
//void Uart_Proc(void)
//{
//	if(Usart_Slow_Down) return;
//		Usart_Slow_Down = 1;
//	
//	if(Uart_Buf_Index != 0)//��ʾ���յ�������
//	{
//		Uart_Buf_Index = 0;

//		sprintf(seg_string,"%s",Uart_Buf);			
//		Usart_Send_String(seg_string);
//	}
//}

