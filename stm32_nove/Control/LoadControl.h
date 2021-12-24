/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-10 15:54:01
 * @LastEditTime: 2021-12-21 11:31:42
 * @Brief:  
 */
#ifndef __LOAD_CONTROL_H__
#define __LOAD_CONTROL_H__

#include "stm32f10x.h"
#include "TLV5618.h"
#include "Hardware_Config.h"


#define LOAD_ADC_CC_COMPENSATE    377     //�������Ӧ��ADC����ֵ

//=====================ADC����=====================
#define     ADC_MAX_VALUE       ADS112C04_MAX_VALUE
#define     ADC_REF_VLOT        ADS112C04_REF_VALUE

//---------------------ADC��������------------------
#define     CS_SAMP_RESIS       10    //unit:mR



//-----���1V��DAC�����mV����
#define  LOAD_CV_1V_DAC_MV        (uint16_t)(1000/(LOAD_CV_UP_RESIS+LOAD_CV_DOWN_RESIS)*LOAD_CV_DOWN_RESIS)

//-----DACʵ�����1mV��Ҫ��DAC����(�Ŵ�4096��)
#define DAC_MAX_VALUE   (((uint32_t)TLV5618_MAX_VALUE)<<12)
#define DAC_UNIT_MV     (((uint32_t)DAC_MAX_VALUE)/((uint32_t)TLV5618_REF_VALUE))   //DAC���1mV��Ҫ��DACֵ(�Ŵ�4096����ֵ)

void F_Load_ConstCurr(uint16_t data);
void F_Load_ConstVolt(uint16_t data);
void F_Load_ConstCurrVolt(uint16_t curr, uint16_t volt);
void F_Load_Stop(void);

#endif
