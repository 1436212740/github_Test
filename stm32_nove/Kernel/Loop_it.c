/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 15:30:23
 * @LastEditTime: 2021-12-14 13:37:17
 * @Brief:  
 */
#include "Loop.h"

/**
  * @brief  ����timer3��ʱ���Ļ�������
  * @param  None
  * @retval : None
  */
void Timer3_Base_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);        			//ʱ�ӿ���	
	
	TIM_DeInit(TIM3);	
	
	TIM_TimeBaseStructure.TIM_Period =TIMER3_PEIOD_CODE; //0x1C4E<<1;  		       	    	//��ʱ������ֵ�趨200us
 
	TIM_TimeBaseStructure.TIM_Prescaler =TIMER3_PRESCALER_CODE ;		//1    										//��ʱ����Ƶֵ�趨
 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    			//����
 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//���ϼ�ʱ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
 
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseStructure);           			//��������
		
	TIM_ARRPreloadConfig(TIM3, ENABLE);	                       			//���أ�ʹ�ܶ�ʱ����������
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);

	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE );	        				//���� ʹ��update�жϲ��񴥷��ж�

	TIM_Cmd(TIM3, ENABLE);                                     			//��ʱ�� �ܿ���
}

/**
  * @brief  ����TIMER2���жϷ������Լ����ȼ�
  * @param  None
  * @retval : None
  */
void Timer3_NVIC_Config(void)
{
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel =  TIM3_IRQn;            			//�ж϶���_Tim6
 
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  			//��ռ���ȼ�0��
 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         			//�����ȼ�3��
 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;            			//IRQͨ��
 
	NVIC_Init(&NVIC_InitStruct);                            			//
}


/**
  * @brief  ����TIMER2��ʼ��
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
    //==========����ʱ��������=============
    
    
    if(++time_cnt>=(500/TIMER3_TIME))		//500us
    {
      time_cnt = 0;
      F_LoopTimeInterruptCall();
    }
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}


