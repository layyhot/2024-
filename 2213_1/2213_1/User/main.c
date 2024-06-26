//代码名称：蚂蚁工厂国家一等奖大模板
//代码简称：国一模板
//本代码版本：V2.0

//***************************
//V1.0代码涉及的资源模块(省赛高频模块考点)
//--Timer(定时器1)
//--bsp_ds1302(时钟)
//--bsp_iic(AD/DA/EEPROM)
//--bsp_init(关闭外设)
//--bsp_key(按键)
//--bsp_led(LED)
//--bsp_onewire(温度传感器)
//--bsp_seg(数码管)
//更新日期：2021.3.3

//***************************
//V2.0新增资源模块（省赛低频，国赛高频考点)
//--Usart（串口1）
//--bsp_555_PWM_Freq(555频率）
//--bsp_ultrasonic(超声波)
//更新日期：2021.4.4

//研发单位：北京以小科技有限公司蚂蚁工厂科技实验室
//代码作者：杜永博
//版权所有，侵权必究


#include "stdio.h"//给sprintf专用

#include "Timer.h"//内部就一个

#include "bsp_init.h"//外部挺多余的
#include "bsp_key.h"//三大金刚之1 key 用手触摸的
#include "bsp_seg.h"//三大金刚之2 seg 用眼能看到
#include "bsp_led.h"//三大金刚之3 led 你还弥红灯了，晃眼，我不爱你



//根据考核模块添加头文件

//--考察温度传感器时用
#include "bsp_onewire.h"

//--考察时钟时专用
#include "bsp_ds1302.h"

//--考察EEPROM/AD/DA时用
//#include "bsp_iic.h"

//--考察555频率测量时候使用
//#include "bsp_555_PWM_Freq.h"

//--考察串口时候使用
//#include "Usart.h"

//--考察超声波的时候使用
//#include "bsp_ultrasonic.h"



//三大金刚函数声明区
void Key_Proc(void);
void Seg_Proc(void);
void Led_Proc(void);


//根据考核内容，其他函数声明区
//--考察串口时候使用
//void Uart_Proc(void);


//变量区
//--定时器滴答变量
unsigned long ms_Tick = 0;

//--定时器减速变量
unsigned int Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned int Led_Slow_Down;


//--按键专用变量
unsigned char Key_Value;
unsigned char Key_Old, Key_Down;

//--数码管专用变量
unsigned char seg_string[10];
unsigned char seg_buf[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char pos;

//--LED专用变量
unsigned char ucLed;



/***用户自定义变量区开始↓***/

bit state_flag = 0;//测试模板好坏需要用的一个变量

//--EEPROM测试专用变量
//unsigned char eeprom_string[4] = {11,12,13,14};
//unsigned char eeprom_string_null[4];

//--时钟专用变量
unsigned char Rtc[3] = {23,59,55};

//--555测量频率专用变量
//unsigned int freq;

//--串口专用变量
//unsigned int Usart_Slow_Down;
//unsigned char Uart_Buf[8];//数据发送和接收的缓存区
//unsigned char Uart_Buf_Index = 0;


//自定义变量区
unsigned char Screen_Display_No;//0-温度显示，1-时间显示，2-参数设置
bit Sys_Mode;//0-温度控制，1-时间控制
unsigned char Temp_Compare = 23;//温度参数比较值
bit Time_Disp_Mode;//时间显示的一种模式，0-时分，1-分秒
float Temp_Value;//温度数据
unsigned long Relay_ms_Tick = 0;
unsigned long LED_ms_Tick = 0;
bit Relay_Xihe_Flag;


/***用户自定义变量区结束↑***/

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

	Cls_Peripheral();//关闭LED/继电器/蜂鸣器
	Timer1Init();//定时器1初始化
	EA = 1;//打开总中断

	//--EEPROM测试
	//EEPROM_Write(eeprom_string, 0, 4);	
	
	//--时钟测试
	Set_Rtc(Rtc);
	
	//--555测量频率专用的初始化函数
  //PWM_555_Freq_Timer0Init();		//配置成了计数器模式，不自动重装
	
	//--串口专用
	//UsartInit();
	
	//--超声波专用
  //Ultrasonic_Timer0Init();		
	rd_temperature();
  Delay750ms();
	
	
	while(1)
	{
		Key_Proc();//三大金刚子函数
		Seg_Proc();
		Led_Proc();
		
		//--串口专用
//    Uart_Proc();
		
	}
}



/* Timer1_interrupt routine */
void tm1_isr() interrupt 3
{
	ms_Tick++;//滴答定时器，能记录的数据长达29天
	
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//减速变量区，控制子函数的刷新频率
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;	
	if(++Led_Slow_Down == 100) Led_Slow_Down = 0;	
//--串口专用减速
//	if(++Usart_Slow_Down == 100) Usart_Slow_Down = 0;	
	
	
//--555测量频率专用的测量函数
//		if((ms_Tick%1000) == 0)
//	{
// 		freq = ((TH0<<8)|TL0);
// 		TH0 = 0;
// 		TL0 = 0;
//	}
	
	
	
	
	
  Seg_Disp(seg_buf, pos);//用于数码管显示
	if(++pos == 8) pos = 0;
	
	Led_Disp(ucLed);//用于LED显示
	
}









void	Key_Proc(void)
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	//三行代码
	Key_Value = Key_Read();
	Key_Down = Key_Value & (Key_Old ^ Key_Value);//下降沿，按得哪个按键，这个值就是几
	Key_Old = Key_Value;//检测电平，一直按着一直等于一个数
	
	
	/***用户自定义代码区↓***/	
 
	
	
	switch(Key_Down)//这个结构很常见，判别是哪个按键按下去了，根据按键按下的情况，控制内部数据变量的变化
	{
		case 12:
				if(++Screen_Display_No == 3) //0-温度显示，1-时间显示，2-参数设置
					Screen_Display_No = 0;
				break;//这个break很重要
		
		case 13:
		    Sys_Mode^=1;//0-温度控制，1-时间控制
				P0 = 0X00;
		   	P2 = P2 & 0X1F | 0xA0;//beep relay
			  P2 &= 0X1F;	
				Relay_Xihe_Flag = 0;
			break;
	}
	
	
	
	
	
	if(Screen_Display_No == 2)
	{
		switch(Key_Down)//这个结构很常见，判别是哪个按键按下去了，根据按键按下的情况，控制内部数据变量的变化
		{
			case 16:
					if(++Temp_Compare == 100)  
						Temp_Compare = 99;
					break;//这个break很重要
			
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
			
	
	/***用户自定义代码区↑***/			
}





void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	
	//借用一片区域用于数据的采集
	Temp_Value = rd_temperature()/16.0;
	Read_Rtc(Rtc);	
	
	
	
	
	
	//显示区
	
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
	
	
	

	//输出控制区  Sys_Mode;//0-温度控制，1-时间控制
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
//	else //屏幕上显示的内容，在这个地方，就等同于seg_string中放了什么内容，你敢放什么，我就敢显示什么，不敢装逼，只限制于seg能显示的范围。
//	{
		//--温度传感器专用
//		sprintf(seg_string,"%03d",(unsigned int)(rd_temperature()/16.0));	
		
		//--时钟专用
//		Read_Rtc(Rtc);
//		sprintf(seg_string,"%02d-%02d-%02d",(unsigned int)Rtc[0],(unsigned int)Rtc[1],(unsigned int)Rtc[2]);	
		
		//--EEPROM测试
		//EEPROM_Read(eeprom_string_null, 0, 4);
		//sprintf(seg_string,"%2d--%2d",(unsigned int)eeprom_string_null[0],(unsigned int)eeprom_string_null[3]);	

		//--AD测试
		//sprintf(seg_string,"%03d",(unsigned int)Pcf8591_Adc(Adj_Res_Channel));	
		
		//--DA测试
		//Pcf8591_Dac(255);
		
		//--555频率测试
    //sprintf(seg_string,"---%5d",freq);	
		
		//--超声波测试
	  //sprintf(seg_string,"     %03d",(unsigned int)(Wave_Recv()));	
		
//		
//	}
	
	/***用户自定义代码区↑***/			
	
	Seg_Tran(seg_string, seg_buf);
}






void	Led_Proc(void)
{
	if(Led_Slow_Down) return;
	Led_Slow_Down = 1;
	
	
	/***用户自定义代码区↓***/		

	
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
	
	
	/***用户自定义代码区↑***/			

}



/*----------------------------
串口专用，UART中断服务程序
-----------------------------*/
//void Uart() interrupt 4
//{
//	if (RI)
//	{
//		RI = 0;                 //清除RI位
//		Uart_Buf[Uart_Buf_Index] = SBUF;              //P0显示串口数据
//		Uart_Buf_Index++;//第n个数据过来之后，变成了n
//	}
//}

/*----------------------------
串口专用，串口处理子程序
-----------------------------*/
//void Uart_Proc(void)
//{
//	if(Usart_Slow_Down) return;
//		Usart_Slow_Down = 1;
//	
//	if(Uart_Buf_Index != 0)//表示接收到了数据
//	{
//		Uart_Buf_Index = 0;

//		sprintf(seg_string,"%s",Uart_Buf);			
//		Usart_Send_String(seg_string);
//	}
//}

