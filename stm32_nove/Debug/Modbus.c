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
 * @bried:  解析发送数据
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
    case 0x00:    //停止
      F_Load_Stop();
      printf("停止模式\n");
      break;
    case 0x01:    //恒流
      F_Load_ConstCurr(data);
      printf("恒流模式,值为%d\n",data);
      break;
    case 0x02:    //恒压
      F_Load_ConstVolt(data);
      printf("恒压模式,值为%d\n",data);
      break;
    case 0x03:
      F_Load_ConstCurrVolt(data, data2);
      printf("恒流恒压模式,电流=%d,电压=%d\n", data, data2);
      break;
    default:
      break;
    }

  }
}

