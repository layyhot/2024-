#include<stc15.h>
#include<iic.h>
#include<intrins.h>
#define FOSC 12000000L
typedef unsigned int uint;
typedef unsigned char uchar;

uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF};
uchar menu1[8];
uchar menu2[8];

bit menu_flag=1;
uint freq,freq_cnt;
bit finish=0;
bit volt_flag=0,led_flag=1;
bit smg_flag=1;

uchar Trg=0,Cont=0;
void BTN()
{
	uchar dat=P3^0xFF;
	Trg=dat&(dat^Cont);
	Cont=dat;
}

void freq_handle()
{
	ET0=1;
	ET1=1;TR0=1;
	freq=freq_cnt;
	freq_cnt=0;
}
void Delay20ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 216;
	k = 35;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void AllInit()
{
	P2=0x80;P0=0xFF;
	P2=0xA0;P0=0x00;
	P2=0xC0;P0=0xFF;
	P2=0xE0;P0=0xFF;
}
void Timer0Init(void)		
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD = 0x04;		//���ö�ʱ��ģʽ
	TL0 = 0xff;		//���ö�ʱ��ֵ
	TH0 = 0xff;
	TR0=1;
	ET0=1;
}

void Timer1Init(void)		//50����@12.000MHz
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0xB0;		//���ö�ʱ��ֵ
	TH1 = 0x3C;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1=1;
}

void Timer2Init(void)		//1����@12.000MHz
{
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0x20;		//���ö�ʱ��ֵ
	T2H = 0xD1;		//���ö�ʱ��ֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	IE2 |=1<<2;
	EA=1;
}


bit key_flag;
void main()
{
	uint volt;
	AllInit();
	Timer0Init();
	Timer1Init();
	Timer2Init();
	while(1)
	{
		//��ȡ��ѹ
		if(menu_flag==1)
		{
			EA=0;
			volt=read_adc(0x03)/0.512;
			EA=1;
			Delay20ms();
		}
		if(volt_flag==0)volt=200;
		//Ƶ�ʴ���Ԫ
		if(menu_flag==0 && finish==1)
		{
			finish=0;
			freq_handle();
		}
		//������Ԫ
		if(key_flag)
		{
			key_flag=0;
			BTN();
			if(Trg & 0x08)//s4
			{
				menu_flag=~menu_flag;
			}
			if(Trg & 0x04)//s5
			{
				volt_flag=~volt_flag;
			}
			if(Trg & 0x02)//s6
			{
				led_flag=~led_flag;
			}
			if(Trg & 0x01)//s7
			{
				smg_flag=~smg_flag;
			}
			
		}
		//����ܵ�Ԫ
		if(menu_flag==0)
		{	
			menu1[0]=0x8E;menu1[1]=0xFF;
			menu1[2]=tab[10];
			if(freq>10000)
			{
				menu1[3]=tab[freq/10000];
				menu1[4]=tab[freq%10000/1000];
				menu1[5]=tab[freq%1000/100];
				menu1[6]=tab[freq%100/10];
				menu1[7]=tab[freq%10];
			}else if(freq<10000 && freq>1000)
			{
				menu1[3]=tab[10];
				menu1[4]=tab[freq/1000];
				menu1[5]=tab[freq%1000/100];
				menu1[6]=tab[freq%100/10];
				menu1[7]=tab[freq%10];
			}else if(freq<1000 && freq>100)
			{
				menu1[3]=tab[10];
				menu1[4]=tab[10];
				menu1[5]=tab[freq/100];
				menu1[6]=tab[freq%100/10];
				menu1[7]=tab[freq%10];
			}else if(freq<100 && freq>10)
			{
				menu1[3]=tab[10];
				menu1[4]=tab[10];
				menu1[5]=tab[10];
				menu1[6]=tab[freq/10];
				menu1[7]=tab[freq%10];
			}
		}else if(menu_flag==1)
		{
			menu2[0]=0xc1;menu2[1]=0xFF;
			menu2[2]=0xFF;menu2[3]=0xFF;
			menu2[4]=0xFF;
			menu2[5]=tab[volt/100]&0x7F;
			menu2[6]=tab[volt%100/10];
			menu2[7]=tab[volt%10];
		}
	}
}
void Timer0()interrupt 1
{
	freq_cnt++;
}
uchar time_cnt;
void Timer1()interrupt 3
{
	time_cnt++;
	if(time_cnt==20)
	{
		finish=1;
		ET0=0;
		ET1=0;TR0=0;
		time_cnt=0;
	}
}

uchar smg_cnt,i;
uchar key_cnt;
void Timer2()interrupt 12
{
	key_cnt++;
	if(key_cnt==10)
	{
		key_cnt=0;
		key_flag=1;
	}
	if(smg_flag)
	{
		smg_cnt++;
		if(smg_cnt==2)
		{
			smg_cnt=0;
			P2=0xC0;P0=0x01<<i;P2=0;
			if(menu_flag==0){P2=0xE0;P0=menu1[i];P2=0;}
			if(menu_flag==1){P2=0xE0;P0=menu2[i];P2=0;}
			i++;
			if(i==8)i=0;
		}
	}else 
	{
		smg_cnt=0;
		P2=0xC0;P0=0xFF;P2=0;
		P2=0xE0;P0=0xFF;P2=0;
	}
}