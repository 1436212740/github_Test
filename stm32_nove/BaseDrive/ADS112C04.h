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

#define HAEDWARE_IIC     1    //软/硬件IIC切换

//=======================================================
//======================接口配置=========================
//=======================================================
#define ADS112C04_RDY_GPIO        GPIOB
#define ADS112C04_RDY_PIN         GPIO_Pin_5
#define ADS112C04_RDY_READ        GPIO_ReadInputDataBit(ADS112C04_RDY_GPIO, ADS112C04_RDY_PIN)
#if(!HAEDWARE_IIC)
//-----------模拟IIC---------------------
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

#define ADS112C04_MAX_VALUE       (1U<<15)  //原本16位，差分方向占用一位
#define ADS112C04_REF_VALUE       2750      //unit:mV
#endif
//=======================================================
//======================ADS112C04寄存器配置===============
//========================================================
#define ADS112C04_WRITE_ADD            0x80   //从机地址(由硬件配置)  A0 A1都接GND
#define ADS112C04_READ_ADD             0x81
    //------------------REG0-----------------
#define ADS112C04_MUX             0       //多路选择器(4bit)      
#define ADS112C04_GAIN            0       //增益选择(3bit)        2^n
#define ADS112C04_PGA_BYPASS      0       //PGA开关(1bit)        开启PGA开关后可以增大增益
    //------------------REG1-----------------
#define ADS112C04_DR              7       //采集速率(3bit)        20SPS
#define ADS112C04_MODE            1       //模式选择(1bit)        0--正常模式   1--增强模式(增强模式采样速率翻倍)
#define ADS112C04_CM              1       //转换模式(1bit)        0--单次转换   1--连续转换
#define ADS112C04_VREF            1       //基准选择(2bit)        0--2.048v    1--外部基准    2,3--VCC基准 
#define ADS112C04_TS              0       //温度传感器(1bit)      0--关闭       1--开启
    //------------------REG2-----------------
#define ADS112C04_DRDY            0       //数据准备标志(只读)(1bit)   1--准备好
#define ADS112C04_DCNT            0       //数据计数器(1bit)      0--关闭       1--开启
#define ADS112C04_CRC             0       //数据校验(2bit)
#define ADS112C04_BCS             0       //电流源开关(1bit)      0--关闭       1--开启(用于识别传感器故障)
#define ADS112C04_IDAC            0       //设置电流(3bit)        off
    //------------------REG3-----------------
#define ADS112C04_I1MUX           0       //电流源1通道选择(3bit)
#define ADS112C04_I2MUX           0       //电流源2通道选择(3bit)
#define ADS112C04_RESERVED        0       //保留(2bit)


//=======================================================
//======================内部寄存器对于地址================
//=======================================================
#define ADS112C04_RESET           0x06    //软件复位
#define ADS112C04_START           0x08    //开始转换信号
#define ADS112C04_POWERDOWN       0x02    //掉电模式,关闭内部IDAC,保存寄存器
#define ADS112C04_RDATA           0x10    //读数据
#define ADS112C04_RREG0           0x20    //读REG0
#define ADS112C04_RREG1           0x24    //读REG1
#define ADS112C04_RREG2           0x28    //读REG2
#define ADS112C04_RREG3           0x2C    //读REG3
#define ADS112C04_WREG0           0x40    //写REG0
#define ADS112C04_WREG1           0x44    //写REG1
#define ADS112C04_WREG2           0x48    //写REG2
#define ADS112C04_WREG3           0x4C    //写REG3

#define ADS112C04_REG0_DATA       ((ADS112C04_MUX<<4)|(ADS112C04_GAIN<<1)|(ADS112C04_PGA_BYPASS<<0))         
#define ADS112C04_REG1_DATA       ((ADS112C04_DR<<5)|(ADS112C04_MODE<<4)|(ADS112C04_CM<<3)|(ADS112C04_VREF<<1)|(ADS112C04_TS<<0))
#define ADS112C04_REG2_DATA       ((ADS112C04_DRDY<<7)|(ADS112C04_DCNT<<6)|(ADS112C04_CRC<<4)|(ADS112C04_BCS<<3)|(ADS112C04_IDAC<<0))
#define ADS112C04_REG3_DATA       ((ADS112C04_I1MUX<<5)|(ADS112C04_I2MUX<<2)|(ADS112C04_RESERVED<<0))

//===================================================
//======================数据结构======================
//===================================================
//=========采集通道选择=========
typedef enum
{
      //------采集通道选择--------
  CS_CHANNEL=0,     
  VIN_CHANNEL=6
}ADS112C04_Channel_Typedef;

//========增益选择=========
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
  bool      StartGather;          //开始采集信号
  //---------通道数据----------
  ADS112C04_Channel_Typedef eADS112C04_Channel;     //通道选择
  ADS112C04_Gain_Typedef eADS112C04_Gain;           //增益选择
  
  //---------临时采集----------
  bool      GatherOkFlag;         //采集成功标志
  #define   GATHER_VALUE      8      //轮询采集，所以需要两个数组
  uint8_t  ErrCnt ;         //采集失败个数
  uint8_t   GatherCnt;
  int16_t  ReadData;   //临时采集数据
  int16_t  ReadCsDataBuff[GATHER_VALUE];   //存放CS采样值数据
  int16_t  ReadVinDataBuff[GATHER_VALUE];  //存放VIN采样值数据

  //--------处理后的数据-------
  bool     AverageOkFlag;         //采集数据成功标识
  uint8_t  AverageErrorCnt;       //采集数据出错次数
  int16_t  AverageReadCsData;     //存放计算后原始的值
  int16_t  AverageReadVinData;    //存放计算后原始的值
  int16_t  AverageCaliReadCsData; //存放校准后的电流值
  int16_t  AverageCaliReadVinData;//存放校准后的电压值
  //--------
}ADS112C04_Data_Typedef;
extern ADS112C04_Data_Typedef sADS112C04_Data;

void F_ADS112C04_Init(void);
void F_ADS112C04_AverageInterruptCallback(void);

#endif

