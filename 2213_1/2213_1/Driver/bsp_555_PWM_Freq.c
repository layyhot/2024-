#include "bsp_555_PWM_Freq.h"

void PWM_555_Freq_Timer0Init(void)		//���ó��˼�����ģʽ�����Զ���װ
{
	TMOD &= 0xF0;		
	TMOD |= 0x05;		 
	
	TL0 = 0;		//���ö�ʱ��ֵ
	TH0 = 0;		//���ö�ʱ��ֵ
	
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}



