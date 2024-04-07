#include "bsp_led.h"

void Led_Disp(unsigned char ucLed)
{

	P0 = (~ucLed);
	P2 = P2 & 0X1F | 0x80;//led
	P2 &= 0X1F;

}




