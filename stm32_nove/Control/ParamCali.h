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

//====================Ӳ������ADC����ֵ==================
#define THEORY_

typedef enum
{
  CALI_ERROR_NONE=0,                      //�޴���
  CALI_ERROR_SOURCE_TIME_OUT,             //����У׼Դ��ʱ
  CALI_ERROR_VOLT_STABILIZE_AVERAGE,      //��ѹԴ���ȶ�(�Ʋ���)
  CALI_ERROR_VOLT_STABILIZE_MAX,          //��ѹԴ���ȶ�(�д�ë��)
  CALI_ERROR_CURR_STABILIZE_AVERAGE,      //����Դ���ȶ�(�Ʋ���)
  CALI_ERROR_CURR_STABILIZE_MAX,          //����Դ���ȶ�(�д�ë��)
  CALI_ERROR_SET_CURR_VALUE,              //����:û�����õ�������
  CALI_ERROR_SET_VOLT_VALUE,              //����:û�����õ�ѹ����
}CALI_ERROR;


typedef enum
{
  CALI_STATE_DRY=0,                   //У׼׼������
  CALI_STATE_INPUT_CHK,               //����Դ����У��
  CALI_STATE_ADC_ZERO_CURR_OFFSET,    //�����ADC�������ݲ���
  CALI_STATE_ADC_VOLT_CHK,            //У׼ADC�ĵ�ѹ
  CALI_STATE_ADC_CURR_CHK,            //У׼ADC�ĵ���
  CALI_STATE_DAC_ZERO_CURR_OFFSET,    //�����DAC�������ݲ���
  CALI_STATE_DAC_VOLT_CHK,            //У׼DAC�ĵ�ѹ
  CALI_STATE_DAC_CURR_CHK,            //У׼DAC�ĵ���
  CALI_STATE_COMPLETE,                //У�����
}CALI_STATE;


typedef struct 
{
  //�趨��ǰ�����ĵ���ֵ(��λС��)
  uint32_t SetCaliCurrValue;          //unit:mA
  //�趨��ǰ�����ĵ�ѹֵ(��λС��)
  uint32_t SetCaliVoltValue;          //unit:10mV    
  
  
  bool StartCali;     //����״̬У׼��־
  CALI_STATE    eCaliState;
  CALI_ERROR    eCaliError;
  uint8_t       delay;      //�ȴ��ȶ�(����Ϊһ����ȡADCʱ��)
  //===========��ѹ/�����ȶ��Բ���=========
  #define   CNT_NUMBER      50
  bool      StabilizeOkFlag;  //�ȶ���OK
  uint8_t   GatherCnt;        //��ǰ��ѹ�ɼ�����
  uint16_t  MaxError;         //������
  int16_t  AverageError;      //�ۼ����
  int16_t  GatherData[CNT_NUMBER];        //�ɼ�ֵ
  int16_t  AverageData;       //ƽ��ֵ
  
  //============����洢ֵ===============
  bool     MemoryCaliFlag;    //�洢������У׼����
  int16_t  MemoryAdcCurrOffset;     //��㲹��ֵ
  int16_t  MemorySetVolt;     //�洢���õ�ѹֵ(unit:10mV)
  int16_t  MemorySetCurr;     //�洢���õ���ֵ(unit:1mA)
  int16_t  MemoryAdcVolt;     //�洢���õ�ѹ��Ӧ�ĵ�ѹADCֵ
  int16_t  MemoryAdcCurr;     //�洢���õ�����Ӧ�ĵ���ADCֵ
  int16_t  MemoryDacCurrOffset; //DAC����������
}ParamCali_Typedef;
extern ParamCali_Typedef sParamCali;

void F_ParamCali_Init(void);
void F_ParamCaliLoopCallback(void);

#endif
