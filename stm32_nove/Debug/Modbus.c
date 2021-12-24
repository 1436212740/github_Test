/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-10 15:44:54
 * @LastEditTime: 2021-12-11 10:27:13
 * @Brief:  
 */
#include "Modbus.h"
#include "usart.h"
#include "LoadControl.h"

/**
 * @bried:  ������������
 * @para:  None
 * @retval:  None
 */
void F_ReveiveDataAnalysis(void)
{
  if(sUsartData.eReveiveState == REVEIVE_COMPLETE)
  {
    sUsartData.eReveiveState = REVEIVE_IDLE;
    uint8_t mode = sUsartData.ReveiveBuff[0];
    uint16_t data = (sUsartData.ReveiveBuff[2]<<8)|(sUsartData.ReveiveBuff[1]);
    uint16_t data2 = (sUsartData.ReveiveBuff[4]<<8)|(sUsartData.ReveiveBuff[3]);
    switch (mode)
    {
    case 0x00:    //ֹͣ
      F_Load_Stop();
      printf("ֹͣģʽ\n");
      break;
    case 0x01:    //����
      F_Load_ConstCurr(data);
      printf("����ģʽ,ֵΪ%d\n",data);
      break;
    case 0x02:    //��ѹ
      F_Load_ConstVolt(data);
      printf("��ѹģʽ,ֵΪ%d\n",data);
      break;
    case 0x03:
      F_Load_ConstCurrVolt(data, data2);
      printf("������ѹģʽ,����=%d,��ѹ=%d\n", data, data2);
      break;
    default:
      break;
    }

  }
}

