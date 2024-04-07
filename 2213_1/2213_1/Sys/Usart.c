#include "Usart.h"


void UsartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0x8F;		//设定定时初值
	T2H = 0xFD;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	
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