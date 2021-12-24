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
  * @brief  初始化Uart1的IO口
  * @param  None
  * @retval : None
  */
void Uart1_GPIO_Config(void)
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 使用IO端口配置 */    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;									//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;							//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);   										//初始化GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;							//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   										//初始化GPIOA
}

/**
  * @brief  设置Uart1的模式
  * @param  None
  * @retval : None
  */
void Uart1_Mode_Config(void)
{
	USART_InitTypeDef 		USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* USART1 工作模式配置 */
	USART_InitStructure.USART_BaudRate = 19200;									//波特率设置：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No;  							//是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//接收与发送都使能
	USART_Init(USART1, &USART_InitStructure);  										//初始化USART1
}

/**
  * @brief  设置Uart1的中断服务函数以及优先级
  * @param  None
  * @retval : None
  */
void Uart1_NVIC_Config(void)
{
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;				//服务函数
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;					//中断使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;				//次优先级
	NVIC_Init(&NVIC_InitStruct);
	
	//USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);		//清除接收中断的标志
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}

/**
  * @brief  初始化UART1
  * @param  None
  * @retval : None
  */
void Uart1_Init(void)
{
	Uart1_GPIO_Config();
	Uart1_Mode_Config();
	Uart1_NVIC_Config();
	USART_Cmd(USART1, ENABLE);														// USART1使能	
}


/**
 * @bried:  中断数据收发
 * @para:  None
 * @retval:  None
 */
void USART1_IRQHandler(void)
{
  //-------------------------接收数据----------------------------------
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
  {
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);		//清除接收中断的标志
    sUsartData.ReveiveTimeCnt = 0;

    if(sUsartData.eReveiveState != REVEIVE_GOING)
    {
      sUsartData.ReveiveLength = 0;
      sUsartData.eReveiveState = REVEIVE_GOING;
    }
    sUsartData.ReveiveBuff[sUsartData.ReveiveLength++] = (uint8_t )USART_ReceiveData(USART1);
  }
  //-------------------------发送数据------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_TXE)!=RESET)  //传输buff为空的
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
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);  //传输完成，关空buff中断
    }
	}
	//USART_ClearITPendingBit(USART1,USART_IT_RXNE);		//清除接收中断的标志
}

/**
 * @bried:  用于识别数据接收完成
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
			sUsartData.eReveiveState = REVEIVE_COMPLETE;	//数据接收完成
		}
	}
}


/**
 * @bried:  中断发送串口数据
 * @param {uint8_t} *buff
 * @param {uint8_t} length
 * @retval:
 */
void F_SendData(uint8_t *buff, uint8_t length)
{
  //------------导入数据----------------
	for(uint8_t i=0;i<length;i++)			
	{
    sUsartData.SendBuff[i] = *buff;
		buff++;
	}
	sUsartData.SendAllLength = length;
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);		//开始发送数据
}


int fputc(int ch, FILE *f)  
{  
 /* e.g.给USART写一个字符 */  
 USART_SendData(USART1, (uint8_t) ch);  
  
 /* 循环直到发送完成 */  
 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
  
 return ch;  
}  

