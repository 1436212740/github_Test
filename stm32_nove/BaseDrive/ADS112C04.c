/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-08 09:24:00
 * @LastEditTime: 2021-12-21 17:23:13
 * @Brief:  
 */
#include "ADS112C04.h"
#include "ParamCali.h"
#include "usart.h"
#include "LoadControl.h"

ADS112C04_Data_Typedef sADS112C04_Data;

//================本文件内部调用文件=================
  //-----初始化---------
static void IIC0_Init(void);                  //IIC初始化
static void F_ADS112C04_Config(void);         //配置初始化
static void F_StartSign(void);                //开始采集
static void F_ADS112C04_Interrupt_Init(void); //中断配置
static void F_ADS112C04_NVIC_Init(void);      //优先级配置
  //------采集原始值-----
static void F_ADS112C04_Gather(void);         //AD采集函数
static void F_ADS112C04_ReadData(void);       //读取AD数据
static void F_Set_Channel_Gain(void);         //通道切换
#if (HAEDWARE_IIC)
bool IIC0_WriteData(uint8_t reg, uint8_t* value, uint8_t length);    //写
bool IIC0_ReadData(uint8_t reg, uint8_t *value, uint8_t length);      //读
#else
#include "iic.h"
#endif
/**
 * @bried:  IIC0Init的GPIO配置初始化
 * @para:   None
 * @retval: None
 */
static void IIC0_Init(void)
{
  #if(!HAEDWARE_IIC)
  GPIO_InitTypeDef 		GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ADS112C04_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;									//推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADS112C04_SCL_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = ADS112C04_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;									//开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADS112C04_SDA_GPIO, &GPIO_InitStructure);

  ADS112C04_SCL_H;
  ADS112C04_SDA_H;
  #else
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

  I2C_InitStructure.I2C_ClockSpeed = 200000;	//IIC速度
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	//IIC的模式
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	//IIC时钟占空比 低电平比上高电平
	I2C_InitStructure.I2C_OwnAddress1 = 0;	//STM32自己的IIC地址，做主机时随便填，地址有7位和10位之分，具体看I2C_AcknowledgedAddress的
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;	//打开IIC应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1,&I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
  
  #endif
  GPIO_InitStructure.GPIO_Pin = ADS112C04_RDY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;									    //上拉
	GPIO_Init(ADS112C04_RDY_GPIO, &GPIO_InitStructure);
}


/**
 * @bried:  ADS112C04配置
 * @para:   None
 * @retval: None
 */
static void F_ADS112C04_Config(void)
{
  uint8_t wreg_add[4], reg_data[4];

  wreg_add[0] = ADS112C04_WREG0;
  wreg_add[1] = ADS112C04_WREG1;
  wreg_add[2] = ADS112C04_WREG2;
  wreg_add[3] = ADS112C04_WREG3;
  reg_data[0] = ADS112C04_REG0_DATA;
  reg_data[1] = ADS112C04_REG1_DATA;
  reg_data[2] = ADS112C04_REG2_DATA;
  reg_data[3] = ADS112C04_REG3_DATA;
  
  for (uint8_t i = 0; i < 4; i++)
    IIC0_WriteData(wreg_add[i], &reg_data[i], 1);

  sADS112C04_Data.eADS112C04_Channel = CS_CHANNEL;
  sADS112C04_Data.eADS112C04_Gain = GAIN_8;

  //sADS112C04_Data.eADS112C04_Channel = VIN_CHANNEL;
  //sADS112C04_Data.eADS112C04_Gain = GAIN_1;
  F_Set_Channel_Gain();
  F_StartSign();
}


/**
 * @bried:  ADS112C04初始化
 * @para:   None
 * @retval: None
 */
void F_ADS112C04_Init(void)
{
  //------IO配置---------
  IIC0_Init();
  //------ADS112C04内部设置----------
  F_ADS112C04_Config();
  //------采集中断配置------
  F_ADS112C04_Interrupt_Init();
  F_ADS112C04_NVIC_Init();
}



/**
 * @bried:  设置采集通道和增益
 * @para:  
 * @retval:  
 */
static void F_Set_Channel_Gain(void)
{
  uint8_t write_data=(sADS112C04_Data.eADS112C04_Channel<<4)|(sADS112C04_Data.eADS112C04_Gain<<1);
  IIC0_WriteData(ADS112C04_WREG0, &write_data, 1);
  //if(flag)  printf("写入OK\n");
  //else      printf("写入err\n");
}

/**
 * @bried:  读取ADS112C04数据
 * @param {uint16_t*} read_data--用于接受读取的数据
 * @retval: True--成功     False--失败
 */
static void F_ADS112C04_ReadData(void)
{
  uint8_t data[2];
  uint16_t delay=1000;
  //-------准备数据-------
  while (ADS112C04_RDY_READ&&delay)   { delay--;  ;}//等待采集完成
  if(!delay) 
  {
    sADS112C04_Data.ReadData = 0;
    sADS112C04_Data.GatherOkFlag = false;
    return;
  } 
  //if(ADS112C04_RDY_READ)
  //{
  //  sADS112C04_Data.ReadData = 0;
  //  sADS112C04_Data.GatherOkFlag = false;
  //  return ;
  //}
  //-------采集数据-------
  if(IIC0_ReadData(ADS112C04_RDATA, data, 2))
  {
    sADS112C04_Data.ReadData = (data[0]<<8)|(data[1]);
    sADS112C04_Data.GatherOkFlag = true;
  }
  else
  {
    sADS112C04_Data.ReadData = 0;
    sADS112C04_Data.GatherOkFlag = false;
  }
}

//================================================
//=============硬件IIC的读写时序===================
//================================================
#if (HAEDWARE_IIC)
/**
 * @bried:  ADS112C04的开始信号
 * @para:  None
 * @retval:  None
 */
static void F_StartSign(void)
{
  uint32_t delay=0;
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    //I2C1->CR1 &= ~0x8000;
    if(delay++>0x10000)  
    {
      return ;  
    }
  }
  delay = 0;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if(delay++>0x10000)  
    {
      I2C_GenerateSTOP(I2C1, ENABLE);
      return ;  
    }
      
  }
  delay=0;

  I2C_Send7bitAddress(I2C1, 0x80, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if(delay++>0x10000)  
    {
      I2C_GenerateSTOP(I2C1, ENABLE);
      return ;  
    }
      
  }
  delay=0;
  
  I2C_SendData(I2C1, ADS112C04_START);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) 
  {
    if(delay++>0x10000) 
    {
      I2C_GenerateSTOP(I2C1, ENABLE);
      return ;  
    }
      
  }
  delay=0;
  I2C_GenerateSTOP(I2C1, ENABLE);
  while((I2C1->CR1&0x200) == 0x200);
  return ;
}


/**
 * @bried:  IIC的写时序
 * @param {uint8_t} reg---寄存器
 * @param {uint8_t*} value---写入值地址
 * @param {uint8_t} length---数据个数
 * @retval:  成功/失败
 */
bool IIC0_WriteData(uint8_t reg, uint8_t* value, uint8_t length)
{
  uint32_t delay=0;

  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    //I2C1->CR1 &= ~0x8000;
    if(delay++>0x100)  
    {
      //printf("W忙碌中\n");
      return false;  
    }
  }
  delay = 0;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if(delay++>0x100)  
    {
      //printf("W开始\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }
      
  }
  delay=0;

  I2C_Send7bitAddress(I2C1, 0x80, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if(delay++>0x100)  
    {
      //printf("W地址\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }
      
  }
  delay=0;
  
  I2C_SendData(I2C1, reg);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) 
  {
    if(delay++>0x100) 
    {
      //printf("W寄存器\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }
      
  }
  delay=0;

  for(uint8_t i=0; i<length; i++)
  {
    I2C_SendData(I2C1, value[i]);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) 
    {
      if(delay++>0x100)  
      {
        //printf("W数据\n");
        I2C_GenerateSTOP(I2C1, ENABLE);
        return false; 
      }
         
    }
    delay=0;
  }

  I2C_GenerateSTOP(I2C1, ENABLE);
  while((I2C1->CR1&0x200) == 0x200);
  //printf(" W OK\n");
  return true;
}


/**
 * @bried:  IIC的读时序
 * @param {uint8_t} reg---寄存器
 * @param {uint8_t*} value---写入值地址
 * @param {uint8_t} length---数据个数
 * @retval:  成功/失败
 */
bool IIC0_ReadData(uint8_t reg, uint8_t *value, uint8_t length)
{
  uint32_t delay=0;

  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    //I2C1->CR1 &= ~0x8000;
    if(delay++>0x100)  
    {
      //printf("R忙碌中\n");
      return false;  
    }
  }
  delay = 0;
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if(delay++>0x100)  
    {
      //printf("R开始写\n");
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }
      
  }
  delay=0;

  I2C_Send7bitAddress(I2C1, ADS112C04_WRITE_ADD, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if(delay++>0x100)  
    {
      //printf("R地址写\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }   
  }
  delay=0;

  I2C_SendData(I2C1, reg);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) 
  {
    if(delay++>0x100) 
    {
      //printf("R寄存器\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }
      
  }
  delay=0;

  I2C_GenerateSTART(I2C1, ENABLE);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if(delay++>0x100)  
    {
      //printf("R开始\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }
  }
  delay=0;

  I2C_Send7bitAddress(I2C1, ADS112C04_WRITE_ADD, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) 
  {
    if(delay++>0x100)  
    {
      //printf("R地址读\n");
      I2C_GenerateSTOP(I2C1, ENABLE);
      return false;  
    }   
  }
  delay=0;
  while (length)
  {
    if(length==1)
    {
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *value++ = I2C_ReceiveData(I2C1);
    length--;
  }
  //printf(" R OK\n");
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  return true;
}
#else
/**
 * @bried:  ADS112C04的开始信号
 * @para:  None
 * @retval:  None
 */
static void F_StartSign(void)
{
  IIC0_Start();
  IIC0_SendWord(ADS112C04_WRITE_ADD);
  IIC0_SendWord(ADS112C04_START);
  IIC0_Stop();
}
#endif

//================================================
//================中断采集AD数据===================
//================================================
static void F_ADS112C04_Interrupt_Init(void)   //500us采集一次
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);        			//时钟开启	
	
	TIM_DeInit(TIM2);	
	
	TIM_TimeBaseStructure.TIM_Period =36000-1; //0x1C4E<<1;  		       	    	//定时器顶端值设定200us
 
	TIM_TimeBaseStructure.TIM_Prescaler =1;//1 ;		//1    										//定时器分频值设定
 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    			//忽略
 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//向上计时
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
 
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);           			//结束配置
		
	TIM_ARRPreloadConfig(TIM2, ENABLE);	                       			//开关，使能定时器计数功能
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);

	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE );	        				//开关 使能update中断捕获触发中断

	TIM_Cmd(TIM2, ENABLE);                                     			//定时器 总开关
}


/**
  * @brief  设置中断服务函数以及优先级
  * @param  None
  * @retval : None
  */
static void F_ADS112C04_NVIC_Init(void)
{
	NVIC_InitTypeDef 		NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel =  TIM2_IRQn;            			//中断对象_TIM2
 
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;  			//先占优先级0级
 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         			//从优先级3级
 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;            			//IRQ通道
 
	NVIC_Init(&NVIC_InitStruct);                            			//
}


/**
 * @bried:  中断采集
 * @para:  
 * @retval:  
 */
void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
    //==========AD采集操作函数=============
    F_ADS112C04_Gather();   //AD采集
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}


//================================================
//====================采集AD数据===================
//================================================
/**
 * @bried:  AD采集函数
 * @para:  None
 * @retval:  None
 */
static void F_ADS112C04_Gather(void)
{
  if(sADS112C04_Data.StartGather == true)
  {
    F_ADS112C04_ReadData();
    if(sADS112C04_Data.GatherOkFlag)
    {
      if(sADS112C04_Data.eADS112C04_Channel == CS_CHANNEL)
      {
        GPIO_WriteBit(GPIOC, GPIO_Pin_14, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_14)));
        sADS112C04_Data.ReadCsDataBuff[sADS112C04_Data.GatherCnt] = sADS112C04_Data.ReadData;
        //if(sADS112C04_Data.GatherCnt>=GATHER_VALUE) {sADS112C04_Data.GatherCnt=0; sADS112C04_Data.StartGather=false;}
        GPIO_WriteBit(GPIOC, GPIO_Pin_14, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_14)));
      }
      else
      {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));
        sADS112C04_Data.ReadVinDataBuff[sADS112C04_Data.GatherCnt++] = sADS112C04_Data.ReadData;
        if(sADS112C04_Data.GatherCnt>=GATHER_VALUE) {sADS112C04_Data.GatherCnt=0; sADS112C04_Data.StartGather=false;}
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));
      }
    }
    else
    {
      sADS112C04_Data.GatherCnt++;
      if(sADS112C04_Data.GatherCnt>=GATHER_VALUE) {sADS112C04_Data.GatherCnt = 0;  sADS112C04_Data.StartGather=false;}
      sADS112C04_Data.ErrCnt ++;
    }

    if(sADS112C04_Data.eADS112C04_Channel!=CS_CHANNEL)
    {
    sADS112C04_Data.eADS112C04_Channel = CS_CHANNEL;
    sADS112C04_Data.eADS112C04_Gain = GAIN_16;
    }
    else
    {
      sADS112C04_Data.eADS112C04_Channel = VIN_CHANNEL;
      sADS112C04_Data.eADS112C04_Gain = GAIN_1;
    }
    F_Set_Channel_Gain();
  }
}

/**
 * @bried:  计算采集的临时数据(中断回调计算)
 * @para:   None
 * @retval: None
 */
void F_ADS112C04_AverageInterruptCallback(void)
{
  if(sADS112C04_Data.StartGather==false)    //临时数据采集完成
  {
    int32_t sum=sADS112C04_Data.ReadCsDataBuff[0];
    //------计算均值----------
    for(uint8_t i=1; i<GATHER_VALUE; i++)
    {
      sum += sADS112C04_Data.ReadCsDataBuff[i];
    }
    sADS112C04_Data.AverageReadCsData = (sum>>3);   //由总数量绝对

    sum = sADS112C04_Data.ReadVinDataBuff[0];
    for(uint8_t i=1; i<GATHER_VALUE; i++)
    {
      sum += sADS112C04_Data.ReadVinDataBuff[i];
    }
    sADS112C04_Data.AverageReadVinData = (sum>>3);

    //---------------计算校验后的电流值------------
    if(sParamCali.MemoryCaliFlag)     //电流差分方式需要放大一倍
      sADS112C04_Data.AverageCaliReadCsData = (sADS112C04_Data.AverageReadCsData - sParamCali.MemoryAdcCurrOffset);//<<1;
    else
      sADS112C04_Data.AverageCaliReadCsData = (sADS112C04_Data.AverageReadCsData - LOAD_ADC_CC_COMPENSATE);//<<1;
    sADS112C04_Data.AverageCaliReadVinData = sADS112C04_Data.AverageReadVinData;
    //---------------采集数据查看------------------
    if(!sADS112C04_Data.ErrCnt)
    {
      if(sParamCali.StartCali==false)
      {
        //printf("cs :");
        //for(uint16_t i=0;i<GATHER_VALUE;i++)
        //{
        //  printf("%d,",sADS112C04_Data.ReadCsDataBuff[i]);
        //}
        //printf("A:%d,",sADS112C04_Data.AverageReadCsData);
        //printf("vin:");
        //for(uint16_t i=0;i<GATHER_VALUE;i++)
        //{
        //  printf("%d,",sADS112C04_Data.ReadVinDataBuff[i]);
        //}
        //printf("A:%d,",sADS112C04_Data.AverageReadVinData);
        //printf("\n");
      }
    }
    else
    {
      printf("err_cnt:%d\n",sADS112C04_Data.ErrCnt);
    }
    if(sADS112C04_Data.ErrCnt>1)      sADS112C04_Data.AverageOkFlag = false;
    else                              sADS112C04_Data.AverageOkFlag = true;
      
    sADS112C04_Data.AverageErrorCnt = sADS112C04_Data.ErrCnt;
    sADS112C04_Data.StartGather = true;   //开始采集
    sADS112C04_Data.ErrCnt = 0;           //清除失败次数
  }
}

