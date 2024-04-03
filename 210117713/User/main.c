#include "reg52.h"
#include "onewire.h"
#include "ds1302.h"  

sbit R1 = P3^3;
sbit R2 = P3^2;
sbit C1 = P3^5;
sbit C2 = P3^4;


unsigned char code SMG_NoDot[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
unsigned char code SMG_Dot[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};

unsigned char Read[7] = {0x81,0x83,0x85,0x87,0x89,0x8a,0x8c};
unsigned char Write[7] = {0x80,0x82,0x84,0x86,0x88,0x8b,0x8d};
unsigned char Time[7] = {0x30,0x59,0x23,0x12,0x03,0x07,0x23};


unsigned char stat_smg = 1;
unsigned char mode = 0;//����ģʽ 

unsigned char T_h = 0;
unsigned char T_m = 0;
unsigned char T_s = 0;

unsigned char stat_jdq = 0x00;
unsigned char stat_led = 0xff;
unsigned int count_100ms = 0;
unsigned char T_5s = 0;
unsigned int count_5s = 0;

float temp_ds18b20 = 0;//ʵ���¶�
unsigned int smg_ds18b20 = 0;//ת���¶�
unsigned int smg_temp = 23;//�¶Ȳ���

//==========�������ʾ======
void Delay_SMG(unsigned int t)
{
	while(t--);
}

void Display_SMG_Bit(unsigned char pos,unsigned char value)
{
	P2 = (P2 & 0x1f) | 0xc0;
	P0 = 0x01 << pos;
	P2 &= 0x1f;
	
	P2 = (P2 & 0x1f) | 0xe0;
	P0 = value;
	P2 &= 0x1f;
	
	Delay_SMG(500);
	
	P2 = (P2 & 0x1f) | 0xc0;
	P0 = 0x01 << pos;
	P2 &= 0x1f;
	
	P2 = (P2 & 0x1f) | 0xe0;
	P0 = 0xff;
	P2 &= 0x1f;
	
}

void Display_SMG()
{
	switch(stat_smg)
	{
		case 1: //U1 �¶���ʾ����
			Display_SMG_Bit(0,0xc1);
			Display_SMG_Bit(1,SMG_NoDot[1]);
			
			Display_SMG_Bit(5,SMG_NoDot[smg_ds18b20 / 100]);
			Display_SMG_Bit(6,SMG_Dot[(smg_ds18b20 % 100) / 10]);
			Display_SMG_Bit(7,SMG_NoDot[smg_ds18b20 % 10]);
		break;
		
		case 2://U2  ʱ����ʾ����--ʱ����
			Display_SMG_Bit(0,0xc1);
			Display_SMG_Bit(1,SMG_NoDot[2]);
		
			Display_SMG_Bit(3,SMG_NoDot[T_h / 16]);
			Display_SMG_Bit(4,SMG_NoDot[T_h % 16]);
			Display_SMG_Bit(5,0xbf);
			Display_SMG_Bit(6,SMG_NoDot[T_m / 16]);
			Display_SMG_Bit(7,SMG_NoDot[T_m % 16]);
		break;
		
		case 3://U3 �������ý���
			Display_SMG_Bit(0,0xc1);
			Display_SMG_Bit(1,SMG_NoDot[3]);
		
			Display_SMG_Bit(6,SMG_NoDot[smg_temp / 10]);
			Display_SMG_Bit(7,SMG_NoDot[smg_temp % 10]);
		break;
		
		case 4://ʱ����ʾ--�֣���
			Display_SMG_Bit(0,0xc1);
			Display_SMG_Bit(1,SMG_NoDot[2]);
		
			Display_SMG_Bit(3,SMG_NoDot[T_m / 16]);
			Display_SMG_Bit(4,SMG_NoDot[T_m % 16]);
			Display_SMG_Bit(5,0xbf);
			Display_SMG_Bit(6,SMG_NoDot[T_s / 16]);
			Display_SMG_Bit(7,SMG_NoDot[T_s % 16]);
		break;
	}
}
//==========��ʼ��DS1302====
void Init_DS1302()
{
	unsigned char i;
	Write_Ds1302_Byte( 0x8e,0x00 );//�Ӵ�д����
	for(i = 0;i < 7;i++)
	{
		Write_Ds1302_Byte( Write[i],Time[i] );
	}
	Write_Ds1302_Byte( 0x8e,0x80 );//ʹ��д����
}

//=========��ȡDS1302=======
void Read_Ds1302()
{
	T_s = Read_Ds1302_Byte(Read[0]);
	T_m = Read_Ds1302_Byte(Read[1]);
	T_h = Read_Ds1302_Byte(Read[2]);
	
	if(T_m == 0 && T_s == 0)
	{
		stat_led &= ~0x01; 
		P2 = (P2 & 0x1f) | 0x80;
		P0 = stat_led;//L1 ����5��
		P2 &= 0x1f;
		
		//����5��
		T_5s = 1;
	}
}

//===========��ʱ��T0======
void Init_T0()
{
	TMOD = 0x01;
	TH0 = (65536 - 50000) / 256;//��ʱ50ms
	TL0 = (65536 - 50000) % 256;
	
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}

void SeviceT0() interrupt 1
{
	TH0 = (65536 - 50000) / 256;//��ʱ50ms
	TL0 = (65536 - 50000) % 256;
	
	Read_Ds1302(); 				//ÿ50ms����һ��ʱ����
	if(stat_jdq == 0x10)	//����̵�������
	{
		count_100ms++;
		if(count_100ms == 2)//L3��0.1s����
		{
			count_100ms = 0;
			if((stat_led & 0x04) == 0x04)//L3Ϩ��
			{
				stat_led &= ~0x04;//����L3
			}
			else
			{
				stat_led |= 0x04;//Ϩ��L3
			}
			P2 = (P2 & 0x1f) | 0x80;
			P0 = stat_led;
			P2 &= 0x1f;
		}
	}
	if(T_5s == 1)
	{
		count_5s++;
		if(mode == 1)//��ʱ�����ģʽ
		{
			stat_jdq = 0x10;
		}
		if(count_5s == 100)//5s
		{
			count_5s = 0;
			T_5s = 0;
			stat_jdq = 0x00;
			stat_led |= 0x05;//L1,L3Ϩ��
			P2 = (P2 & 0x1f) | 0x80;
			P0 = stat_led;//L1 
			P2 &= 0x1f;
		}
		P2 = (P2 & 0x1f) | 0xa0;
		P0 = stat_jdq;//L1 ����5��
		P2 &= 0x1f;
	}
}
//==========DS18B20�¶ȶ�������ʾ===
void Init_DS18B20_Temp()
{
	unsigned char LSB,MSB;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	do
	{
		init_ds18b20();
		Write_DS18B20(0xcc);
		Write_DS18B20(0xbe);
		LSB = Read_DS18B20();
		MSB = Read_DS18B20();
		MSB = (MSB << 4) | (LSB >> 4);
	}while(MSB == 85);
}
void Delay_DS18B20(unsigned int t)
{
	while(t--)
	{
		Display_SMG();
	}
}
void Read_DS18B20_Temp()
{
	unsigned char LSB,MSB;
	unsigned int temp = 0;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	Display_SMG();
	
	init_ds18b20();
	
	Display_SMG();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = MSB;
	temp = (temp << 8) | LSB;
	if((temp & 0xf800) == 0x0000)
	{
		temp_ds18b20 =  temp * 0.0625;
	}
	smg_ds18b20 = temp_ds18b20 * 10;
	Display_SMG();
	
	if(mode == 0)
	{
		if(smg_ds18b20 > smg_temp * 10)
		{
			stat_jdq = 0x10;
		}
		else
		{
			stat_jdq = 0x00;
			stat_led |= 0x04;
		}
		P2 = (P2 & 0x1f) | 0x80;
		P0 = stat_led;
		P2 &= 0x1f;
		
		P2 = (P2 & 0x1f) | 0xa0;
		P0 = stat_jdq;
		P2 &= 0x1f;
	}
}

//==========�������========
void Scan_Keys()
{
	C1 = 1;
	C2 = 1;
	R1 = 0;
	R2 = 1;
	if(C1 == 0)//S12����
	{
		
			switch(stat_smg)
			{
				case 1:
					stat_smg = 2;
				break;
				
				case 2:
					stat_smg = 3;
				break;
				
				case 3:
					stat_smg = 1;
				break;
			}
			while(C1 == 0)
			{
				if(stat_smg == 1)
				{
					Read_DS18B20_Temp();//��Ӱ�����ݲɼ�
				}
				Display_SMG();
			}
		
	}
	if(C2 == 0)//S16����
	{
		
			if(stat_smg == 3);//�����ò�������Ч
			{
				if(smg_temp == 99)
				{
					smg_temp = 99;
				}
				else
				{
					smg_temp++;
				}
				while(C2 == 0)
				{
					Display_SMG();
				}
			}
		
	}
	
	C1 = 1;
	C2 = 1;
	R1 = 1;
	R2 = 0;
	if(C1 == 0)//S13����
	{
		
		if(stat_smg == 3)
		{
				
			
			if(mode == 0)
			{
				mode = 1;
				if(T_5s == 0)
				{
					stat_jdq = 0x00;
				}
				stat_led |= 0x02;//L2Ϩ��
			}
			else if(mode == 1)
			{
				mode = 0;
				if(T_5s == 1)
				{
					if(temp_ds18b20 <= smg_temp * 10)
					{
						stat_jdq = 0x00;
					}
				}
				stat_led &= ~0x02;//L2����
			}
		}
			if(stat_jdq == 0x00)
			{
				stat_led |= 0x04;//L3Ϩ��
			}
			P2 = (P2 & 0x1f) | 0x80;
			P0 = stat_led;
			P2 &= 0x1f;
		
			P2 = (P2 & 0x1f) | 0xa0;
			P0 = stat_jdq;
			P2 &= 0x1f;
			while(C1 == 0)
			{
				Read_DS18B20_Temp();
				Display_SMG();
			}
		
	}
	if(C2 == 0)//S17����
	{
		
			if(stat_smg == 3);//�����ò�������Ч
			{
				smg_temp--;
				if(smg_temp == 10)
				{
					smg_temp = 10;
				}
				while(C2 == 0)
				{
					Display_SMG();
				}
			}
			
			if(stat_smg == 2)
			{
				stat_smg = 4;
				while(C2 == 0)
				{
					Display_SMG();
				}
				stat_smg = 2;
			}
	}
	
}

//==========ϵͳ��ʼ��======
void Init_Sys()
{
	P2 = (P2 & 0x1f) | 0x80;
	P0 = 0xff;
	P2 &= 0x1f;
	
	P2 = (P2 & 0x1f) | 0xa0;
	P0 = 0x00;
	P2 &= 0x1f;
	
	Init_DS18B20_Temp();
	mode = 0;
	
	stat_led &= ~0x02;
	P2 = (P2 & 0x1f) | 0x80;
	P0 = stat_led;
	P2 &= 0x1f;
	
	Init_DS1302();
	Init_T0();
}

//=========������
void main()
{
	Init_Sys();
	
	while(1)
	{
		Read_DS18B20_Temp();
		Scan_Keys();
	}
}