/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-10 15:53:55
 * @LastEditTime: 2021-12-17 16:33:48
 * @Brief:  
 */
#include "LoadControl.h"
#include "TLV5618.h"


/**
 * @bried:  恒流模式
 * @para:  输入电流值
 * @retval:  None
 */
void F_Load_ConstCurr(uint16_t data)
{
  sTLV5618_Data.WriteOutA = data;   //电流
  sTLV5618_Data.WriteOutB = 1536;   //设定最大电压
  F_Set_DAC_Data();
  GPIO_SetBits(GPIOC, GPIO_Pin_15); 
}


/**
 * @bried:  恒压模式
 * @para:   输入电压值
 * @retval:  None
 */
void F_Load_ConstVolt(uint16_t data)    //单位是1V
{
  uint16_t cv_volt = data * LOAD_CV_1V_DAC_MV;
  uint16_t dac_volt = (uint16_t)((uint32_t)(cv_volt * DAC_UNIT_MV) >> 12);
  sTLV5618_Data.WriteOutA = 2024;   //电流
  sTLV5618_Data.WriteOutB = dac_volt;   //电压
  F_Set_DAC_Data();
  GPIO_ResetBits(GPIOC, GPIO_Pin_15); 
}


/**
 * @bried:  停止工作
 * @para:  None
 * @retval:  None
 */
void F_Load_Stop(void)
{
  sTLV5618_Data.WriteOutA = 0;   //电流
  sTLV5618_Data.WriteOutB = 0;   //电压
  F_Set_DAC_Data();
  GPIO_ResetBits(GPIOC, GPIO_Pin_15); 
}


/**
 * @bried:  恒流恒压模式
 * @para:  None
 * @retval:  None
 */
void F_Load_ConstCurrVolt(uint16_t curr, uint16_t volt)
{
  sTLV5618_Data.WriteOutA = curr;   //电流
  sTLV5618_Data.WriteOutB = volt;   //电压
  F_Set_DAC_Data();
  GPIO_ResetBits(GPIOC, GPIO_Pin_15); 
}
