/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 15:30:56
 * @LastEditTime: 2021-12-21 16:40:43
 * @Brief:  
 */
#include  "Loop.h"
#include "usart.h"
#include "BaseDrive.h"
#include "Modbus.h"
#include "ParamCali.h"
#include "LoadControl.h"
#include "CurrVoltControl.h"

Main_Time_Typedef sMainTime;

/**
 * @bried:  副时钟 500us一次
 * @para:   None
 * @retval: None
 */
void F_500us_Time(void)
{
  
}

/**
 * @bried:  副时钟 1ms一次
 * @para:   None
 * @retval: None
 */
void F_1ms_Time(void)
{
  
  F_ReveiveEndInteruptCallback();
}

/**
 * @bried:  副时钟 20ms一次
 * @para:   None
 * @retval: None
 */
void F_20ms_Time(void)
{
  
}

/**
 * @bried:  副时钟 100ms一次
 * @para:   None
 * @retval: None
 */
void F_100ms_Time(void)
{ 
  F_ReveiveDataAnalysis();
  F_ADS112C04_AverageInterruptCallback();
  if(sParamCali.StartCali==false)
  {
    if(sADS112C04_Data.AverageOkFlag)
    {
      //uint32_t magnify_volt = sADS112C04_Data.AverageReadVinData << 8;
      //uint16_t reality = magnify_volt / sParamCali.MinVoltValue;
      
      //uint32_t test = (uint32_t )(sParamCali.SetCaliVoltValue * sADS112C04_Data.AverageReadVinData);
      //uint16_t reality = test/sParamCali.MinVoltValue;
      //printf("电压=%d,AD=%d\n",reality,sADS112C04_Data.AverageReadVinData);
      if(sADS112C04_Data.AverageOkFlag)
      {
        F_ReadCailCurrVolt();
        sADS112C04_Data.AverageOkFlag = false;
      }
      
      printf("I=%d,V=%d, i=%d\n",sCalculate.ReadCurr ,sCalculate.ReadVolt,sADS112C04_Data.AverageReadCsData);
      sADS112C04_Data.AverageOkFlag = false;
    }
  }
}


void F_LoopTimeLoopCall(void)
{
  if(sMainTime.Flag_500us)  {sMainTime.Flag_500us = false; F_500us_Time();}
  if(sMainTime.Flag_1ms)    {sMainTime.Flag_1ms   = false; F_1ms_Time();}
  if(sMainTime.Flag_20ms)   {sMainTime.Flag_20ms  = false; F_20ms_Time();}
  if(sMainTime.Flag_100ms)  {sMainTime.Flag_100ms = false; F_100ms_Time();}

  F_ParamCaliLoopCallback();
}

/**
 * @bried:  loop时钟源(中断回调1ms)
 * @para:  None
 * @retval:  None
 */
void F_LoopTimeInterruptCall(void)
{
  sMainTime.Time_500us ++;
  
  sMainTime.Flag_500us = true;
  if(!(sMainTime.Time_500us%2))         sMainTime.Flag_1ms = true;
  //if(!(sMainTime.Time_500us%4))         sMainTime.Flag_500us = true;
  if(!(sMainTime.Time_500us%(10*2)))    sMainTime.Flag_20ms = true;
  if(!(sMainTime.Time_500us%(100*2)))   sMainTime.Flag_100ms = true;
  if(!(sMainTime.Time_500us%(1000*2)))  sMainTime.Time_500us = 0;
}



