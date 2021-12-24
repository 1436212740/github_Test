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

//================���ļ��ڲ������ļ�=================
static void F_InputSourceChk(void);       //У׼Դ����
static void F_ZeroCurrOffset(void);       //�����ADC�����ɼ�
static void F_ADCVoltCalculate(void);     //ADC��ѹУ׼
static void F_ADCCurrCalculate(void);     //У׼ADC�ĵ���
static void F_DACCurrZeroCheck(void);		
static void F_DAC_Volt_Check(void);       //У׼DAC�ĵ�ѹ

static void F_StabilityCurrVolt(uint16_t data, uint16_t cnt);   //�����ȶ��Բ���
static void F_StabilityValueClear(void);                        //�ȶ�����ʱ�������


void F_ParamCali_Init(void)
{
  sParamCali.SetCaliCurrValue = 1000;   //1A
  sParamCali.SetCaliVoltValue = 3000;   //30V
  sParamCali.StartCali = true;
  sParamCali.eCaliState = CALI_STATE_DRY;//CALI_STATE_COMPLETE;


  //-----�洢ֵ------
  //sParamCali.MemoryZeroCurrOffse = 404;
  //sParamCali.MemoryAdcVolt = 7515;
  //sParamCali.MemoryAdcCurr = 958;
  //sParamCali.MemorySetVolt = 3000;
  //sParamCali.MemorySetCurr = 1000;
}//

/**
 * @bried: ����У׼��ִ������
 * 1.��ѯУ׼ǰ�ĵ�����ѹ�趨ֵ��Ϊ��
 * 2.ʶ��У׼Դ����
 * 3.0������������������㲹��ֵ
 * 4.����С������0������������ѹ(У׼ADC)
 * 5.��ѹС��ѹ����������(У׼ADC)
 * 6.ָ����������,������ѹ,����У׼DAC
 * 7.ָ����ѹ��ѹ,������ѹ,����У׼DAC
 * 8.������ѹ������֤
 * @para:  None
 * @retval:None
 */
void F_ParamCaliLoopCallback(void)
{
  if(sParamCali.StartCali == true)
  {
    //=============�׶���ת��ȴ��ȶ���ȡ����============
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
    //=============У׼�׶���ת====================
    switch (sParamCali.eCaliState)
    {
    case CALI_STATE_DRY:                //У��ǰ��鱸������
      if(!sParamCali.SetCaliCurrValue) {sParamCali.eCaliError = CALI_ERROR_SET_CURR_VALUE; sParamCali.eCaliState = CALI_STATE_COMPLETE;}
      else if(!sParamCali.SetCaliVoltValue) {sParamCali.eCaliError = CALI_ERROR_SET_VOLT_VALUE; sParamCali.eCaliState = CALI_STATE_COMPLETE;}
      else  
      {
        sParamCali.eCaliState = CALI_STATE_INPUT_CHK;
      }
      break;
    case CALI_STATE_INPUT_CHK:              //��ѹԴ����У��
      F_InputSourceChk();
      break;
    case CALI_STATE_ADC_ZERO_CURR_OFFSET:   //�����ADC��������
      F_ZeroCurrOffset();
      break;
    case CALI_STATE_ADC_VOLT_CHK:           //У׼ADC�ĵ�ѹ
      F_ADCVoltCalculate();
      break;
    case CALI_STATE_ADC_CURR_CHK:           //У׼ADC�ĵ���
      F_ADCCurrCalculate();
      break;
    case CALI_STATE_DAC_ZERO_CURR_OFFSET:    //�����DAC��������
      F_DACCurrZeroCheck();
      break;  
    case CALI_STATE_DAC_VOLT_CHK:           //У׼DAC�ĵ�ѹ
      F_DAC_Volt_Check();
      break;
    case CALI_STATE_DAC_CURR_CHK:           //У׼DAC�ĵ���
      break;
    case CALI_STATE_COMPLETE:               //У׼���
      sParamCali.StartCali = false;
      sParamCali.MemoryCaliFlag = true;
      //-----------�洢-------------
      break;
    default:
      break;
    }
  }
}



/**
 * @bried:  �ȴ�У׼Դ����
 * @para:   None
 * @retval: None
 */
static void F_InputSourceChk(void)
{
  uint16_t set_curr = 0;
  F_Load_ConstCurr(set_curr);   //����0����
  if(sADS112C04_Data.AverageOkFlag)
  {
    //=================???????????????????????????????
    if(sADS112C04_Data.AverageReadVinData>100)    //ǰ��д��̶�ֵ�����ڸ���ʵ��Ӳ����ֵ���и��ļ���
    {
      F_StabilityValueClear();

      uint16_t set_curr = 0;
      F_Load_ConstCurr(set_curr);   //����0����
      sParamCali.eCaliState = CALI_STATE_ADC_ZERO_CURR_OFFSET;//����У׼ADC����
      sParamCali.delay = 5;
      debug("ADC���������ֵ�ɼ�\n");
    }
    else
    {
      //==========???????????????????????????
      //����д�볬ʱ��Դ����
      debug(" �ȴ�У׼Դ����\n");
    }
    sADS112C04_Data.AverageOkFlag = false;
  }
}

/**
 * @bried:  �����ADC�����ɼ�(�ý׶�һ��Ҫ�ڹ�ѹ�׶β��ԣ������ٺ�������0��������)
 * @para:   None
 * @retval: None
 */
static void F_ZeroCurrOffset(void)
{
  //uint16_t set_volt = 4095;
  //F_Load_ConstVolt(set_volt);   //���ú�ѹ����ѹ,��ʱ����һ����0
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadCsData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
  if(sParamCali.StabilizeOkFlag)
  {
    //===========����ѹ�ȶ��Բ���(����)============
    debug("�ɼ�����Ϊ\n");
    for(uint8_t i=0;i<CNT_NUMBER;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n����");
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
    F_Load_ConstCurr(set_curr);   //����0����
    sParamCali.eCaliState = CALI_STATE_DAC_ZERO_CURR_OFFSET;  //CALI_STATE_ADC_VOLT_CHK;//����У׼ADC����
    sParamCali.delay = 5;
    debug("\nADC��ѹУ׼��ʼ�ɼ�\n");
  }
}

/**
 * @bried:  У׼ADC��ѹ����
 * @para:   None
 * @retval: None
 */
static void F_ADCVoltCalculate(void)
{
  //===========����ѹ�ȶ��Բ���(�ɼ�)============
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadVinData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
  
  if(sParamCali.StabilizeOkFlag)
  {
    //===========����ѹ�ȶ��Բ���(����)============
    debug("�ɼ�����Ϊ\n");
    for(uint8_t i=0;i<CNT_NUMBER;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n����");
    debug("A_err:%d, M_err:%d, A_Volt:%d, ",sParamCali.AverageError,\
                                            sParamCali.MaxError, sParamCali.AverageData);

    if(sParamCali.MaxError>10)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_VOLT_STABILIZE_MAX;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      debug("err:У׼ADC��ѹ������\n");
      return ;
    }
    if(abs(sParamCali.AverageError)>2*CNT_NUMBER)
    {
      F_StabilityValueClear();
      sParamCali.eCaliError = CALI_ERROR_VOLT_STABILIZE_AVERAGE;
      sParamCali.eCaliState = CALI_STATE_COMPLETE;
      debug("err:У׼ADC��ѹ������\n");
      return ;
    }
    //===========����У׼��ľ���ֵ(��ѹ10mV���ڵ�ADֵ(adֵ�����Ŵ󣬷�ֹ̫С���ȶ�ʧ))==================
    //---------����10mV����ADֵ---------
    sParamCali.MemoryAdcVolt = sParamCali.AverageData;
    sParamCali.MemorySetVolt = sParamCali.SetCaliVoltValue;
    F_StabilityValueClear();

    uint16_t set_volt = 15;    //??????��Ҫ����Ϊ�����ѹ����DAC��һ��
    F_Load_ConstVolt(set_volt); //���ú�ѹ
    sParamCali.eCaliState = CALI_STATE_ADC_CURR_CHK;    //��ת��ADC����У׼
    sParamCali.delay = 3;

    debug("ADC����У׼��ʼ�ɼ�\n");
  }
}

/**
 * @bried:  У׼ADC��������
 * @para:   None
 * @retval: None
 */
static void F_ADCCurrCalculate(void)
{
  //===========�������ȶ��Բ���(�ɼ�)==============
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadCsData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
  //===========�������ȶ��Բ���(����)============
  if(sParamCali.StabilizeOkFlag)
  {
    debug("�ɼ�����Ϊ\n");
    for(uint8_t i=0;i<CNT_NUMBER;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n����");
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
    //===========����У׼��ľ���ֵ(����1mA���ڵ�ADֵ(adֵ�����Ŵ󣬷�ֹ̫С���ȶ�ʧ))==================
    //---------����1mA����ADֵ---------
    //int32_t magnify_curr = sParamCali.AverageData<<8;   //�Ŵ�256������㣬���ᶪʧ��ѹ����
    //sParamCali.MinCurrValue = magnify_curr/sParamCali.SetCaliCurrValue;

    sParamCali.MemoryAdcCurr = sParamCali.AverageData - sParamCali.MemoryAdcCurrOffset;
    sParamCali.MemorySetCurr = sParamCali.SetCaliCurrValue;
    F_StabilityValueClear();
    //sParamCali.eCaliState = CALI_STATE_DAC_VOLT_CHK;    //У׼DAC�ĵ�ѹ
    sParamCali.eCaliState = CALI_STATE_COMPLETE;
    sParamCali.delay = 3;
    debug("DAC��ѹУ׼��ʼ\n");


    debug("MRV:%d, MRC:%d, MZ:%d\n",sParamCali.MemoryAdcVolt, sParamCali.MemoryAdcCurr, sParamCali.MemoryAdcCurrOffset);
  }
}

/**
 * @bried:  У׼DAC������ֵ
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
    debug("���ɼ�����Ϊ\n");
    for(uint8_t i=0;i<5;i++)
      debug("%d,",sParamCali.GatherData[i]);
    debug("\n����");
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
      debug("�ҵ����:%d\n",dac_zero);
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
 * @bried:  У׼DAC��ѹ����
 * @para:   None
 * @retval: None
 */
static void F_DAC_Volt_Check(void)
{
  //--------CV��·��ѹ---------
  //��������:105.1K   ��������:2.7K
  //1V---->0.025V
  //--------DAC����-----------
  //12bit---->2.75V
  //x--->0.001V
  //==========DAC��ߵ�ѹ��һ��ʱУ׼=============
  //----------��ֹ���ȶ�ʧ���Ŵ�256��-------------
  //#define LOAD_CV_UP_RESIS      105.1   //��������
  //#define LOAD_CV_DOWN_RESIS    2.7     //��������
  //ʵ��1Vת����DAC�����xmV
  //#define LOAD_CV_1V_DAC_MV     (uint16_t)(1000/(LOAD_CV_UP_RESIS+LOAD_CV_DOWN_RESIS)*LOAD_CV_DOWN_RESIS)

  //#define TLV5618_MAX_VALUE       (uint16_t)(1U<<12)  //DAC���λ��
  //#define TLV5618_REF_VALUE       (uint16_t)(3000)      //unit:mV  DAC��׼ֵ

  //#define DAC_MAX_VALUE   (((uint32_t)TLV5618_MAX_VALUE)<<12)
  //#define DAC_UNIT_MV     (((uint32_t)DAC_MAX_VALUE)/((uint32_t)TLV5618_REF_VALUE))   //DAC���1mV��Ҫ��DACֵ(�Ŵ�256����ֵ)

  //uint16_t set_dac_volt = LOAD_CV_1V_DAC_MV * (sParamCali.SetCaliVoltValue/100);    //DAC�����ѹ(mV)
  //uint16_t dac_data = (set_dac_volt * DAC_UNIT_MV) >> 12;    //�ָ�����
  //dac_data >>= 1;   //��ѹΪ��ߵ�ѹ��һ��
  //F_Load_ConstVolt(dac_data); //���ú�ѹ
  //debug(" 1mv=%dDAC, %dmV, output=%d\n",DAC_UNIT_MV, LOAD_CV_1V_DAC_MV, dac_data);
  
  //==============��ȡ��ǰ��ѹ========
  if(sADS112C04_Data.AverageOkFlag)
  {
    F_StabilityCurrVolt(sADS112C04_Data.AverageReadVinData, CNT_NUMBER);
    sADS112C04_Data.AverageOkFlag = false;
  }
}


/**
 * @bried:  ����/��ѹ�ȶ��Բ���
 *          ʹ�øú������Զ�����cnt�����ݺ󣬷���OK�ı�ʶ
 * @para:   data---�����ȶ��Ե�����
 *          cnt----�ȶ��Բ�������
 * @retval:  None
 */
static void F_StabilityCurrVolt(uint16_t data, uint16_t cnt)    //������һ������????????
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
      sParamCali.MaxError = err;    //ȡ������ֵ

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
    sParamCali.AverageData = (uint16_t)(sum/(cnt-1));  //У׼�׶Σ��������ޣ�����ֱ�ӳ�
    sParamCali.StabilizeOkFlag = true;
  }
}

/**
 * @bried:  ����ȶ������ݵ���ʱ����
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

