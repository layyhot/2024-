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

//--����ʱ��ʱר��
#include "bsp_ds1302.h"

//--����EEPROM/AD/DAʱ��
//#include "bsp_iic.h"

//--����555Ƶ�ʲ���ʱ��ʹ��
//#include "bsp_555_PWM_Freq.h"

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
unsigned char Rtc[3] = {23,59,55};

//--555����Ƶ��ר�ñ���
//unsigned int freq;

//--����ר�ñ���
//unsigned int Usart_Slow_Down;
//unsigned char Uart_Buf[8];//���ݷ��ͺͽ��յĻ�����
//unsigned char Uart_Buf_Index = 0;


//�Զ��������
unsigned char Screen_Display_No;//0-�¶���ʾ��1-ʱ����ʾ��2-��������
bit Sys_Mode;//0-�¶ȿ��ƣ�1-ʱ�����
unsigned char Temp_Compare = 23;//�¶Ȳ����Ƚ�ֵ
bit Time_Disp_Mode;//ʱ����ʾ��һ��ģʽ��0-ʱ�֣�1-����
float Temp_Value;//�¶�����
unsigned long Relay_ms_Tick = 0;
unsigned long LED_ms_Tick = 0;
bit Relay_Xihe_Flag;


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

	//--EEPROM����
	//EEPROM_Write(eeprom_string, 0, 4);	
	
	//--ʱ�Ӳ���
	Set_Rtc(Rtc);
	
	//--555����Ƶ��ר�õĳ�ʼ������
  //PWM_555_Freq_Timer0Init();		//���ó��˼�����ģʽ�����Զ���װ
	
	//--����ר��
	//UsartInit();
	
	//--������ר��
  //Ultrasonic_Timer0Init();		
	rd_temperature();
  Delay750ms();
	
	
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
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;	
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
	Key_Down = Key_Value & (Key_Old ^ Key_Value);//�½��أ������ĸ����������ֵ���Ǽ�
	Key_Old = Key_Value;//����ƽ��һֱ����һֱ����һ����
	
	
	/***�û��Զ����������***/	
 
	
	
	switch(Key_Down)//����ṹ�ܳ������б����ĸ���������ȥ�ˣ����ݰ������µ�����������ڲ����ݱ����ı仯
	{
		case 12:
				if(++Screen_Display_No == 3) //0-�¶���ʾ��1-ʱ����ʾ��2-��������
					Screen_Display_No = 0;
				break;//���break����Ҫ
		
		case 13:
		    Sys_Mode^=1;//0-�¶ȿ��ƣ�1-ʱ�����
				P0 = 0X00;
		   	P2 = P2 & 0X1F | 0xA0;//beep relay
			  P2 &= 0X1F;	
				Relay_Xihe_Flag = 0;
			break;
	}
	
	
	
	
	
	if(Screen_Display_No == 2)
	{
		switch(Key_Down)//����ṹ�ܳ������б����ĸ���������ȥ�ˣ����ݰ������µ�����������ڲ����ݱ����ı仯
		{
			case 16:
					if(++Temp_Compare == 100)  
						Temp_Compare = 99;
					break;//���break����Ҫ
			
			case 17:
					if(--Temp_Compare == 9)  
						Temp_Compare = 10;
				break;
		}	
	}
	
	
	
  if(Screen_Display_No == 1)
	{
		if(Key_Old == 17)
		{
    	Time_Disp_Mode = 1;
		}
		else if(Key_Old == 0)
		{
    	Time_Disp_Mode = 0;			
		}
	}
			
	
	/***�û��Զ����������***/			
}





void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	
	//����һƬ�����������ݵĲɼ�
	Temp_Value = rd_temperature()/16.0;
	Read_Rtc(Rtc);	
	
	
	
	
	
	//��ʾ��
	
	if(Screen_Display_No == 0)
	{
 		sprintf(seg_string,"U1   %4.1f",Temp_Value);	
  }	
	else if(Screen_Display_No == 1)
	{
		if(Time_Disp_Mode == 0)
		{
			sprintf(seg_string,"U2 %02d-%02d",(unsigned int)Rtc[0],(unsigned int)Rtc[1]);	
		}
		else
		{
			sprintf(seg_string,"U2 %02d-%02d",(unsigned int)Rtc[1],(unsigned int)Rtc[2]);			
		}
  }		
	else if(Screen_Display_No == 2)
	{
 		sprintf(seg_string,"U3    %02d",(unsigned int)Temp_Compare);	
  }	
	
	
	

	//���������  Sys_Mode;//0-�¶ȿ��ƣ�1-ʱ�����
	if(Sys_Mode == 0)//
	{
		if((unsigned int)(Temp_Value*10) > (unsigned int)(Temp_Compare*10))
		{
			P0 = 0X10;
			P2 = P2 & 0X1F | 0xA0;//beep relay
			P2 &= 0X1F;	
      Relay_Xihe_Flag = 1;			
		}
		else
		{
			P0 = 0X00;
			P2 = P2 & 0X1F | 0xA0;//beep relay
			P2 &= 0X1F;	
      Relay_Xihe_Flag = 0;						
		}
	}
	else
	{
		if((Rtc[1] == 0)&&(Rtc[2] == 0))
		{
			P0 = 0X10;
			P2 = P2 & 0X1F | 0xA0;//beep relay
			P2 &= 0X1F;			
			Relay_ms_Tick = ms_Tick;
      Relay_Xihe_Flag = 1;						
		}
		
		if((ms_Tick - Relay_ms_Tick) >= 5000)
		{
			P0 = 0X00;
			P2 = P2 & 0X1F | 0xA0;//beep relay
			P2 &= 0X1F;
      Relay_Xihe_Flag = 0;						
		}
	}
	
	
	
	
	
	
	
//	if(state_flag == 1)
//	{
//		sprintf(seg_string,"12345678");	
//  }		
//	else //��Ļ����ʾ�����ݣ�������ط����͵�ͬ��seg_string�з���ʲô���ݣ���ҷ�ʲô���Ҿ͸���ʾʲô������װ�ƣ�ֻ������seg����ʾ�ķ�Χ��
//	{
		//--�¶ȴ�����ר��
//		sprintf(seg_string,"%03d",(unsigned int)(rd_temperature()/16.0));	
		
		//--ʱ��ר��
//		Read_Rtc(Rtc);
//		sprintf(seg_string,"%02d-%02d-%02d",(unsigned int)Rtc[0],(unsigned int)Rtc[1],(unsigned int)Rtc[2]);	
		
		//--EEPROM����
		//EEPROM_Read(eeprom_string_null, 0, 4);
		//sprintf(seg_string,"%2d--%2d",(unsigned int)eeprom_string_null[0],(unsigned int)eeprom_string_null[3]);	

		//--AD����
		//sprintf(seg_string,"%03d",(unsigned int)Pcf8591_Adc(Adj_Res_Channel));	
		
		//--DA����
		//Pcf8591_Dac(255);
		
		//--555Ƶ�ʲ���
    //sprintf(seg_string,"---%5d",freq);	
		
		//--����������
	  //sprintf(seg_string,"     %03d",(unsigned int)(Wave_Recv()));	
		
//		
//	}
	
	/***�û��Զ����������***/			
	
	Seg_Tran(seg_string, seg_buf);
}






void	Led_Proc(void)
{
	if(Led_Slow_Down) return;
	Led_Slow_Down = 1;
	
	
	/***�û��Զ����������***/		

	
	if((Rtc[1] == 0)&&(Rtc[2] == 0))
	{
		ucLed |= 0x01;		
	  LED_ms_Tick = ms_Tick;
	}
	if((ms_Tick - LED_ms_Tick) >= 5000)
	{
		ucLed &= (~0x01);			
	}
	
	

	if(Sys_Mode == 0)
		ucLed |= 0x02;
	else
		ucLed &= (~0x02);			
	
	
	
	if(Relay_Xihe_Flag == 1)
		ucLed ^= 0x04;		
	else
		ucLed &= (~0x04);	 
	
	
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

