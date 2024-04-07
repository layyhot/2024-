/*
  ����˵��: IIC������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨ 8051��12MHz
  ��    ��: 2011-8-9
*/

//��΢��һ��
#include "bsp_iic.h"

#define DELAY_TIME 5

//ǿ������
//#define SlaveAddrW 0xA0
//#define SlaveAddrR 0xA1

//�������Ŷ���
sbit SDA = P2^1;  /* ������ */
sbit SCL = P2^0;  /* ʱ���� */

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//������������
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//����ֹͣ����
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//����Ӧ��
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0��Ӧ��1����Ӧ��
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//�ȴ�Ӧ��
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//ͨ��I2C���߷�������
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//��I2C�����Ͻ�������
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

//***************��һ������Ҫ�Լ�д---�����������ݣ��Աȼ���
unsigned char Pcf8591_Adc(unsigned char channel_num_contrl)
{
	unsigned char temp;
	
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	
	IIC_SendByte(channel_num_contrl);
	IIC_WaitAck();	

	//-------------����д����ʽ���ʣ����ö�����ʽ����
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();

	temp = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	
	return temp;
}

void EEPROM_Read (unsigned char *EEPROM_String, unsigned char addr, unsigned char num)
{

	IIC_Start();
	IIC_SendByte(0XA0);
	IIC_WaitAck();
	
	IIC_SendByte(addr);
	IIC_WaitAck();	

	//-------------����д����ʽ���ʣ����ö�����ʽ����

	IIC_Start();
	IIC_SendByte(0XA1);
	IIC_WaitAck();

	while(num--)
	{
	
		*EEPROM_String++ = IIC_RecByte();
		if(num) 	IIC_SendAck(0);
		else 	IIC_SendAck(1);
	}
	
	IIC_Stop();
}




//***************��һ������Ҫ�Լ�д---����д�����ݣ��Աȼ���
void Pcf8591_Dac(unsigned char trans_dat)
{

	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	
	IIC_SendByte(0x41);
	IIC_WaitAck();	
	
	IIC_SendByte(trans_dat);
	IIC_WaitAck();	
	IIC_Stop();
}

void EEPROM_Write(unsigned char *EEPROM_String, unsigned char addr, unsigned char num)
{
	IIC_Start();
	IIC_SendByte(0xA0);
	IIC_WaitAck();
	
	IIC_SendByte(addr);
	IIC_WaitAck();	
	
	while(num--)
	{
		IIC_SendByte(*EEPROM_String++);	
		IIC_WaitAck();	
		IIC_Delay(200);
	}
	IIC_Stop();
}



