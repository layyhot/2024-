#include "Usart.h"


void UsartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0x8F;		//�趨��ʱ��ֵ
	T2H = 0xFD;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	
	ES = 1;
}


void Usart_Send_String( unsigned char *Usart_String )
{
	
	while(*Usart_String != '\0')
	{
		SBUF = *Usart_String;
		while(TI == 0);
		TI = 0;
		Usart_String++;
	}


}