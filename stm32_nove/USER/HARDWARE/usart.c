/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 09:25:47
 * @LastEditTime: 2021-12-14 13:36:18
 * @Brief:  
 */
#include "usart.h"

Usart_Data_Typedef sUsartData;

/**
  * @brief  ��ʼ��Uart1��IO��
  * @param  None
  * @retval : None
  */
void Uart1_GPIO_Config(void)
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 ʹ��IO�˿����� */    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;									//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;							//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);   										//��ʼ��GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;							//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   										//��ʼ��GPIOA
}

/**
  * @brief  ����Uart1��ģʽ
  * @param  None
  * @retval : None
  */
void Uart1_Mode_Config(void)
{
	USART_InitTypeDef 		USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* USART1 ����ģʽ���� */
	USART_InitStructure.USART_BaudRate = 19200;									//���������ã�115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//����λ�����ã�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//ֹͣλ���ã�1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;  							//�Ƿ���żУ�飺��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ģʽ���ã�û��ʹ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�����뷢�Ͷ�ʹ��
	USART_Init(USART1, &USART_InitStructure);  										//��ʼ��USART1
}

/**
  * @brief  ����Uart1���жϷ������Լ����ȼ�
  * @param  None
  * @retval : None
  */
void Uart1_NVIC_Config(void)
{
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;				//������
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;					//�ж�ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//�����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;				//�����ȼ�
	NVIC_Init(&NVIC_InitStruct);
	
	//USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);		//��������жϵı�־
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}

/**
  * @brief  ��ʼ��UART1
  * @param  None
  * @retval : None
  */
void Uart1_Init(void)
{
	Uart1_GPIO_Config();
	Uart1_Mode_Config();
	Uart1_NVIC_Config();
	USART_Cmd(USART1, ENABLE);														// USART1ʹ��	
}


/**
 * @bried:  �ж������շ�
 * @para:  None
 * @retval:  None
 */
void USART1_IRQHandler(void)
{
  //-------------------------��������----------------------------------
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
  {
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);		//��������жϵı�־
    sUsartData.ReveiveTimeCnt = 0;

    if(sUsartData.eReveiveState != REVEIVE_GOING)
    {
      sUsartData.ReveiveLength = 0;
      sUsartData.eReveiveState = REVEIVE_GOING;
    }
    sUsartData.ReveiveBuff[sUsartData.ReveiveLength++] = (uint8_t )USART_ReceiveData(USART1);
  }
  //-------------------------��������------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_TXE)!=RESET)  //����buffΪ�յ�
	{
    if(sUsartData.eSendState!=SEND_GOING)
    {
      sUsartData.SendNowLength = 0;
      sUsartData.eSendState = SEND_GOING;
    }
    if(sUsartData.SendNowLength < sUsartData.SendAllLength)
      USART_SendData(USART1, sUsartData.SendBuff[sUsartData.SendNowLength]);
    else if(sUsartData.SendNowLength == sUsartData.SendAllLength)
    {
      sUsartData.eSendState = SEND_IDLE;
      sUsartData.SendNowLength = 0;
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);  //������ɣ��ؿ�buff�ж�
    }
	}
	//USART_ClearITPendingBit(USART1,USART_IT_RXNE);		//��������жϵı�־
}

/**
 * @bried:  ����ʶ�����ݽ������
 * @para:  None
 * @retval:  None
 */
void F_ReveiveEndInteruptCallback(void)
{
	if(sUsartData.eReveiveState == SEND_GOING)
	{
		if(++sUsartData.ReveiveTimeCnt>REVEIVE_TIME_OUT) 
		{
			sUsartData.ReveiveTimeCnt = 0;
			sUsartData.eReveiveState = REVEIVE_COMPLETE;	//���ݽ������
		}
	}
}


/**
 * @bried:  �жϷ��ʹ�������
 * @param {uint8_t} *buff
 * @param {uint8_t} length
 * @retval:
 */
void F_SendData(uint8_t *buff, uint8_t length)
{
  //------------��������----------------
	for(uint8_t i=0;i<length;i++)			
	{
    sUsartData.SendBuff[i] = *buff;
		buff++;
	}
	sUsartData.SendAllLength = length;
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);		//��ʼ��������
}


int fputc(int ch, FILE *f)  
{  
 /* e.g.��USARTдһ���ַ� */  
 USART_SendData(USART1, (uint8_t) ch);  
  
 /* ѭ��ֱ��������� */  
 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
  
 return ch;  
}  

