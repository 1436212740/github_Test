/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-21 14:09:45
 * @LastEditTime: 2021-12-21 14:32:51
 * @Brief:  
 */
#ifndef __CURR_VOLT_CONTROL_H__
#define __CURR_VOLT_CONTROL_H__

#include "stm32f10x.h"

//===========计算后的电流电压数据===========
typedef struct 
{
  uint16_t ReadCurr;
  uint16_t ReadVolt;
}Calculate_Typedef;
extern Calculate_Typedef sCalculate;

void F_ReadCailCurrVolt(void);


#endif

