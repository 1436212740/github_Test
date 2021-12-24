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
	SysTick->LOAD=nus*9;						//时间加载  		 
	SysTick->VAL=0x00;        					//清空计时器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒计时
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待计时器
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计时器
	SysTick->VAL =0X00;      					//清空计时器 
}

void Delay_ms(u16 nms)
{
	Delay_us(nms*1000);
}
