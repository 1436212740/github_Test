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
//=======================�ӿ���������======================
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
 * @bried:  ��ʼ��IIC0�ӿ�
 * @para:  	None	
 * @retval: None
 */
__weak void IIC0_Init(void)
{
	{
		//...		׼˫��IOģʽ   ��   ��©ģʽ(�͵�ƽ�͸���̬)
		//Ӳ��������Ҫ��IIC0������������(4.7K)
	}
	IIC0_SCL_H;
	IIC0_SDA_H;
}

//========================================================
//======================END===============================
//========================================================

/**
 * @bried:  ������ʼ�ź�
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
 * @bried:  ���������ź�
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
 * @bried:  �ȴ�Ack�ź�
 * @para:   None
 * @retval: True--�ӻ�Ӧ��		False--�ӻ���Ӧ��
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
 * @bried:  ����Ӧ���ź�
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
 * @bried:  ������Ӧ���ź�
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
 * @bried:  ��������
 * @para:  	txd--�跢�͵�����
 * @retval: True--���ͳɹ�   False--����ʧ��
 */
bool IIC0_SendWord(uint8_t txd)
{                     
  uint8_t t;     
	IIC0_SCL_L;	
	for(t=0;t<IIC0_BIT_LENGTH;t++)
  {    
		if (txd&IIC0_BIT_MASK)	//�ж����λλ ��λ�Ƚ�
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
 * @bried:  ��ȡһ���ֽ�
 * @para:  	ack--�Ƿ���Ҫ����Ack
 * @retval: ��ȡ������
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
     IIC0_Ack();		//����ACK   
   else
      IIC0_NAck(); 	//����nACK
	
    return receive;
}

/**
 * @bried:  ������
 * @param {uint8_t} addr	:�ӻ���ַ
 * @param {uint8_t} reg	:�Ĵ�����ַ
 * @param {uint8_t} *p		:���ڽ������ݵ��׵�ַ
 * @param {uint8_t} Cnt	:��Ҫ��ȡ�����ݳ���
 * @retval: True--�ɹ�     False--ʧ��
 */
bool IIC0_ReadData(uint8_t reg, uint8_t *p,uint8_t Cnt)
{
	uint8_t i=0;
	IIC0_Start();
	//----------д�ӻ���ַ--------
	if (!IIC0_SendWord(IIC0_WRITE_ADD))
		return false;
	//----------д�Ĵ�����ַ------
	if (!IIC0_SendWord(reg))
		return false;
	IIC0_Start();
	//----------д�ӻ���ַ--------
	if (!IIC0_SendWord(IIC0_READ_ADD))
		return false;
	//----------���Ĵ�������------
	for(i=0;i<(Cnt-1);i++)
		p[i]=IIC0_ReadWord(true);
	p[i]=IIC0_ReadWord(false);
	IIC0_Stop();
	return true;
}


/**
 * @bried:  	д����
 * @retval:  
 * @param {uint8_t} addr				:�ӻ���ַ
 * @param {uint8_t} reg				:�ӻ��Ĵ���
 * @param {uint8_t} *p					:��Ҫ�������ݵ��׵�ַ
 * @param {uint8_t} Cnt				:��Ҫ���͵����ݳ���
 * @retval True		False 
 */
bool IIC0_WriteData(uint8_t reg, uint8_t *p,uint8_t Cnt)
{
	uint8_t i;
	IIC0_Start();
	//--------д�ӻ���ַ--------
	if (!IIC0_SendWord(IIC0_WRITE_ADD))
		return false;
	//--------д�Ĵ�����ַ------
	if (!IIC0_SendWord(reg))
		return false;

	//--------д����-----------
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
//=======================�ӿ���������======================
//========================================================
#define IIC1_SCL_H  		
#define IIC1_SCL_L  	
#define IIC1_SDA_H  		
#define IIC1_SDA_L  	
#define IIC1_SDA_Read 
#define IIC1_DELAY           //  Agreement time 4.7us

/**
 * @bried:  ��ʼ��IIC1�ӿ�
 * @para:  	None	
 * @retval: None
 */
__weak void IIC1_Init(void)
{
	{
		//...		׼˫��IOģʽ   ��   ��©ģʽ(�͵�ƽ�͸���̬)
		//Ӳ��������Ҫ��IIC1������������(4.7K)
	}
	IIC1_SCL_H;
	IIC1_SDA_H;
}

//========================================================
//======================END===============================
//========================================================

/**
 * @bried:  ������ʼ�ź�
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
 * @bried:  ���������ź�
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
 * @bried:  �ȴ�Ack�ź�
 * @para:   None
 * @retval: True--�ӻ�Ӧ��		False--�ӻ���Ӧ��
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
 * @bried:  ����Ӧ���ź�
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
 * @bried:  ������Ӧ���ź�
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
 * @bried:  ��������
 * @para:  	txd--�跢�͵�����
 * @retval: True--���ͳɹ�   False--����ʧ��
 */
bool IIC1_SendWord(uint8_t txd)
{                     
  uint8_t t;     
	IIC1_SCL_L;	
	for(t=0;t<IIC1_BIT_LENGTH;t++)
  {    
		if (txd&IIC1_BIT_MASK)	//�ж����λλ ��λ�Ƚ�
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
 * @bried:  ��ȡһ���ֽ�
 * @para:  	ack--�Ƿ���Ҫ����Ack
 * @retval: ��ȡ������
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
     IIC1_Ack();		//����ACK   
   else
      IIC1_NAck(); 	//����nACK
	
    return receive;
}

/**
 * @bried:  ��ȡ�ӻ�����
 * @param {uint16_IIC1} reg_addr : �ӻ���ַ
 * @param {uint16_IIC1} *p				:���ڽ������ݵ��׵�ַ
 * @param {uint16_IIC1} Cnt			:��Ҫ��ȡ�����ݳ���
 * @retval: True--�ɹ�     False--ʧ��
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
 * @param {uint16_IIC1} reg_addr	:�ӻ���ַ
 * @param {uint16_IIC1} *p 			:��Ҫ�������ݵ��׵�ַ
 * @param {uint16_IIC1} Cnt			:��Ҫ���͵����ݳ���
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

