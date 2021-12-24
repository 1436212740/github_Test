/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-21 14:09:38
 * @LastEditTime: 2021-12-21 14:36:41
 * @Brief:  
 */
#include "CurrVoltControl.h"
#include "ParamCali.h"
#include "ADS112C04.h"
#include "TLV5618.h"


Calculate_Typedef sCalculate;

/**
 * @bried:  ��ȡУ׼��ĵ�����������
 * @para:   None
 * @retval: None
 */
void F_ReadCailCurrVolt(void)
{
  if(sParamCali.MemoryCaliFlag)   //--��У׼
  {
    sCalculate.ReadCurr = sParamCali.MemorySetCurr * sADS112C04_Data.AverageCaliReadCsData / sParamCali.MemoryAdcCurr;
    sCalculate.ReadVolt = sParamCali.MemorySetVolt * sADS112C04_Data.AverageCaliReadVinData / sParamCali.MemoryAdcVolt;
  }
  else
  {
    //����δУ׼���ݣ����Ҽ���Ԥ��ֵ(����ʶ��Ӳ����·���ⲿ�����Ƿ�Ӵ�)
  }


}




