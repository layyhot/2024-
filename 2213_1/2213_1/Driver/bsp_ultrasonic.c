#include "bsp_ultrasonic.h"

sbit Tx = P1^0;
sbit Rx = P1^1;

void Ultrasonic_Timer0Init(void)
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0并不开始计时
}


unsigned char Wave_Recv(void)
{
	unsigned char Wave_Num = 10;
  unsigned char Dist;
	
	//完成超声波的发送
	Tx = 0;
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值	
	
	TR0 = 1;
	while( Wave_Num-- )
	{
		
		while(!TF0);
		Tx ^= 1;
		TF0 = 0;
	}
	
	
	//完成超声波的接收
	TR0 = 0;	//关掉定时器的目的是为了重新装载
	TL0 = 0;		//设置定时初值，用于计时
	TH0 = 0;		//设置定时初值		
	TR0 = 1;	//关掉定时器的目的是为了重新装载	
	
	while(Rx && (~TF0));
	TR0 = 0;	//关闭的目的是计算距离
	if(TF0 == 1)//表示溢出了
	{
		Dist = 250;//表示数据太大了，有病，为什么要测量这么长的高度
		TF0 = 0;
	}
	else //没有产生溢出
	{
		Dist = (((TH0<<8) | TL0) * 0.017);
	}
	return Dist;
	
}


