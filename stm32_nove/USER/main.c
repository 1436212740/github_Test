/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-11-27 11:02:20
 * @LastEditTime: 2021-12-16 14:17:27
 * @Brief:  
 */
#include "stm32f10x.h"
#include "BaseDrive.h"
#include "usart.h"
#include "Loop.h"
#include "ParamCali.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//ÖÐ¶Ï×éºÅ
	SystemInit();
	Delay_ms(1000);
	Uart1_Init();
	F_LoopIt_Init();
	F_TVL5618_Init();	
	F_ADS112C04_Init();
	F_ParamCali_Init();
	while(1)
	{
		F_LoopTimeLoopCall();
	}
}
