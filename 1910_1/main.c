# include "bsp_init.h"

# include "bsp_seg.h"
# include "timer.h"
# include "bsp_key.h"
# include "bsp_led.h"

#include "bsp_iic.h"

# include "stdio.h"
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

//滴答定时专用
unsigned long ms_Tick=0;//上电之后会一直运行下去 ，49天才会产生溢出 

//根据代码需求发生变化
bit Vol_Fre_Disp_Mode = 0;//0-电压测量模式，1-频率测量模式
unsigned int Frequent = 0;//频率值

bit Seg_Dis_or_Die = 1;//控制数码管亮灭，0-灭，1-亮
bit LED_Dis_or_Die = 1;//控制LED亮灭，0-灭，1-亮

bit DAC_Output_Ctrl = 0;//控制DAC输出，0-固定2v，1-随着Rb2变化的模式
float AD_Num = 0;//指示灯专用的AD读取的数值	
	
	
//-----------------------------------------------
/* main */
void main()
{
	//根据代码需求发生变化
	Cls_Peripheral();//关闭外设
	Timer1Init();//定时器1初始化，并且使能定时器1中断，1ms进入一次
	EA = 1;//打开总中断

	Timer0Init_Count();//定时器0配置成外部输入模式，对外部输入进行频率计数

	while(1)
	{
		//永远不变
		Key_Proc();//按键处理，底层数据变更
		Seg_Proc();//显示处理，显示信息生成
    Led_Proc();//LED处理，LED状态信息表示		
		
	
	}


}



//-----------------------------------------------
/* Timer1_interrupt routine */
void tm1_isr() interrupt 3
{
	ms_Tick++;//上电自动++，持续运行 
		
	
	//永远不变
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 500) Seg_Slow_Down = 0;
	
	//取时间间隔
	if(!(ms_Tick%1000)) //每1s读数
	{
		Frequent = ((TH0<<8)|TL0);
		TL0 = 0;		//设置定时初值
		TH0 = 0;		//设置定时初值	
	}
	




	//永远不变
	Seg_Disp(seg_buf, pos);//数码管显示刷新
	if( ++pos == 8 ) 	pos = 0;
	
	Led_Disp(ucLed);//LED显示
	
	
}



//-----------------------------------------------
/*key_proc */

void Key_Proc(void)//按键处理，底层数据变更
{
	//永远不变
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//减速程序
	
	Key_Value = Key_Read_BTN();//读取按键按下的编号
	Key_Down = Key_Value & (Key_Old ^ Key_Value);// (0000^0101) = 0101     0101 & 0101 =0101  ,如果按键发生了下降沿的变化，输出结果和本次按键数值相同
																					// (0101^0101) = 0000       0101&0000 = 0000  如果按键一直保持同样的状态，输出结果为0
	Key_Old = Key_Value;
	
	
	
	
	//根据代码需求发生变化
	switch(Key_Down)//如果捕捉到下降沿跳变
	{
		case 4://电压模式和频率模式的切换
			Vol_Fre_Disp_Mode ^= 1;//0-电压模式，1-频率模式
			break;
			
		case 7://控制数码管关闭或者打开
			Seg_Dis_or_Die ^= 1;//0-灭，1-亮
			break;		
		
		case 5://控制DAC输出		
			DAC_Output_Ctrl^= 1;//0-固定2v，1-随着Rb2变化的模式
			break;			
		
		case 6://控制LED关闭或者打开
			LED_Dis_or_Die ^= 1;//0-灭，1-亮
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

	//借用你的地方，其实是DAC功能,0-固定2v，1-随着Rb2变化的模式
	if(DAC_Output_Ctrl == 0)
		Pcf8591_Dac(0x66);//固定输出2v
	else
	{
		Pcf8591_Dac(Pcf8591_Adc(Adj_Res_Channel));//先读过来，在转换出去
	}
	

	
	//根据代码需求发生变化

	if(Seg_Dis_or_Die)
	{
		if(Vol_Fre_Disp_Mode == 0)
			sprintf(seg_string,"U    %3.2f",Pcf8591_Adc(Adj_Res_Channel)/51.0);
		else 
			sprintf(seg_string,"F %6d",Frequent);//读取频率值	
	}	
	else 
			sprintf(seg_string,"        ");//不显示内容	
	

	
	//永远不变
	Seg_Tran(seg_string, seg_buf);
}

//----------------------------------------------
/*led_proc */
void Led_Proc(void)//显示处理，显示信息生成
{
	
	if(LED_Dis_or_Die)//控制LED亮灭，0-灭，1-亮
	{	
			//L1和L2两个灯控制电压模式还是频率模式
			if(Vol_Fre_Disp_Mode == 0)//0-电压模式 L1亮，L2灭；
			{
				ucLed &= (~0x03);//低两位清零
				ucLed |= 0x01; //L1亮
			}
			else//1-频率模式 L2亮，L1灭
			{
				ucLed &= (~0x03);//低两位清零
				ucLed |= 0x02; //L1亮
			}		
			
			//L3控制显示实时电压的大小，匹配灯，1.5-2.5-3.5，在1.5-2.5亮，>3.5亮
			AD_Num = Pcf8591_Adc(Adj_Res_Channel)/51.0;
			if(((AD_Num>=1.5)&&(AD_Num<2.5))||(AD_Num>=3.5))
				ucLed |= 0x04; //L3亮	
			else 
				ucLed &= (~0x04); //L3灭			

			
			//L4显示频率大小，匹配灯，大于等于1000且小于等于5000，或大于等于10000HZ亮
			if(((Frequent>1000)&&(Frequent<5000))||(Frequent>10000))
				ucLed |= 0x08; //L4亮	
			else 
				ucLed &= (~0x08); //L4灭	
			
			//L5输出模式，0-固定2v，熄灭；1-随着Rb2变化模式，亮。
			if(DAC_Output_Ctrl)
				ucLed |= 0x10; //L5亮			
			else
				ucLed &= (~0x10); //L5灭		
	}
	else//如果不允许输出，熄灭LED
		ucLed = 0;
	
	
	
	
	
}


