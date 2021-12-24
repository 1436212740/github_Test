/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 09:26:24
 * @LastEditTime: 2021-12-20 18:55:22
 * @Brief:  
 */
#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"
#include <stdbool.h>
#include "stdio.h"

#define  DEBUG            1
#define  PRINGF_ERROR     1
#if DEBUG
#define debug(...)  printf(__VA_ARGS__)
#else
#define debug(...)
#endif

#if  PRINGF_ERROR
#define error_message()     printf("[%s:%u]%s\n", __FILE__, __LINE__, __FUNCTION__)
#define printf_error(...)   {error_message();printf(__VA_ARGS__);}
#else
#define printf_error(...)
#endif


#define REVEIVE_LENGTH      10      //可接收最大数据长度
#define SEND_LENGTH        10      //最大可发送数据长度

typedef enum
{
  REVEIVE_IDLE=0,   //空闲
  REVEIVE_GOING,    //进行中
  REVEIVE_COMPLETE  //数据完成
}REVEIVE_STATE;


typedef enum
{
  SEND_IDLE=0,
  SEND_GOING,
  SEND_COMPLETE
}SEND_STATE;


typedef struct 
{
  //------接受数据-----------
  #define REVEIVE_TIME_OUT    5
  REVEIVE_STATE eReveiveState;          //接收标志
  uint8_t ReveiveBuff[REVEIVE_LENGTH];    //数据
  uint8_t ReveiveLength;      //接收数据长度
  uint16_t ReveiveTimeCnt;    //停止一帧数据接收的时间间隔
  
  //------发送数据-----------
  SEND_STATE    eSendState;             //发送成功标志
  uint8_t SendBuff[SEND_LENGTH];       //数据
  uint8_t SendAllLength;         //发送数据总长度
  uint8_t SendNowLength;          //已经发送的数据长度
  
}Usart_Data_Typedef;
extern Usart_Data_Typedef sUsartData;

void Uart1_Init(void);
void F_SendData(uint8_t *buff, uint8_t length);
void F_ReveiveEndInteruptCallback(void);

#endif
