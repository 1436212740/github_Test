/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-11-01 14:44:09
 * @LastEditTime: 2021-12-13 10:28:09
 * @Brief:  
 */
#include "iic.h"
#include "ADS112C04.h"

#if(!HAEDWARE_IIC)
#if   (IIC_NUMBER==0)
//========================================================
//=======================接口配置区域======================
//========================================================
#define IIC0_SCL_H  				ADS112C04_SCL_H
#define IIC0_SCL_L  				ADS112C04_SCL_L
#define IIC0_SDA_H  				ADS112C04_SDA_H
#define IIC0_SDA_L  				ADS112C04_SDA_L
#define IIC0_SDA_Read 			ADS112C04_SDA_READ
#define IIC0_DELAY          ADS112C04_IIC_DELAY //  Agreement time 4.7us
#define IIC0_WRITE_ADD			ADS112C04_WRITE_ADD			
#define	IIC0_READ_ADD				ADS112C04_READ_ADD
/**
 * @bried:  初始化IIC0接口
 * @para:  	None	
 * @retval: None
 */
__weak void IIC0_Init(void)
{
	{
		//...		准双向IO模式   或   开漏模式(低电平和高阻态)
		//硬件配置需要对IIC0总线上拉电阻(4.7K)
	}
	IIC0_SCL_H;
	IIC0_SDA_H;
}

//========================================================
//======================END===============================
//========================================================

/**
 * @bried:  产生开始信号
 * @para:  	None
 * @retval: None
 */
void IIC0_Start(void)
{
	IIC0_SDA_H;    
	IIC0_SCL_H;
	IIC0_DELAY;
	IIC0_SDA_L;
	IIC0_DELAY;
	IIC0_SCL_L;
	IIC0_DELAY;
}  

/**
 * @bried:  产生结束信号
 * @para:  	None
 * @retval: None
 */
void IIC0_Stop(void)
{
	IIC0_SCL_L;
	IIC0_SDA_L;
	IIC0_DELAY;
	IIC0_SCL_H; 
	IIC0_DELAY; 
	IIC0_SDA_H;
}

/**
 * @bried:  等待Ack信号
 * @para:   None
 * @retval: True--从机应答		False--从机非应答
 */
bool IIC0_WaitAck(void)
{
	uint16_t ucErrTime=0;
	IIC0_SDA_L;
	IIC0_DELAY;   
	IIC0_SCL_H;
	IIC0_DELAY;  
	
	IIC0_SDA_H;IIC0_DELAY;
	while(IIC0_SDA_Read)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC0_Stop();
			return false;
		}
	}
	IIC0_SCL_L;
	return true;  
} 

/**
 * @bried:  产生应答信号
 * @para:   None
 * @retval: None
 */
void IIC0_Ack(void)
{
	IIC0_SCL_L;
	IIC0_SDA_L;
	IIC0_DELAY;
	IIC0_SCL_H;
	IIC0_DELAY;
	IIC0_SCL_L;
}

/**
 * @bried:  产生非应答信号
 * @para:   None
 * @retval: None
 */
void IIC0_NAck(void)
{
	IIC0_SCL_L;
	IIC0_SDA_H;
	IIC0_DELAY;
	IIC0_SCL_H;
	IIC0_DELAY;
	IIC0_SCL_L;
}    

/**
 * @bried:  发送数据
 * @para:  	txd--需发送的数据
 * @retval: True--发送成功   False--发送失败
 */
bool IIC0_SendWord(uint8_t txd)
{                     
  uint8_t t;     
	IIC0_SCL_L;	
	for(t=0;t<IIC0_BIT_LENGTH;t++)
  {    
		if (txd&IIC0_BIT_MASK)	//判断最高位位 高位先进
			IIC0_SDA_H;
		else
			IIC0_SDA_L;
		txd<<=1;  
		IIC0_DELAY;  			
		IIC0_SCL_H;
		IIC0_DELAY; 
		IIC0_SCL_L;
		IIC0_DELAY;
  }
	return IIC0_WaitAck();
}    

/**
 * @bried:  读取一个字节
 * @para:  	ack--是否需要发送Ack
 * @retval: 读取的数据
 */
uint8_t IIC0_ReadWord(bool ack)
{
	uint8_t i,receive=0;
	
	for(i=0;i<IIC0_BIT_LENGTH;i++ )
	{
		IIC0_SCL_L; 
		IIC0_DELAY;
		IIC0_SCL_H;
		receive<<=1;
		IIC0_SDA_H;
		IIC0_DELAY;
		if(IIC0_SDA_Read)
			receive++;   
		IIC0_DELAY; 
  }  

   if (ack)
     IIC0_Ack();		//发送ACK   
   else
      IIC0_NAck(); 	//发送nACK
	
    return receive;
}

/**
 * @bried:  读数据
 * @param {uint8_t} addr	:从机地址
 * @param {uint8_t} reg	:寄存器地址
 * @param {uint8_t} *p		:用于接收数据的首地址
 * @param {uint8_t} Cnt	:需要读取的数据长度
 * @retval: True--成功     False--失败
 */
bool IIC0_ReadData(uint8_t reg, uint8_t *p,uint8_t Cnt)
{
	uint8_t i=0;
	IIC0_Start();
	//----------写从机地址--------
	if (!IIC0_SendWord(IIC0_WRITE_ADD))
		return false;
	//----------写寄存器地址------
	if (!IIC0_SendWord(reg))
		return false;
	IIC0_Start();
	//----------写从机地址--------
	if (!IIC0_SendWord(IIC0_READ_ADD))
		return false;
	//----------读寄存器数据------
	for(i=0;i<(Cnt-1);i++)
		p[i]=IIC0_ReadWord(true);
	p[i]=IIC0_ReadWord(false);
	IIC0_Stop();
	return true;
}


/**
 * @bried:  	写数据
 * @retval:  
 * @param {uint8_t} addr				:从机地址
 * @param {uint8_t} reg				:从机寄存器
 * @param {uint8_t} *p					:需要发送数据的首地址
 * @param {uint8_t} Cnt				:需要发送的数据长度
 * @retval True		False 
 */
bool IIC0_WriteData(uint8_t reg, uint8_t *p,uint8_t Cnt)
{
	uint8_t i;
	IIC0_Start();
	//--------写从机地址--------
	if (!IIC0_SendWord(IIC0_WRITE_ADD))
		return false;
	//--------写寄存器地址------
	if (!IIC0_SendWord(reg))
		return false;

	//--------写数据-----------
	for(i=0;i<Cnt;i++)
	{
		if(!IIC0_SendWord(p[i]))
		{
			IIC0_Stop();
			return false;
		}
	}	
	IIC0_Stop();
	return true;
}

#else

//========================================================
//=======================接口配置区域======================
//========================================================
#define IIC1_SCL_H  		
#define IIC1_SCL_L  	
#define IIC1_SDA_H  		
#define IIC1_SDA_L  	
#define IIC1_SDA_Read 
#define IIC1_DELAY           //  Agreement time 4.7us

/**
 * @bried:  初始化IIC1接口
 * @para:  	None	
 * @retval: None
 */
__weak void IIC1_Init(void)
{
	{
		//...		准双向IO模式   或   开漏模式(低电平和高阻态)
		//硬件配置需要对IIC1总线上拉电阻(4.7K)
	}
	IIC1_SCL_H;
	IIC1_SDA_H;
}

//========================================================
//======================END===============================
//========================================================

/**
 * @bried:  产生开始信号
 * @para:  	None
 * @retval: None
 */
void IIC1_Start(void)
{
	IIC1_SDA_H;    
	IIC1_SCL_H;
	IIC1_DELAY;
	IIC1_SDA_L;
	IIC1_DELAY;
	IIC1_SCL_L;
	IIC1_DELAY;
}  

/**
 * @bried:  产生结束信号
 * @para:  	None
 * @retval: None
 */
void IIC1_Stop(void)
{
	IIC1_SCL_L;
	IIC1_SDA_L;
	IIC1_DELAY;
	IIC1_SCL_H; 
	IIC1_DELAY; 
	IIC1_SDA_H;
}

/**
 * @bried:  等待Ack信号
 * @para:   None
 * @retval: True--从机应答		False--从机非应答
 */
bool IIC1_WaitAck(void)
{
	uint8_t ucErrTime=0;
	IIC1_SDA_L;
	IIC1_DELAY;   
	IIC1_SCL_H;
	IIC1_DELAY;  
	
	IIC1_SDA_H;IIC1_DELAY;
	while(IIC1_SDA_Read)
	{
		ucErrTime++;
		if(ucErrTime>1000)
		{
			IIC1_Stop();
			return false;
		}
	}
	IIC1_SCL_L;
	return true;  
} 

/**
 * @bried:  产生应答信号
 * @para:   None
 * @retval: None
 */
void IIC1_Ack(void)
{
	IIC1_SCL_L;
	IIC1_SDA_L;
	IIC1_DELAY;
	IIC1_SCL_H;
	IIC1_DELAY;
	IIC1_SCL_L;
}

/**
 * @bried:  产生非应答信号
 * @para:   None
 * @retval: None
 */
void IIC1_NAck(void)
{
	IIC1_SCL_L;
	IIC1_SDA_H;
	IIC1_DELAY;
	IIC1_SCL_H;
	IIC1_DELAY;
	IIC1_SCL_L;
}    

/**
 * @bried:  发送数据
 * @para:  	txd--需发送的数据
 * @retval: True--发送成功   False--发送失败
 */
bool IIC1_SendWord(uint8_t txd)
{                     
  uint8_t t;     
	IIC1_SCL_L;	
	for(t=0;t<IIC1_BIT_LENGTH;t++)
  {    
		if (txd&IIC1_BIT_MASK)	//判断最高位位 高位先进
			IIC1_SDA_H;
		else
			IIC1_SDA_L;
		txd<<=1;  
		IIC1_DELAY;  			
		IIC1_SCL_H;
		IIC1_DELAY; 
		IIC1_SCL_L;
		IIC1_DELAY;
  }
	return IIC1_WaitAck();
}    

/**
 * @bried:  读取一个字节
 * @para:  	ack--是否需要发送Ack
 * @retval: 读取的数据
 */
uint8_t IIC1_ReadWord(bool ack)
{
	uint8_t i,receive=0;
	
	for(i=0;i<IIC1_BIT_LENGTH;i++ )
	{
		IIC1_SCL_L; 
		IIC1_DELAY;
		IIC1_SCL_H;
		receive<<=1;
		IIC1_SDA_H;
		IIC1_DELAY;
		if(IIC1_SDA_Read)
			receive++;   
		IIC1_DELAY; 
  }  

   if (ack)
     IIC1_Ack();		//发送ACK   
   else
      IIC1_NAck(); 	//发送nACK
	
    return receive;
}

/**
 * @bried:  读取从机数据
 * @param {uint16_IIC1} reg_addr : 从机地址
 * @param {uint16_IIC1} *p				:用于接收数据的首地址
 * @param {uint16_IIC1} Cnt			:需要读取的数据长度
 * @retval: True--成功     False--失败
 */
bool IIC1_ReadData(uint8_t reg_addr,uint8_t *p,uint8_t Cnt)
{
	uint8_t i=0;
	IIC1_Start();
	if (!IIC1_SendWord(reg_addr|IIC1_RD_MASK))
	{
		return false;
	}
	for(i=0;i<(Cnt-1);i++)
	{ 
		p[i]=IIC1_ReadWord(true);
	}
	p[i]=IIC1_ReadWord(false);
	IIC1_Stop();	
	return true;
}

/**
 * @bried:  
 * @param {uint16_IIC1} reg_addr	:从机地址
 * @param {uint16_IIC1} *p 			:需要发送数据的首地址
 * @param {uint16_IIC1} Cnt			:需要发送的数据长度
 * @retval True		False  
 */ 
bool IIC1_WriteData(uint8_t reg_addr,uint8_t *p,uint8_t Cnt)
{
	uint8_t i;
	IIC1_Start();
	if (!IIC1_SendWord(reg_addr))
	{
		return false;
	}
	for(i=0;i<Cnt;i++)
	{
		if(!IIC1_SendWord(p[i]))
		{
			IIC1_Stop();
			return false;
		}
	}	
	IIC1_Stop();
	return true;
}

#endif
#endif

