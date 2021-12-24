/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-14 15:08:52
 * @LastEditTime: 2021-12-21 17:24:24
 * @Brief:  
 */
#include "ParamCali.h"
#include "ADS112C04.h"
#include "TLV5618.h"
#include "usart.h"
#include "LoadControl.h"
#include <math.h>
#include "stdlib.h"

ParamCali_Typedef sParamCali;

//================本文件内部调用文件=================
static void F_InputSourceChk(void);       //校准源插入
static void F_ZeroCurrOffset(void);       //零电流ADC补偿采集
static void F_ADCVoltCalculate(void);     //ADC电压校准
static void F_ADCCurrCalculate(void);     //校准ADC的电流
static void F_DACCurrZeroCheck(void);		
static void F_DAC_Volt_Check(void);       //校准DAC的电压

static void F_StabilityCurrVolt(uint16_t data, uint16_t cnt);   //数据稳定性测试
static void F_StabilityValueClear(void);                        //稳定性临时变量清除


void F_ParamCali_Init(void)
{
  sParamCali.SetCaliCurrValue = 1000;   //1A
  sParamCali.SetCaliVoltValue = 3000;   //30V
  sParamCali.StartCali = true;
  sParamCali.eCaliState = CALI_STATE_DRY;//CALI_STATE_COMPLETE;


  //-----存储值------
  //sParamCali.MemoryZeroCurrOffse = 404;
  //sParamCali.MemoryAdcVolt = 7515;
  //sParamCali.MemoryAdcCurr = 958;
  //sParamCali.MemorySetVolt = 3000;
  //sParamCali.MemorySetCurr = 1000;
}//

/**
 * @bried: 精度校准的执行流程
 * 1.查询校准前的电流电压设定值不为零
 * 2.识别校准源插入
 * 3.0电流测量电流检测的零点补偿值
 * 4.恒流小电流或0电流测量最大电压(校准ADC)
 * 5.恒压小电压测试最大电流(校准ADC)
 * 6.指定恒流电流,测量电压,反向校准DAC
 * 7.指定恒压电压,测量电压,反向校准DAC
 * 8.恒流恒压滑动验证
 * @para:  None
 * @retval:None
 */
void F_ParamCaliLoopCallback(void)
{
  if(sParamCali.StartCali == true)
  {
    //=============阶段跳转后等待稳定读取数据============
    if(sParamCali.delay)
    {
      if(sADS112C04_Data.AverageOkFlag)
      {
        sADS112C04_Data.AverageOkFlag = false;
        if(--sParamCali.delay!=0)
          return;
      }
      else   return;
    }
    //=============校准阶段跳转====================
    switch (sParamCali.eCaliState)
    {
    case CALI_STATE_DRY:                //校验前检查备用条件
      if(!sParamCali.SetCaliCurrValue) {sParamCali.eCaliError = CALI_ERROR_SET_CURR_VALUE; sParamCali.eCaliState = CALI_STATE_COMPLETE;}
      else if(!sParamCali.SetCaliVoltValue) {sParamCali.eCaliError = CALI_ERROR_SET_VOLT_VALUE; sParamCali.eCaliState = CALI_STATE_COMPLETE;}
      else  
      {
        sParamCali.eCaliState = CALI_STATE_INPUT_CHK;
      }
      break;
    case CALI_STATE_INPUT_CHK:              //电压源插入校验
      F_InputSourceChk();
      break;
    case CALI_STATE_ADC_ZERO_CURR_OFFSET:   //零电流ADC电流补偿
      F_ZeroCurrOffset();
      break;
    case CALI_STATE_ADC_VOLT_CHK:           //校准ADC的电压
      F_ADCVoltCalculate();
      break;
    case CALI_STATE_ADC_CURR_CHK:           //校准ADC的电流
      F_ADCCurrCalculate();
      break;
    case CALI_STATE_DAC_ZERO_CURR_OFFSET:    //零电流DAC补偿数据
      F_DACCurrZeroCheck();
      break;  
    case CALI_STATE_DAC_VOLT_CHK:           //校准DAC的电压
      F_DAC_Volt_Check();
      break;
    case CALI_STATE_DAC_CURR_CHK:           //校准DAC的电流
      break;
    case CALI_STATE_COMPLETE:               //校准完成
      sParamCali.StartCali = false;
      sParamCali.MemoryCaliFlag = true;
      //-----------存储-------------
      break;
    default:
      break;
    }
  }
}



/**
 * @bried:  等待校准源接入
 * @para:   None
 * @retval: None
 */
static void F_InputSourceChk(void)
{
  uint16_t set_curr = 0;
  F_Load_ConstCurr(set_curr);   //设置0电流
  if(sADS112C04_Data.AverageOkFlag)
  {
    //=================???????????????????????????????
    if(sADS112C04_Data.AverageReadVinData>100)    //前期写入固定值，后期根据实际硬件阻值进行更改计算
    {
      F_StabilityValueClear();

      uint16_t set_curr = 0;
      F_Load_ConstCurr(set_curr);   //设置0电流
      sParamCali.eCaliState = CALI_STATE_ADC_ZERO_CURR_OFFSET;//计算校准ADC精度
      sParamCali.delay = 5;
      debug("ADC零电流补偿值采集\n");
    }
    else
    {
      //==========???????????????????????????
      //后期写入超时的源接入
      debug(" 等待校准源插入\n");
    }
    sADS112C04_Data.AverageOkFlag = false;
  }
}

/**
 * @bried:  零电流ADC补偿采集(该阶段一定要在过压阶段测试，不能再恒流设置0电流测试)
 * @para:   None
 * @retval: None
 */
static void F_ZeroCurrOffset(void)
{
  //uint16_t set_volt = 4095;
  //F_Load_ConstVolt(set_volt);   //设置恒压最大电压,此时电流一定是0
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadCsData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
  if(sParamCali.StabilizeOkFlag)
  {
    //===========最大电压稳定性测量(计算)============
    debug("采集数据为\n");
    for(uint8_t i=0;i<CNT_NUMBER;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n计算");
    debug("A_err:%d, M_err:%d, A_Volt:%d, ",sParamCali.AverageError,\
                                            sParamCali.MaxError, sParamCali.AverageData);

    if(sParamCali.MaxError>10)
    {
      sParamCali.eCaliError = CALI_ERROR_CURR_STABILIZE_MAX;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      return ;
    }
    if(abs(sParamCali.AverageError)>CNT_NUMBER*2)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_CURR_STABILIZE_AVERAGE;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      return ;
    }
    sParamCali.MemoryAdcCurrOffset = sParamCali.AverageData;
    F_StabilityValueClear();

    uint16_t set_curr = 0;
    F_Load_ConstCurr(set_curr);   //设置0电流
    sParamCali.eCaliState = CALI_STATE_DAC_ZERO_CURR_OFFSET;  //CALI_STATE_ADC_VOLT_CHK;//计算校准ADC精度
    sParamCali.delay = 5;
    debug("\nADC电压校准开始采集\n");
  }
}

/**
 * @bried:  校准ADC电压精度
 * @para:   None
 * @retval: None
 */
static void F_ADCVoltCalculate(void)
{
  //===========最大电压稳定性测量(采集)============
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadVinData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
  
  if(sParamCali.StabilizeOkFlag)
  {
    //===========最大电压稳定性测量(计算)============
    debug("采集数据为\n");
    for(uint8_t i=0;i<CNT_NUMBER;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n计算");
    debug("A_err:%d, M_err:%d, A_Volt:%d, ",sParamCali.AverageError,\
                                            sParamCali.MaxError, sParamCali.AverageData);

    if(sParamCali.MaxError>10)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_VOLT_STABILIZE_MAX;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      debug("err:校准ADC电压波动大\n");
      return ;
    }
    if(abs(sParamCali.AverageError)>2*CNT_NUMBER)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_VOLT_STABILIZE_AVERAGE;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      debug("err:校准ADC电压波动大\n");
      return ;
    }
    //===========计算校准后的精度值(电压10mV对于的AD值(ad值经过放大，防止太小精度丢失))==================
    //---------计算10mV对于AD值---------
    sParamCali.MemoryAdcVolt = sParamCali.AverageData;
    sParamCali.MemorySetVolt = sParamCali.SetCaliVoltValue;
    F_StabilityValueClear();

    uint16_t set_volt = 15;    //??????需要设置为输入电压理论DAC的一半
    F_Load_ConstVolt(set_volt); //设置恒压
    sParamCali.eCaliState = CALI_STATE_ADC_CURR_CHK;    //跳转到ADC电流校准
    sParamCali.delay = 3;

    debug("ADC电流校准开始采集\n");
  }
}

/**
 * @bried:  校准ADC电流精度
 * @para:   None
 * @retval: None
 */
static void F_ADCCurrCalculate(void)
{
  //===========最大电流稳定性测量(采集)==============
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadCsData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
  //===========最大电流稳定性测量(计算)============
  if(sParamCali.StabilizeOkFlag)
  {
    debug("采集数据为\n");
    for(uint8_t i=0;i<CNT_NUMBER;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n计算");
    debug("A_err:%d, M_err:%d, A_Volt:%d, ",sParamCali.AverageError,\
                                            sParamCali.MaxError, sParamCali.AverageData);
    if(sParamCali.MaxError>10)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_CURR_STABILIZE_MAX;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      return ;
    }
    if(abs(sParamCali.AverageError)>CNT_NUMBER*2)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_CURR_STABILIZE_AVERAGE;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      return ;
    }
    //===========计算校准后的精度值(电流1mA对于的AD值(ad值经过放大，防止太小精度丢失))==================
    //---------计算1mA对于AD值---------
    //int32_t magnify_curr = sParamCali.AverageData<<8;   //放大256倍后计算，不会丢失电压精度
    //sParamCali.MinCurrValue = magnify_curr/sParamCali.SetCaliCurrValue;

    sParamCali.MemoryAdcCurr = sParamCali.AverageData - sParamCali.MemoryAdcCurrOffset;
    sParamCali.MemorySetCurr = sParamCali.SetCaliCurrValue;
    F_StabilityValueClear();
    //sParamCali.eCaliState = CALI_STATE_DAC_VOLT_CHK;    //校准DAC的电压
    sParamCali.eCaliState = CALI_STATE_COMPLETE;
    sParamCali.delay = 3;
    debug("DAC电压校准开始\n");


    debug("MRV:%d, MRC:%d, MZ:%d\n",sParamCali.MemoryAdcVolt, sParamCali.MemoryAdcCurr, sParamCali.MemoryAdcCurrOffset);
  }
}

/**
 * @bried:  校准DAC零点电流值
 * @para:   None
 * @retval: None
 */
static void F_DACCurrZeroCheck(void)
{
  static uint16_t dac_zero=180;
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadCsData, 5);
    sADS112C04_Data.AverageOkFlag = true;
  }
  if(sParamCali.StabilizeOkFlag)
  {
    debug("零点采集数据为\n");
    for(uint8_t i=0;i<5;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n计算");
    debug("A_err:%d, M_err:%d, A_Volt:%d, ",sParamCali.AverageError,\
                                            sParamCali.MaxError, sParamCali.AverageData);

    int16_t diff = sParamCali.AverageData-sParamCali.MemoryAdcCurrOffset;
    if(diff>8)
    {
      F_StabilityValueClear();
      printf("dac = %d\n",dac_zero);
      F_Load_ConstCurr(dac_zero--);
      
      sParamCali.delay = 5;
    }
    if((diff>=4)&&(diff<8))
    {
      F_StabilityValueClear();
      sParamCali.eCaliState = CALI_STATE_DAC_VOLT_CHK;
      sParamCali.MemoryDacCurrOffset = dac_zero - 2;
      debug("找到零点:%d\n",dac_zero);
    }
    else
    {
      F_StabilityValueClear();
      //sParamCali.eCaliState = CALI_STATE_DAC_ZERO_CURR_OFFSET;
      printf("dac = %d\n",dac_zero);
      F_Load_ConstCurr(dac_zero++);
      
      sParamCali.delay = 5;
    }
  }
  
}


/**
 * @bried:  校准DAC电压精度
 * @para:   None
 * @retval: None
 */
static void F_DAC_Volt_Check(void)
{
  //--------CV环路分压---------
  //上拉电阻:105.1K   下拉电阻:2.7K
  //1V---->0.025V
  //--------DAC计算-----------
  //12bit---->2.75V
  //x--->0.001V
  //==========DAC最高电压的一半时校准=============
  //----------防止精度丢失，放大256倍-------------
  //#define LOAD_CV_UP_RESIS      105.1   //上拉电阻
  //#define LOAD_CV_DOWN_RESIS    2.7     //下拉电阻
  //实际1V转换成DAC的输出xmV
  //#define LOAD_CV_1V_DAC_MV     (uint16_t)(1000/(LOAD_CV_UP_RESIS+LOAD_CV_DOWN_RESIS)*LOAD_CV_DOWN_RESIS)

  //#define TLV5618_MAX_VALUE       (uint16_t)(1U<<12)  //DAC最大位数
  //#define TLV5618_REF_VALUE       (uint16_t)(3000)      //unit:mV  DAC基准值

  //#define DAC_MAX_VALUE   (((uint32_t)TLV5618_MAX_VALUE)<<12)
  //#define DAC_UNIT_MV     (((uint32_t)DAC_MAX_VALUE)/((uint32_t)TLV5618_REF_VALUE))   //DAC输出1mV需要的DAC值(放大256倍的值)

  //uint16_t set_dac_volt = LOAD_CV_1V_DAC_MV * (sParamCali.SetCaliVoltValue/100);    //DAC输出电压(mV)
  //uint16_t dac_data = (set_dac_volt * DAC_UNIT_MV) >> 12;    //恢复倍速
  //dac_data >>= 1;   //恒压为最高电压的一半
  //F_Load_ConstVolt(dac_data); //设置恒压
  //debug(" 1mv=%dDAC, %dmV, output=%d\n",DAC_UNIT_MV, LOAD_CV_1V_DAC_MV, dac_data);
  
  //==============读取当前电压========
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadVinData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
}


/**
 * @bried:  电流/电压稳定性测量
 *          使用该函数后，自动测量cnt次数据后，返回OK的标识
 * @para:   data---测量稳定性的数据
 *          cnt----稳定性测量次数
 * @retval:  None
 */
static void F_StabilityCurrVolt(uint16_t data, uint16_t cnt)    //放弃第一组数据????????
{
  if(sParamCali.GatherCnt==0)
  {
    ;
  }
  else if(sParamCali.GatherCnt==1)
  {
    sParamCali.GatherData[sParamCali.GatherCnt] = data;
  }
  else
  {
    int16_t err = sParamCali.GatherData[sParamCali.GatherCnt-1] - data;
    sParamCali.GatherData[sParamCali.GatherCnt] = data;

    err = abs(err);

    if(err>sParamCali.MaxError)
      sParamCali.MaxError = err;    //取最大误差值

    sParamCali.AverageError += err;

  }
  sParamCali.GatherCnt ++;
  if(sParamCali.GatherCnt==cnt)
  {
    uint32_t sum=0;
    for(uint8_t i=1; i<cnt; i++)
    {
      sum += sParamCali.GatherData[i];
    }
    sParamCali.AverageData = (uint16_t)(sum/(cnt-1));  //校准阶段，算力无限，可以直接除
    sParamCali.StabilizeOkFlag = true;
  }
}

/**
 * @bried:  清除稳定性数据的临时变量
 * @para:   None
 * @retval: NOne
 */
static void F_StabilityValueClear(void)
{
  sParamCali.StabilizeOkFlag = false;
  sParamCali.GatherCnt = 0;
  sParamCali.MaxError = 0;
  sParamCali.AverageError = 0;
  sParamCali.AverageData = 0;
  for(uint8_t i=0; i<CNT_NUMBER; i++)
  {
    sParamCali.GatherData[i] = 0;
  }
}

