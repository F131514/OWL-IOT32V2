#ifndef _PIPE_H
#define _PIPE_H

#include "stm32f10x.h"
#include <stdbool.h>

typedef struct _Pipe_t {
	uint8_t * beginPtr;	//�ܵ��ڴ���׵�ַ
	uint8_t * headPtr;		//�ܵ�Ԫ���ײ�,ָ����һ��Ҫ�洢�ĵ�ַ
	uint8_t * tailPtr;		//�ܵ�Ԫ��β��,ָ����һ��Ҫ��ȡ�ĵ�ַ
	uint8_t * endPtr;	//�ܵ��ڴ��β�� ,���һ���ڴ��ַ+1
}  Pipe_t;

void pipe_init(Pipe_t *pipe, uint8_t *buffer, uint16_t size);
bool pipe_write(Pipe_t *pipe, uint8_t value);
bool pipe_read(Pipe_t *pipe, uint8_t *value);

#endif
