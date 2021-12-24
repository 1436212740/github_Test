/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-11-01 14:44:09
 * @LastEditTime: 2021-12-10 10:45:13
 * @Brief:  
 */
#ifndef __IIC_H
#define	__IIC_H

#include <stdbool.h>
#include <stdint.h>

#define  IIC_NUMBER       0    //Multi-channel IIC protocol for different interfaces

#if   (IIC_NUMBER==0)
#define IIC0_BIT_LENGTH		8    //data length 8 bit
#define IIC0_BIT_MASK		((uint8_t)(0x0001<<(IIC0_BIT_LENGTH-1)))
#define IIC0_RD_MASK			((uint8_t)(0x0001<<(IIC0_BIT_LENGTH-1)))

__weak void IIC0_Init(void);
bool IIC0_ReadData(uint8_t reg, uint8_t *p,uint8_t Cnt);    //读写数据
bool IIC0_WriteData(uint8_t reg, uint8_t *p,uint8_t Cnt);

void IIC0_Start(void);
void IIC0_Stop(void);
bool IIC0_WaitAck(void);
void IIC0_Ack(void);
void IIC0_NAck(void);
uint8_t IIC0_ReadWord(bool ack);
bool IIC0_SendWord(uint8_t txd);    //写单个字节

#else
#define IIC1_BIT_LENGTH		8    //data length 8 bit
#define IIC1_BIT_MASK		((uint8_t)(0x0001<<(IIC1_BIT_LENGTH-1)))
#define IIC1_RD_MASK			((uint8_t)(0x0001<<(IIC1_BIT_LENGTH-1)))

__weak void IIC1_Init(void);
bool IIC1_ReadData(uint8_t reg_addr,uint8_t *p,uint8_t Cnt);
bool IIC1_WriteData(uint8_t reg_addr,uint8_t *p,uint8_t Cnt);

#endif


#endif /* __IIC_H */
