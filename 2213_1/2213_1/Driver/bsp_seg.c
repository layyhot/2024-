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
			case 'F': temp = 0x8e; break;	//����ʮ����������
			
			case 'H': temp = 0x89; break;			
			case 'L': temp = 0xc7; break;
			case 'N': temp = 0xc8; break;			
			case 'P': temp = 0x8c; break;
			case 'U': temp = 0xc1; break;	//����Ů����	

			case '-': temp = 0xbf; break;			
			case ' ': temp = 0xff; break;	//�����������

      default: temp = 0xff; break;		//Ĭ��
		}
		
		if(seg_string[j+1] == '.')//�ж�С����
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
	P2 = P2 & 0X1F | 0xc0;//λ��ѡͨ
	P2 &= 0X1F;

	P0 = 0xff;
	P2 = P2 & 0X1F | 0xe0;//����ѡͨ������
	P2 &= 0X1F;

	P0 = seg_buf[pos];
	P2 = P2 & 0X1F | 0xe0;//����ѡͨ
	P2 &= 0X1F;
}










