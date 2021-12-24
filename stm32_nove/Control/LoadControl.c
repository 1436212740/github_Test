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
 * @bried:  ����ģʽ
 * @para:  �������ֵ
 * @retval:  None
 */
void F_Load_ConstCurr(uint16_t data)
{
  sTLV5618_Data.WriteOutA = data;   //����
  sTLV5618_Data.WriteOutB = 1536;   //�趨����ѹ
  F_Set_DAC_Data();
  GPIO_SetBits(GPIOC, GPIO_Pin_15); 
}


/**
 * @bried:  ��ѹģʽ
 * @para:   �����ѹֵ
 * @retval:  None
 */
void F_Load_ConstVolt(uint16_t data)    //��λ��1V
{
  uint16_t cv_volt = data * LOAD_CV_1V_DAC_MV;
  uint16_t dac_volt = (uint16_t)((uint32_t)(cv_volt * DAC_UNIT_MV) >> 12);
  sTLV5618_Data.WriteOutA = 2024;   //����
  sTLV5618_Data.WriteOutB = dac_volt;   //��ѹ
  F_Set_DAC_Data();
  GPIO_ResetBits(GPIOC, GPIO_Pin_15); 
}


/**
 * @bried:  ֹͣ����
 * @para:  None
 * @retval:  None
 */
void F_Load_Stop(void)
{
  sTLV5618_Data.WriteOutA = 0;   //����
  sTLV5618_Data.WriteOutB = 0;   //��ѹ
  F_Set_DAC_Data();
  GPIO_ResetBits(GPIOC, GPIO_Pin_15); 
}


/**
 * @bried:  ������ѹģʽ
 * @para:  None
 * @retval:  None
 */
void F_Load_ConstCurrVolt(uint16_t curr, uint16_t volt)
{
  sTLV5618_Data.WriteOutA = curr;   //����
  sTLV5618_Data.WriteOutB = volt;   //��ѹ
  F_Set_DAC_Data();
  GPIO_ResetBits(GPIOC, GPIO_Pin_15); 
}
