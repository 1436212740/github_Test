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


#define REVEIVE_LENGTH      10      //�ɽ���������ݳ���
#define SEND_LENGTH        10      //���ɷ������ݳ���

typedef enum
{
  REVEIVE_IDLE=0,   //����
  REVEIVE_GOING,    //������
  REVEIVE_COMPLETE  //�������
}REVEIVE_STATE;


typedef enum
{
  SEND_IDLE=0,
  SEND_GOING,
  SEND_COMPLETE
}SEND_STATE;


typedef struct 
{
  //------��������-----------
  #define REVEIVE_TIME_OUT    5
  REVEIVE_STATE eReveiveState;          //���ձ�־
  uint8_t ReveiveBuff[REVEIVE_LENGTH];    //����
  uint8_t ReveiveLength;      //�������ݳ���
  uint16_t ReveiveTimeCnt;    //ֹͣһ֡���ݽ��յ�ʱ����
  
  //------��������-----------
  SEND_STATE    eSendState;             //���ͳɹ���־
  uint8_t SendBuff[SEND_LENGTH];       //����
  uint8_t SendAllLength;         //���������ܳ���
  uint8_t SendNowLength;          //�Ѿ����͵����ݳ���
  
}Usart_Data_Typedef;
extern Usart_Data_Typedef sUsartData;

void Uart1_Init(void);
void F_SendData(uint8_t *buff, uint8_t length);
void F_ReveiveEndInteruptCallback(void);

#endif
