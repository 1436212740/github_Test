/*
 * @Author: xiaohu
 * @Version: V1.0
 * @Date: 2021-12-09 15:31:02
 * @LastEditTime: 2021-12-13 18:14:09
 * @Brief:  
 */
#ifndef __LOOP_H__
#define __LOOP_H__

#include  "stm32f10x.h"
#include <stdbool.h>

#define TIMER3_TIME					50//unit:us
#define TIMER3_PRESCALER			2			//·ÖÆµ

#define TIMER3_PRESCALER_CODE			TIMER3_PRESCALER - 1
#define TIMER3_PEIOD_CODE					(72*TIMER3_TIME/TIMER3_PRESCALER)	- 1


typedef struct 
{
  uint32_t Time_500us;
  bool    Flag_20ms;
  bool    Flag_100ms;
  bool    Flag_1ms;
  bool    Flag_500us;
}Main_Time_Typedef;

void F_LoopIt_Init(void);
void F_LoopTimeLoopCall(void);
void F_LoopTimeInterruptCall(void);


#endif

