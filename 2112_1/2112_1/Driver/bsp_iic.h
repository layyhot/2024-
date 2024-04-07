//自己写
#include "STC15F2K60S2.H"
#include "intrins.h"

#define Photo_Res_Channel 0x41
#define Adj_Res_Channel 0x43


//给你的
void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 


//自己写
void EEPROM_Read (unsigned char *EEPROM_String, unsigned char addr, unsigned char num);
void EEPROM_Write(unsigned char *EEPROM_String, unsigned char addr, unsigned char num);
void Pcf8591_Dac(unsigned char trans_dat);
unsigned char Pcf8591_Adc(unsigned char channel_num_contrl);

