# include "bsp_init.h"

# include "bsp_seg.h"
# include "timer.h"
# include "bsp_key.h"
# include "bsp_led.h"

#include "bsp_onewire.h"

# include "stdio.h"


#define  PWM_OUT	P34 //define是用前边的代替后边的

//-----------------------------------------------
/* 函数声明 */
//三个主体循环，基本上永远不变
void Key_Proc(void);//按键处理，底层数据变更
void Seg_Proc(void);//显示处理，显示信息生成
void Led_Proc(void);//LED处理，LED状态信息表示


//-----------------------------------------------
/* 全局变量声明 */

//数码管显示专用，基本上永远不变
unsigned char seg_buf[8];//放置字符串转换后的段码到数组
unsigned char seg_string[10];//放置字符串
unsigned char	pos = 0;//中断显示专用

//LED显示专用，基本上永远不变
unsigned char ucLed;//记录LED的状态信息

//按键专用，基本上永远不变
unsigned char Key_Value;//读取按键的数值存储变量
unsigned char Key_Down, Key_Old;//读取按键的数值存储变量	

//按键和显示函数减速专用，基本永远不变
unsigned int Key_Slow_Down;//按键减速
unsigned int Seg_Slow_Down;//按键减速


//根据代码需求发生变化
unsigned char Running_Mode = 0; //运行模式,初始值为模式0 
unsigned char Countdown_Sec = 0; //倒计时秒，初始值为模式0 
bit Temp_Mode = 0;//温度显示模式。1时，温度显示模式，当为0时，常规显示模式。

unsigned char Count_100us = 0; //记录第几次进入定时器0的100us中断
unsigned char Count_PWM = 0; //为产生PWM而产生的count
unsigned int Count_1ms = 0; //记录第几次进入定时器1的1ms中断

unsigned char S5_Countdown_Sec_Sluggish = 0; //令S5按下去之后，秒递减呆滞两秒的变量。
unsigned int temp;
//-----------------------------------------------
/* main */
void main()
{
	//根据代码需求发生变化
	Cls_Peripheral();//关闭外设
	Timer1Init();//定时器1初始化，并且使能定时器1中断，1ms进入一次
  Timer0Init();//定时器0初始化，	100us进入一次中断函数。
	EA = 1;//打开总中断


	while(1)
	{
		//永远不变
		Key_Proc();//按键处理，底层数据变更
		Seg_Proc();//显示处理，显示信息生成
    Led_Proc();//LED处理，LED状态信息表示		
	}


}



//-----------------------------------------------
/* Timer1_interrupt routine 1000us*/
void tm1_isr() interrupt 3
{	
	
	//永远不变
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;
	

	if(++Count_1ms == 1000) //秒定时
	{
		Count_1ms = 0;
		if(S5_Countdown_Sec_Sluggish) S5_Countdown_Sec_Sluggish--;//呆滞有效函数
		else if( (--Countdown_Sec) > 200) Countdown_Sec = 0;//风扇定时自动递减
	}
	
	

	//永远不变
	Seg_Disp(seg_buf, pos);//数码管显示刷新
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
				Count_100us=0;//让记录第几次的变量，在0-9之间变化。
				PWM_OUT = 1;//每10次拉高一次
				ucLed |= 0xF8;//令高5位置1，后五个灯亮
			}
			
			switch(Running_Mode)
			{
				case 0:	Count_PWM = 2; break;
				case 1: Count_PWM = 3; break;
				case 2: Count_PWM = 7; break;
			}
			
			if(Count_100us == Count_PWM) 
			{
				PWM_OUT = 0;//到模式规定的状态位置，拉低一次。
				ucLed &= 0x07;//令高5位复0，后五个灯灭	
			}
		}
	else
	{
				PWM_OUT = 0;//回复到复位状态。
				ucLed &= 0x07;//令高5位复0，后五个灯灭			
	}
	
	Led_Disp(ucLed);//LED显示	
	
}




//-----------------------------------------------
/*key_proc */

void Key_Proc(void)//按键处理，底层数据变更
{
	//永远不变
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//减速程序
	
	Key_Value = Key_Read();//读取按键按下的编号
	Key_Down = Key_Value & (Key_Old ^ Key_Value);// (0000^0101) = 0101     0101 & 0101 =0101  ,如果按键发生了下降沿的变化，输出结果和本次按键数值相同
																					// (0101^0101) = 0000       0101&0000 = 0000  如果按键一直保持同样的状态，输出结果为0
	Key_Old = Key_Value;
	
	
	
	
	//根据代码需求发生变化
	switch(Key_Down)
	{
		case 4://模式切换按钮
		  if ( ++Running_Mode == 3)  Running_Mode = 0;//运行模式在0-2之间切换
			break;
			
		case 5://运行时间切换按钮
			Countdown_Sec+=60;//运行时间在每按一次S5要自动增加60s
			if((Countdown_Sec>=60)&(Countdown_Sec<120)) Countdown_Sec = 60;//当系统倒计时到0-60秒，加完60之后，变成60-120，强制将倒计时限位为60
			if((Countdown_Sec>=120)&(Countdown_Sec<180)) Countdown_Sec = 120;//当系统倒计时到60-120秒，加完60之后，变成120-180，强制将倒计时限位为120
			if(Countdown_Sec ==180) Countdown_Sec = 0;//从120s定时切换到0定时
			S5_Countdown_Sec_Sluggish = 1;//让秒递减呆滞的时间
			break;	
		
		case 9:	//停止按钮
			Countdown_Sec = 0;//将运行时间清零
			break;	

			
		case 8:
			Temp_Mode ^= 1;//0-1的变化
			break;		

	}
	
	
	
}



//----------------------------------------------
/*seg_proc */
void Seg_Proc(void)//显示处理，显示信息生成
{
	//永远不变
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;//减速程序

	
	
	//根据代码需求发生变化
  if(Temp_Mode == 0)
		sprintf(seg_string,"-%1d- %04d",(unsigned int)(Running_Mode+1),(unsigned int)Countdown_Sec);
	else 
	{
		temp = rd_temperature()>>4;
		if(temp < 50)
		sprintf(seg_string,"-4-  %02dC",temp);		
	}
	//永远不变
	Seg_Tran(seg_string, seg_buf);
}

//----------------------------------------------
/*led_proc */
void Led_Proc(void)//显示处理，显示信息生成
{
	
	//根据代码需求发生变化
	if(Countdown_Sec)
	{		
	
		switch(Running_Mode)
		{
			case 0:	ucLed |= 0x01; ucLed &= (~0x06); break;//在让L1点亮的同时，要将L2和L3关闭，L4到L8不影响
			case 1: ucLed |= 0x02; ucLed &= (~0x05); break;//在让L2点亮的同时，要将L1和L3关闭，L4到L8不影响
			case 2: ucLed |= 0x04; ucLed &= (~0x03); break;//在让L3点亮的同时，要将L1和L2关闭，L4到L8不影响  
		}
	}
	else 
	{
		ucLed &= (~0x07);
	}
		
		
			
}


