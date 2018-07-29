#ifndef _TIMEOUT_H_
#define _TIMEOUT_H_


#include "stm32f10x.h"

#define TIMEOUT_STATUS_NO	0		//δ��ʱ״̬

#define TIMEOUT_ENABLE		0
#define TIMEOUT_DISABLE		1

typedef struct __arg_TimeOut
{
	int Count;	//����ֵ
	int TimeOut;//��ʱֵ
	int status;//��ʱ״̬  ����
	uint8_t cmd;//ʹ��/ʧ��
}TimeOut;


void TimeOut_Init(TimeOut *timeout,int timeout_val);
void TimeOut_Reset(TimeOut *timeout);
void TimeOut_Enable(TimeOut *timeout);
void TimeOut_Disable(TimeOut *timeout);
uint8_t TimeOut_IsEnable(TimeOut *timeout);
uint8_t Timeout_Val_add(TimeOut *timeout);
void Timeout_Clear_CountVal(TimeOut *timeout);
int Timeout_Get_CountVal(TimeOut *timeout);
int Timeout_Get_timeoutVal(TimeOut *timeout);
int Timeout_Get_Ststus(TimeOut *timeout);

#endif

