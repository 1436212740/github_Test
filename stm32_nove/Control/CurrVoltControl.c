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
 * @bried:  读取校准后的电流计算数据
 * @para:   None
 * @retval: None
 */
void F_ReadCailCurrVolt(void)
{
  if(sParamCali.MemoryCaliFlag)   //--已校准
  {
    sCalculate.ReadCurr = sParamCali.MemorySetCurr * sADS112C04_Data.AverageCaliReadCsData / sParamCali.MemoryAdcCurr;
    sCalculate.ReadVolt = sParamCali.MemorySetVolt * sADS112C04_Data.AverageCaliReadVinData / sParamCali.MemoryAdcVolt;
  }
  else
  {
    //计算未校准数据，并且计算预估值(用于识别硬件电路的外部电阻是否接错)
  }


}




