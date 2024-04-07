#include "bsp_seg.h"

void Seg_Tran(unsigned char *seg_string, unsigned char *seg_buf)
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char temp;
	
	for(i = 0; i <= 7;i++,j++)
	{
		switch(seg_string[j])
		{
			case '0': temp = 0xc0; break;
			case '1': temp = 0xf9; break;			
			case '2': temp = 0xa4; break;
			case '3': temp = 0xb0; break;			
			case '4': temp = 0x99; break;
			case '5': temp = 0x92; break;			
			case '6': temp = 0x82; break;
			case '7': temp = 0xf8; break;	
			case '8': temp = 0x80; break;
			case '9': temp = 0x90; break;			
			case 'A': temp = 0x88; break;
			case 'B': temp = 0x83; break;			
			case 'C': temp = 0xc6; break;
			case 'D': temp = 0xa1; break;			
			case 'E': temp = 0x86; break;
			case 'F': temp = 0x8e; break;	//所有十六进制数据
			
			case 'H': temp = 0x89; break;			
			case 'L': temp = 0xc7; break;
			case 'N': temp = 0xc8; break;			
			case 'P': temp = 0x8c; break;
			case 'U': temp = 0xc1; break;	//回来女仆呦	

			case '-': temp = 0xbf; break;			
			case ' ': temp = 0xff; break;	//两种特殊情况

      default: temp = 0xff; break;		//默认
		}
		
		if(seg_string[j+1] == '.')//判断小数点
		{
			temp &= 0x7f;
			j++;
		}
		
		seg_buf[i] = temp;
	}
}


void Seg_Disp(unsigned char *seg_buf, unsigned char pos)
{	
	P0 = 1<<pos;
	P2 = P2 & 0X1F | 0xc0;//位码选通
	P2 &= 0X1F;

	P0 = 0xff;
	P2 = P2 & 0X1F | 0xe0;//段码选通，消隐
	P2 &= 0X1F;

	P0 = seg_buf[pos];
	P2 = P2 & 0X1F | 0xe0;//段码选通
	P2 &= 0X1F;
}










