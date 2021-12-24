/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-14 15:08:58
 * @LastEditTime: 2021-12-21 17:21:23
 * @Brief:  
 */
#ifndef __PARAM_CALI_H__
#define __PARAM_CALI_H__

#include "stm32f10x.h"
#include <stdbool.h>

//====================硬件理论ADC采样值==================
#define THEORY_

typedef enum
{
  CALI_ERROR_NONE=0,                      //无错误
  CALI_ERROR_SOURCE_TIME_OUT,             //接入校准源超时
  CALI_ERROR_VOLT_STABILIZE_AVERAGE,      //电压源不稳定(纹波大)
  CALI_ERROR_VOLT_STABILIZE_MAX,          //电压源不稳定(有大毛刺)
  CALI_ERROR_CURR_STABILIZE_AVERAGE,      //电流源不稳定(纹波大)
  CALI_ERROR_CURR_STABILIZE_MAX,          //电流源不稳定(有大毛刺)
  CALI_ERROR_SET_CURR_VALUE,              //错误:没有设置电流参数
  CALI_ERROR_SET_VOLT_VALUE,              //错误:没有设置电压参数
}CALI_ERROR;


typedef enum
{
  CALI_STATE_DRY=0,                   //校准准备工作
  CALI_STATE_INPUT_CHK,               //电流源插入校验
  CALI_STATE_ADC_ZERO_CURR_OFFSET,    //零电流ADC补偿数据测量
  CALI_STATE_ADC_VOLT_CHK,            //校准ADC的电压
  CALI_STATE_ADC_CURR_CHK,            //校准ADC的电流
  CALI_STATE_DAC_ZERO_CURR_OFFSET,    //零电流DAC补偿数据测量
  CALI_STATE_DAC_VOLT_CHK,            //校准DAC的电压
  CALI_STATE_DAC_CURR_CHK,            //校准DAC的电流
  CALI_STATE_COMPLETE,                //校验完成
}CALI_STATE;


typedef struct 
{
  //设定当前给定的电流值(三位小数)
  uint32_t SetCaliCurrValue;          //unit:mA
  //设定当前给定的电压值(两位小数)
  uint32_t SetCaliVoltValue;          //unit:10mV    
  
  
  bool StartCali;     //进行状态校准标志
  CALI_STATE    eCaliState;
  CALI_ERROR    eCaliError;
  uint8_t       delay;      //等待稳定(周期为一个读取ADC时间)
  //===========电压/电流稳定性测量=========
  #define   CNT_NUMBER      50
  bool      StabilizeOkFlag;  //稳定性OK
  uint8_t   GatherCnt;        //当前电压采集次数
  uint16_t  MaxError;         //最大误差
  int16_t  AverageError;      //累计误差
  int16_t  GatherData[CNT_NUMBER];        //采集值
  int16_t  AverageData;       //平均值
  
  //============结果存储值===============
  bool     MemoryCaliFlag;    //存储区域有校准数据
  int16_t  MemoryAdcCurrOffset;     //零点补偿值
  int16_t  MemorySetVolt;     //存储设置电压值(unit:10mV)
  int16_t  MemorySetCurr;     //存储设置电流值(unit:1mA)
  int16_t  MemoryAdcVolt;     //存储设置电压对应的电压ADC值
  int16_t  MemoryAdcCurr;     //存储设置电流对应的电流ADC值
  int16_t  MemoryDacCurrOffset; //DAC零点电流补偿
}ParamCali_Typedef;
extern ParamCali_Typedef sParamCali;

void F_ParamCali_Init(void);
void F_ParamCaliLoopCallback(void);

#endif
