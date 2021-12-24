/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-07 18:27:58
 * @LastEditTime: 2021-12-16 18:26:20
 * @Brief:  
 */
#include  "TLV5618.h"
#include "usart.h"
TLV5618_Data_Struct sTLV5618_Data;

#define		macSPIx							SPI2
/**
 * @bried:  TVL5618
 * @para:  
 * @retval:  
 */
void F_TVL5618_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
        
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

 GPIO_InitStructure.GPIO_Pin = TLV5618_SCL_PIN;//SCK
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(TLV5618_SCL_GPIO, &GPIO_InitStructure);

//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//MISO
//GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = TLV5618_SDA_PIN;//MOSI
 GPIO_Init(TLV5618_SDA_GPIO, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = TLV5618_CS_PIN;//CS
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(TLV5618_CS_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//CS
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
 SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
 SPI_InitStructure.SPI_CRCPolynomial = 7;
 SPI_Init(SPI1, &SPI_InitStructure);

 ///* Enable SPI1  */
 SPI_Cmd(SPI1, ENABLE);

 sTLV5618_Data.eMode = SLOW_MODE;
 //sTLV5618_Data.WriteOutA = 0;
 //sTLV5618_Data.WriteOutB = 0;
 sTLV5618_Data.WriteOutALast = 0;
 sTLV5618_Data.WriteOutBLast = 0;
 

}


/**
 * @bried:  设置DAC的电压
 * @para:   None(提前改变WriteOutA,B)
 * @retval: None
 */
void F_Set_DAC_Data(void)
{
  int16_t sub_a=0, sub_b=0;

  if(sTLV5618_Data.WriteOutA>0x0fff)  sTLV5618_Data.WriteOutA = 0x0fff;
  if(sTLV5618_Data.WriteOutB>0x0fff)  sTLV5618_Data.WriteOutB = 0x0fff;
  sub_a = 1;
  sub_b = 1;
  //sub_a = sTLV5618_Data.WriteOutA - sTLV5618_Data.WriteOutALast;
  //sub_b = sTLV5618_Data.WriteOutB - sTLV5618_Data.WriteOutBLast;
  //sTLV5618_Data.WriteOutALast = sTLV5618_Data.WriteOutA;
  //sTLV5618_Data.WriteOutBLast = sTLV5618_Data.WriteOutB;

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);    //等待传输完成
  TLV5618_CS_L;
  
  if((sub_a)||((sub_b)))
  {
    uint16_t write;
    if((sub_a)&&(sub_b))    //A B 同时设置
    {
      write = (sTLV5618_Data.eMode<<14)|sTLV5618_Data.WriteOutB|0x1000;
      SPI_I2S_SendData(SPI1, write);
      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
      TLV5618_CS_H;
      TLV5618_CS_L;
      write = (sTLV5618_Data.eMode<<14)|sTLV5618_Data.WriteOutA|0x8000;
      SPI_I2S_SendData(SPI1, write);
    }
    else if(sub_a)           //设置A
    {
      write = (sTLV5618_Data.eMode<<14)|sTLV5618_Data.WriteOutA|0x8000;
      SPI_I2S_SendData(SPI1, write);
    }
    else if(sub_b)           //设置B
    {
      write = (sTLV5618_Data.eMode<<14)|sTLV5618_Data.WriteOutB|0x0000;
      SPI_I2S_SendData(SPI1, write);
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
    TLV5618_CS_H;
  }
}
