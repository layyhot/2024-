#include "bsp_init.h"

void Cls_Peripheral(void)
{
	
	P0 = 0XFF;
	P2 = P2 & 0X1F | 0x80;//led
	P2 &= 0X1F;
	
	P0 = 0X00;
	P2 = P2 & 0X1F | 0xA0;//beep relay
	P2 &= 0X1F;

}





