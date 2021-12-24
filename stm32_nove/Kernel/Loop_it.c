/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 15:30:23
 * @LastEditTime: 2021-12-14 13:37:17
 * @Brief:  
 */
#include "Loop.h"

/**
  * @brief  设置timer3定时器的基本功能
  * @param  None
  * @retval : None
  */
void Timer3_Base_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);        			//时钟开启	
	
	TIM_DeInit(TIM3);	
	
	TIM_TimeBaseStructure.TIM_Period =TIMER3_PEIOD_CODE; //0x1C4E<<1;  		       	    	//定时器顶端值设定200us
 
	TIM_TimeBaseStructure.TIM_Prescaler =TIMER3_PRESCALER_CODE ;		//1    										//定时器分频值设定
 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    			//忽略
 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//向上计时
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
 
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseStructure);           			//结束配置
		
	TIM_ARRPreloadConfig(TIM3, ENABLE);	                       			//开关，使能定时器计数功能
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);

	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE );	        				//开关 使能update中断捕获触发中断

	TIM_Cmd(TIM3, ENABLE);                                     			//定时器 总开关
}

/**
  * @brief  设置TIMER2的中断服务函数以及优先级
  * @param  None
  * @retval : None
  */
void Timer3_NVIC_Config(void)
{
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel =  TIM3_IRQn;            			//中断对象_Tim6
 
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  			//先占优先级0级
 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         			//从优先级3级
 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;            			//IRQ通道
 
	NVIC_Init(&NVIC_InitStruct);                            			//
}


/**
  * @brief  设置TIMER2初始化
  * @param  None
  * @retval : None
  */
void F_LoopIt_Init(void)
{
	Timer3_Base_Config();
	Timer3_NVIC_Config();
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
    static uint16_t	time_cnt;
    //==========主定时操作函数=============
    
    
    if(++time_cnt>=(500/TIMER3_TIME))		//500us
    {
      time_cnt = 0;
      F_LoopTimeInterruptCall();
    }
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}


