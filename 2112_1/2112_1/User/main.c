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

//--考察EEPROM/AD/DA时用
#include "bsp_iic.h"



//三大金刚函数声明区
void Key_Proc(void);
void Seg_Proc(void);
void Led_Proc(void);
void Voltage_Output_Proc(void);

//根据考核内容，其他函数声明区

//变量区
//--定时器滴答变量
unsigned long ms_Tick = 0;

//--定时器减速变量
unsigned int Key_Slow_Down;
unsigned int Seg_Slow_Down;
unsigned int Led_Slow_Down;
unsigned int Volatge_Output_Slow_Down;

//--按键专用变量
unsigned char Key_Value;
unsigned char Key_Old, Key_Down;

//--数码管专用变量
unsigned char seg_string[10];
unsigned char seg_buf[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char pos;

//--LED专用变量
unsigned char ucLed = 0x00;



/***用户自定义变量区开始↓***/
unsigned char Disp_Interface = 0;//显示界面，0-温度显示，1-参数设置，2-DA输出界面
unsigned char Temp_Compare_Disp = 25;//温度比较值,单纯用来显示和设置时候使用
unsigned char Temp_Compare_Ctrl = 25;//温度比较值,真正起作用的
bit DA_Output_Mode = 0;//DA输出的模式，0-模式1；1-模式2
float Temp;//真实的温度数据
unsigned char DA_Inter_Num = 0;//控制DA输出电压的内部给定数值，0-255

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
	
	rd_temperature();
	Delay750ms();

	
	while(1)
	{
		Key_Proc();//三大金刚子函数
		Seg_Proc();
		Led_Proc();
		Voltage_Output_Proc();
			
	}
}



/* Timer1_interrupt routine */
void tm1_isr() interrupt 3
{
	ms_Tick++;//滴答定时器，能记录的数据长达29天
	
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;//减速变量区，控制子函数的刷新频率
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;	
	if(++Led_Slow_Down == 100) Led_Slow_Down = 0;	
	if(++Volatge_Output_Slow_Down == 500) Volatge_Output_Slow_Down = 0;	
	

	
	
	
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
		case 4://显示界面切换按键，0-温度显示，1-参数设置，2-DA输出界面
				if(++Disp_Interface == 3) Disp_Interface = 0;
				if(Disp_Interface == 2) Temp_Compare_Ctrl = Temp_Compare_Disp;//使能设定的比较值
				break;//这个break很重要
		
		case 8://参数设置数值-
				if(Disp_Interface == 1) 
				{
					Temp_Compare_Disp--;
					if(Temp_Compare_Disp > 200)//将下限设置为0
						Temp_Compare_Disp = 0;
				}
					break;//这个break很重要	
				
		case 9:///参数设置数值+
				if(Disp_Interface == 1) 
				{
					Temp_Compare_Disp++;
					if(Temp_Compare_Disp > 99)//将上限设置为99
						Temp_Compare_Disp = 99;
				}
					break;//这个break很重要		
				
				
		case 5://DA输出的模式切换，0-模式1；1-模式2
				DA_Output_Mode ^= 1;
				break;
		
	}
	/***用户自定义代码区↑***/		

	
}





void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/***用户自定义代码区↓***/	
	
//	sprintf(seg_string,"%1d  %02d  %1d",(unsigned int)Disp_Interface,(unsigned int)Temp_Compare_Disp,(unsigned int)DA_Output_Mode);	
	
//	if(state_flag == 0)
//	{
//		sprintf(seg_string,"12345678");	
//  }		
//	else //屏幕上显示的内容，在这个地方，就等同于seg_string中放了什么内容，你敢放什么，我就敢显示什么，不敢装逼，只限制于seg能显示的范围。
//	{
//		//--温度传感器专用
//		sprintf(seg_string,"     %03d",(unsigned int)(rd_temperature()/16.0));	
//		
//		//--DA测试
//		Pcf8591_Dac(255);

//	}
	
	Temp = rd_temperature()/16.0;//采集温度数据
	
	switch(Disp_Interface)//显示界面切换按键，0-温度显示，1-参数设置，2-DA输出界面
	{
		case 0://温度显示
 	  	sprintf(seg_string,"C   %5.2f",Temp);
			break;		
		
		case 1://参数设置
			sprintf(seg_string,"P     %02d",(unsigned int)Temp_Compare_Disp);	
			break;			
		
		case 2://DA输出界面
			sprintf(seg_string,"A    %4.2f",DA_Inter_Num/51.0);	
			break;		
	}
	
	
	
	
	/***用户自定义代码区↑***/			
	
	Seg_Tran(seg_string, seg_buf);
}






void	Led_Proc(void)
{
	if(Led_Slow_Down) return;
	Led_Slow_Down = 1;
	
	
	/***用户自定义代码区↓***/		
 	if(DA_Output_Mode == 0)//0-模式1，L1点亮；1-模式2，L1熄灭
		ucLed |= 0x01;			
	else 
	 	ucLed &= (~0x01);	
	
	
	switch(Disp_Interface)//显示界面切换按键，0-温度显示，1-参数设置，2-DA输出界面
	{
		case 0://温度显示,L2亮
			ucLed &= (~0x0E);
			ucLed |= 0x02;		
			break;		
		case 1://参数设置,L3亮
			ucLed &= (~0x0E);			
			ucLed |= 0x04;
			break;				
		case 2://DA输出界面,L4亮
			ucLed &= (~0x0E);			
			ucLed |= 0x08;
			break;		
	}	
	
	/***用户自定义代码区↑***/			

}


void Voltage_Output_Proc(void)
{
	if(Volatge_Output_Slow_Down) return;
	Volatge_Output_Slow_Down = 1;	
	
	if(DA_Output_Mode == 0)//0-模式1；1-模式2
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


