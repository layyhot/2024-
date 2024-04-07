#include "bsp_555_PWM_Freq.h"

void PWM_555_Freq_Timer0Init(void)		//配置成了计数器模式，不自动重装
{
	TMOD &= 0xF0;		
	TMOD |= 0x05;		 
	
	TL0 = 0;		//设置定时初值
	TH0 = 0;		//设置定时初值
	
	TR0 = 1;		//定时器0开始计时
}



