/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-08 09:24:27
 * @LastEditTime: 2021-12-21 11:30:39
 * @Brief:  
 */
#ifndef __ADS112C04_H__
#define __ADS112C04_H__

#include "stm32f10x.h"
#include <stdbool.h>
#include "delay.h"

#define HAEDWARE_IIC     1    //��/Ӳ��IIC�л�

//=======================================================
//======================�ӿ�����=========================
//=======================================================
#define ADS112C04_RDY_GPIO        GPIOB
#define ADS112C04_RDY_PIN         GPIO_Pin_5
#define ADS112C04_RDY_READ        GPIO_ReadInputDataBit(ADS112C04_RDY_GPIO, ADS112C04_RDY_PIN)
#if(!HAEDWARE_IIC)
//-----------ģ��IIC---------------------
#define ADS112C04_SCL_GPIO        GPIOB
#define ADS112C04_SCL_PIN         GPIO_Pin_6
#define ADS112C04_SDA_GPIO        GPIOB
#define ADS112C04_SDA_PIN         GPIO_Pin_7


#define ADS112C04_SCL_H           GPIO_SetBits(ADS112C04_SCL_GPIO, ADS112C04_SCL_PIN)
#define ADS112C04_SCL_L           GPIO_ResetBits(ADS112C04_SCL_GPIO, ADS112C04_SCL_PIN)
#define ADS112C04_SDA_H           GPIO_SetBits(ADS112C04_SDA_GPIO, ADS112C04_SDA_PIN)
#define ADS112C04_SDA_L           GPIO_ResetBits(ADS112C04_SDA_GPIO, ADS112C04_SDA_PIN)
#define ADS112C04_SDA_READ        GPIO_ReadInputDataBit(ADS112C04_SDA_GPIO, ADS112C04_SDA_PIN)
#define ADS112C04_IIC_DELAY       Delay_us(6)

#else

#define ADS112C04_MAX_VALUE       (1U<<15)  //ԭ��16λ����ַ���ռ��һλ
#define ADS112C04_REF_VALUE       2750      //unit:mV
#endif
//=======================================================
//======================ADS112C04�Ĵ�������===============
//========================================================
#define ADS112C04_WRITE_ADD            0x80   //�ӻ���ַ(��Ӳ������)  A0 A1����GND
#define ADS112C04_READ_ADD             0x81
    //------------------REG0-----------------
#define ADS112C04_MUX             0       //��·ѡ����(4bit)      
#define ADS112C04_GAIN            0       //����ѡ��(3bit)        2^n
#define ADS112C04_PGA_BYPASS      0       //PGA����(1bit)        ����PGA���غ������������
    //------------------REG1-----------------
#define ADS112C04_DR              7       //�ɼ�����(3bit)        20SPS
#define ADS112C04_MODE            1       //ģʽѡ��(1bit)        0--����ģʽ   1--��ǿģʽ(��ǿģʽ�������ʷ���)
#define ADS112C04_CM              1       //ת��ģʽ(1bit)        0--����ת��   1--����ת��
#define ADS112C04_VREF            1       //��׼ѡ��(2bit)        0--2.048v    1--�ⲿ��׼    2,3--VCC��׼ 
#define ADS112C04_TS              0       //�¶ȴ�����(1bit)      0--�ر�       1--����
    //------------------REG2-----------------
#define ADS112C04_DRDY            0       //����׼����־(ֻ��)(1bit)   1--׼����
#define ADS112C04_DCNT            0       //���ݼ�����(1bit)      0--�ر�       1--����
#define ADS112C04_CRC             0       //����У��(2bit)
#define ADS112C04_BCS             0       //����Դ����(1bit)      0--�ر�       1--����(����ʶ�𴫸�������)
#define ADS112C04_IDAC            0       //���õ���(3bit)        off
    //------------------REG3-----------------
#define ADS112C04_I1MUX           0       //����Դ1ͨ��ѡ��(3bit)
#define ADS112C04_I2MUX           0       //����Դ2ͨ��ѡ��(3bit)
#define ADS112C04_RESERVED        0       //����(2bit)


//=======================================================
//======================�ڲ��Ĵ������ڵ�ַ================
//=======================================================
#define ADS112C04_RESET           0x06    //�����λ
#define ADS112C04_START           0x08    //��ʼת���ź�
#define ADS112C04_POWERDOWN       0x02    //����ģʽ,�ر��ڲ�IDAC,����Ĵ���
#define ADS112C04_RDATA           0x10    //������
#define ADS112C04_RREG0           0x20    //��REG0
#define ADS112C04_RREG1           0x24    //��REG1
#define ADS112C04_RREG2           0x28    //��REG2
#define ADS112C04_RREG3           0x2C    //��REG3
#define ADS112C04_WREG0           0x40    //дREG0
#define ADS112C04_WREG1           0x44    //дREG1
#define ADS112C04_WREG2           0x48    //дREG2
#define ADS112C04_WREG3           0x4C    //дREG3

#define ADS112C04_REG0_DATA       ((ADS112C04_MUX<<4)|(ADS112C04_GAIN<<1)|(ADS112C04_PGA_BYPASS<<0))         
#define ADS112C04_REG1_DATA       ((ADS112C04_DR<<5)|(ADS112C04_MODE<<4)|(ADS112C04_CM<<3)|(ADS112C04_VREF<<1)|(ADS112C04_TS<<0))
#define ADS112C04_REG2_DATA       ((ADS112C04_DRDY<<7)|(ADS112C04_DCNT<<6)|(ADS112C04_CRC<<4)|(ADS112C04_BCS<<3)|(ADS112C04_IDAC<<0))
#define ADS112C04_REG3_DATA       ((ADS112C04_I1MUX<<5)|(ADS112C04_I2MUX<<2)|(ADS112C04_RESERVED<<0))

//===================================================
//======================���ݽṹ======================
//===================================================
//=========�ɼ�ͨ��ѡ��=========
typedef enum
{
      //------�ɼ�ͨ��ѡ��--------
  CS_CHANNEL=0,     
  VIN_CHANNEL=6
}ADS112C04_Channel_Typedef;

//========����ѡ��=========
typedef enum 
{
  GAIN_1=0,
  GAIN_2=1,
  GAIN_4=2,
  GAIN_8=3,
  GAIN_16=4,
  GAIN_32=5,
  GAIN_64=6,
  GAIN_128=7
}ADS112C04_Gain_Typedef;

typedef struct
{
  bool      StartGather;          //��ʼ�ɼ��ź�
  //---------ͨ������----------
  ADS112C04_Channel_Typedef eADS112C04_Channel;     //ͨ��ѡ��
  ADS112C04_Gain_Typedef eADS112C04_Gain;           //����ѡ��
  
  //---------��ʱ�ɼ�----------
  bool      GatherOkFlag;         //�ɼ��ɹ���־
  #define   GATHER_VALUE      8      //��ѯ�ɼ���������Ҫ��������
  uint8_t  ErrCnt ;         //�ɼ�ʧ�ܸ���
  uint8_t   GatherCnt;
  int16_t  ReadData;   //��ʱ�ɼ�����
  int16_t  ReadCsDataBuff[GATHER_VALUE];   //���CS����ֵ����
  int16_t  ReadVinDataBuff[GATHER_VALUE];  //���VIN����ֵ����

  //--------����������-------
  bool     AverageOkFlag;         //�ɼ����ݳɹ���ʶ
  uint8_t  AverageErrorCnt;       //�ɼ����ݳ������
  int16_t  AverageReadCsData;     //��ż����ԭʼ��ֵ
  int16_t  AverageReadVinData;    //��ż����ԭʼ��ֵ
  int16_t  AverageCaliReadCsData; //���У׼��ĵ���ֵ
  int16_t  AverageCaliReadVinData;//���У׼��ĵ�ѹֵ
  //--------
}ADS112C04_Data_Typedef;
extern ADS112C04_Data_Typedef sADS112C04_Data;

void F_ADS112C04_Init(void);
void F_ADS112C04_AverageInterruptCallback(void);

#endif

