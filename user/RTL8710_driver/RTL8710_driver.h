#ifndef _RTL8710_DRIVER_H_
#define _RTL8710_DRIVER_H_

#include "board_config.h"

#include "pipe.h"

typedef enum{
	WIFI_MODE_AP,
	WIFI_MODE_STA,
	WIFI_MODE_APSTA
}WIFIWork_Mode;


typedef enum{
	WIFI_START_SUCCEED = 0,//wifi �����ɹ���־
	WIFI_SETUP_MODE,//����ģʽ
	WIFI_TRANSFER_MODE,//͸��ģʽ
}WIFI_WORK_STATUS;

typedef enum{
	OPEN_OS_WAKELOCK = 0,//�򿪲���ϵͳ��(ϵͳ��ֹ˯��)
	UNLOCK_OS_WAKELOCK,//�������ϵͳ˯����(ϵͳ����˯��)
	GET_OS_SLEEP_STATUS,//��ȡ����ϵͳ˯����״̬
}__ATSP_PWR_SLEEP;

typedef enum{
	AT_PRINT_ENABLE = 0,//ʹ�ܴ��ڴ�ӡATָ��
	AT_PRINT_DISABLE,//���ô��ڴ�ӡATָ��
}__ATSE_AT_PRINT;


#define WIFI_SUCCESS		0
#define WIFI_ERROR			1


#define WIFI_MEM_BUFF			Usart2_buff
#define WIFI_MEM_BUFF_SIZE		USART2_BUFF_SIZE

typedef struct __tag_WIFI_CONNECT_INFO{
	uint8_t ap_ssdi[40];	//AP��SSID
	uint8_t ap_password[40];	//AP������
	uint8_t ipaddr[16];//������iP��ַ
	uint8_t gatewayip[16];//����
	uint8_t mac_addr[18];//������MAC��ַ
}WIFI_CONNECT_INFO;

extern uint8_t wifi_pipe_enable;
extern Pipe_t WIFI_RTL8710_pipe;


void WIFI_RTL8710_Port_GPIO_Init(void);
void WIFI_RTL8710_Enable(void);
void WIFI_RTL8710_Disable(void);
void WIFI_RTL8710_ReSet(void);

void WIFI_RTL8710_Startup(void);
void WIFI_RTL8710_Init(void);

#endif
