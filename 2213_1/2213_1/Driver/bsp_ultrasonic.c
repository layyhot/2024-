#include "bsp_ultrasonic.h"

sbit Tx = P1^0;
sbit Rx = P1^1;

void Ultrasonic_Timer0Init(void)
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xF4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 0;		//��ʱ��0������ʼ��ʱ
}


unsigned char Wave_Recv(void)
{
	unsigned char Wave_Num = 10;
  unsigned char Dist;
	
	//��ɳ������ķ���
	Tx = 0;
	TL0 = 0xF4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ	
	
	TR0 = 1;
	while( Wave_Num-- )
	{
		
		while(!TF0);
		Tx ^= 1;
		TF0 = 0;
	}
	
	
	//��ɳ������Ľ���
	TR0 = 0;	//�ص���ʱ����Ŀ����Ϊ������װ��
	TL0 = 0;		//���ö�ʱ��ֵ�����ڼ�ʱ
	TH0 = 0;		//���ö�ʱ��ֵ		
	TR0 = 1;	//�ص���ʱ����Ŀ����Ϊ������װ��	
	
	while(Rx && (~TF0));
	TR0 = 0;	//�رյ�Ŀ���Ǽ������
	if(TF0 == 1)//��ʾ�����
	{
		Dist = 250;//��ʾ����̫���ˣ��в���ΪʲôҪ������ô���ĸ߶�
		TF0 = 0;
	}
	else //û�в������
	{
		Dist = (((TH0<<8) | TL0) * 0.017);
	}
	return Dist;
	
}


