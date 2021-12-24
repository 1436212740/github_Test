/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-07 18:28:03
 * @LastEditTime: 2021-12-17 13:39:12
 * @Brief:  
 */
#ifndef __TLV5618_H__
#define __TLV5618_H__

#include "stm32f10x.h"
#include <stdbool.h>
#include "Hardware_Config.h"

//=======================================================
//======================接口配置=========================
//=======================================================
#define TLV5618_SCL_GPIO        GPIOA
#define TLV5618_SCL_PIN         GPIO_Pin_5
#define TLV5618_SDA_GPIO        GPIOA
#define TLV5618_SDA_PIN         GPIO_Pin_7
#define TLV5618_CS_GPIO         GPIOB
#define TLV5618_CS_PIN          GPIO_Pin_3

#define TLV5618_CS_H            GPIO_SetBits(TLV5618_CS_GPIO, TLV5618_CS_PIN)
#define TLV5618_CS_L            GPIO_ResetBits(TLV5618_CS_GPIO, TLV5618_CS_PIN)

#define TLV5618_MAX_VALUE       (1U<<12)  //DAC最大位数
#define TLV5618_REF_VALUE       3000      //unit:mV

typedef enum
{
  SLOW_MODE=0,
  FAST_MODE
}TLV5618_Mode_Enum;

typedef struct 
{
  //------用户设置区---------
  TLV5618_Mode_Enum eMode;  //模式
  uint16_t WriteOutA;       //写入A
  uint16_t WriteOutB;       //写入B
  //------过程量-------------
  uint16_t WriteOutALast;   //上次写入A
  uint16_t WriteOutBLast;   //上次写入B
}TLV5618_Data_Struct;
extern TLV5618_Data_Struct sTLV5618_Data;

void F_TVL5618_Init(void);      //初始化ADCS
void F_Set_DAC_Data(void);      //设置DAC的值

#endif

