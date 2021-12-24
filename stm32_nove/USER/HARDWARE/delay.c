/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 16:20:29
 * @LastEditTime: 2021-12-09 16:20:55
 * @Brief:  
 */

#include "delay.h"

#define SysTick_CTRL_ENABLE_Pos             0                               //SysTick CTRL: ENABLE Position 
#define SysTick_CTRL_ENABLE_Msk            (1ul << SysTick_CTRL_ENABLE_Pos)	//SysTick CTRL: ENABLE Mask
void Delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*9;						//ʱ�����  		 
	SysTick->VAL=0x00;        					//��ռ�ʱ��
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����ʱ
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ���ʱ��
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�ʱ��
	SysTick->VAL =0X00;      					//��ռ�ʱ�� 
}

void Delay_ms(u16 nms)
{
	Delay_us(nms*1000);
}
