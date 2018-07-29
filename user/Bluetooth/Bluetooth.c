/*********************************OWL-IOT32*********************************                                      
 
	                         \\\|///
                       \\  - -  //
                        (  @ @  )
+---------------------oOOo-(_)-oOOo-------------------------+
|                                                           |
|                             Oooo                          |
+-----------------------oooO--(   )-------------------------+
                       (   )   ) /
                        \ (   (_/
                         \_)           
***************************************************************************/
#include "Bluetooth.h"
#include "SysTick.h"
#include "usart_driver.h"
#include "ST_string.h"
#include "TimeOut.h"


#ifdef USART1_DEBUG

#define BLE_DEBUG_INFO

#endif

/**********************************************************
**�������ܣ�WIFI ���Žӿڶ��壬��ʼ��
**B_IO0--------------PC3
**B_IO6--------------PB15
**B_RES--------------RESET
**B_EN---------------PA1
**B_Tx---------------USART3_Rx
**B_Rx---------------USART3_Tx
**BCTS---------------PB14--USART3_RTS
**BRTS---------------PB15--USART3_CTS
***********************************************************/
void Bluetooth_RF_BM_S02_Port_GPIO_Init(void)
{
	//����һ��GPIO_InitTypeDef���͵Ľṹ��
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);

	//B_IO0-------PC3------�ָ���������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
	//B_IO6-------PB15------����״ָ̬ʾ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//B_EN--------PA1------ʹ�ܷ��͹㲥
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void Bluetooth_Sendmsg(uint8_t data[],uint8_t len)
{
	int i = 0;
	for(i=0;i<len;i++)
	{
#ifdef USART3_ENABLE
		usart3_putc(data[i]);
#endif
	}
}

//static int Bluetooth_SendStr(unsigned char* str)
//{
//	int ret = 0;
//	
//	while(*str != '\0')
//	{
//		usart3_putc(*str);
//		str++;
//		ret++;
//	}
//	
//	return ret;
//}

static int Bluetooth_SendStr(unsigned char* buff)
{
	int ret = 0;
	#if 1
	while(*buff)
	{
#ifdef USART3_ENABLE
		usart3_putc(*buff);
#endif
		buff++;
		ret++;
	}
	#else
	ret = st_strlen(buff);
	Bluetooth_Sendmsg(buff,ret);
	#endif
	
	return ret;
}

/**********************************************************
**��������:ms��ʱ����
***********************************************************/
void Bluetooth_Delay_ms(uint32_t ms)
{
	delay_ms(ms);
}

/**********************************************************
**��������:����洢�ռ�buff�����ùܵ�TLSR8266_pipe
***********************************************************/
void Bluetooth_ReSetBuff(void)
{
	Clear_USART_Buff(USART3_BUF);
	pipe_init(&Bluetooth_pipe, (uint8_t *)Bluetooth_Buff,USART3_BUFF_SIZE);
}

/**********************************************************
**��������:��ȡATָ��ͺ󷵻ص�ֵ
***********************************************************/
static void Bluetooth_ReadReturn(unsigned char* buf)
{
	pipe_read(&Bluetooth_pipe, &buf[0]);
	pipe_read(&Bluetooth_pipe, &buf[1]);
	pipe_read(&Bluetooth_pipe, &buf[2]);
	pipe_read(&Bluetooth_pipe, &buf[3]);
	pipe_read(&Bluetooth_pipe, &buf[4]);
	pipe_read(&Bluetooth_pipe, &buf[5]);
	pipe_read(&Bluetooth_pipe, &buf[6]);
	pipe_read(&Bluetooth_pipe, &buf[7]);
}

/**********************************************************
**��������:BLEģ�鷵��ֵ�Ƚ�
***********************************************************/
static uint8_t Bluetooth_RetString_Compare(unsigned char* buf)
{
	if(st_strncmp(BLUETOOTH_RETURN_STRING_SUCCESS, buf, BLUETOOTH_RETURN_STRING_SIZE) == 0)
	{
		#ifdef	BLE_DEBUG_INFO
		printf("BLE AT cmd Success!\r\n");
		#endif

		return BLUETOOTH_SUCCESS;
	}

	return BLUETOOTH_ERROR;
}

/**********************************************************
**�������ܣBBluetooth_RF_BM_S02�޸���������
***********************************************************/
uint8_t Bluetooth_RF_BM_S02_change_name(uint32_t id)
{	
	unsigned char BLE_NAME[23];
	uint8_t buf[8] = {0};
	
	sprintf((char*)BLE_NAME,"TTM:REN-OWL-%d",id);
	
	Bluetooth_ReSetBuff();
	
	if(Bluetooth_SendStr(BLE_NAME) == st_strlen(BLE_NAME))
	{
		Bluetooth_Delay_ms(200);
		Bluetooth_ReadReturn(buf);
		
		#if 0
		if(st_strncmp(BLUETOOTH_RETURN_STRING_SUCCESS, buf, BLUETOOTH_RETURN_STRING_SIZE) == 0)
		{
			#ifdef	BLE_DEBUG_INFO
			printf("BLE AT cmd Startup Success!\r\n");
			#endif

			return BLUETOOTH_SUCCESS;
		}
		#endif
		return Bluetooth_RetString_Compare(buf);
	}
	
	return BLUETOOTH_ERROR;
}

/**********************************************************
**�������ܣBBluetooth_RF_BM_S02��ȡMAC��ַ
***********************************************************/
uint8_t Bluetooth_RF_BM_S02_getMAC(void)
{
	uint8_t buf[50] = {0};
	uint8_t i = 0;
	
	Bluetooth_ReSetBuff();
	
	printf("Bluetooth_RF_BM_S02_getMAC()\r\n");
	if(Bluetooth_SendStr("TTM:MAC-?") == st_strlen("TTM:MAC-?"))
	{
		Bluetooth_Delay_ms(200);
//		Bluetooth_ReadReturn(buf);
		for(i = 0;i < 50;i++)
		{
			pipe_read(&Bluetooth_pipe, &buf[i]);
			if(buf[i] == 0)
				break;
		}
		#ifdef BLE_DEBUG_INFO
		printf("%s\r\n",buf);
		#endif
		
		return BLUETOOTH_SUCCESS;
		
//		return Bluetooth_RetString_Compare(buf);
	}
	return BLUETOOTH_ERROR;
}

/**********************************************************
**�������ܣBBluetooth_RF_BM_S02�޸Ĳ�����
***********************************************************/
uint8_t Bluetooth_RF_BM_S02_change_BPS(void)
{
	uint8_t buf[8] = {0};
	
	Bluetooth_ReSetBuff();
	
	if(Bluetooth_SendStr("TTM:BPS-115200") == st_strlen("TTM:BPS-115200"))
	{
		Bluetooth_Delay_ms(200);
		Bluetooth_ReadReturn(buf);
		
		return Bluetooth_RetString_Compare(buf);
	}
	
	return BLUETOOTH_ERROR;
}


/**********************************************************
**�������ܣBBluetooth_RF_BM_S02�޸����Ӽ��ʱ��
***********************************************************/
uint8_t Bluetooth_RF_BM_S02_change_Connect_time(void)
{
	uint8_t buf[8] = {0};
	
	Bluetooth_ReSetBuff();
	
	if(Bluetooth_SendStr("TTM:CIT-100ms") == st_strlen("TTM:CIT-100ms"))
	{
		Bluetooth_Delay_ms(200);
		Bluetooth_ReadReturn(buf);
		
		return Bluetooth_RetString_Compare(buf);
	}
	
	return BLUETOOTH_ERROR;
}


/**********************************************************
**�������ܣBBluetooth_RF_BM_S02��������ģʽ
***********************************************************/
void Bluetooth_RF_BM_S02_Connect_Mode(void)
{
	BM_S02_EN(GPIO_OUT_LOW);
	Bluetooth_Delay_ms(100);
#ifdef	BLE_DEBUG_INFO
	printf("Bluetooth��������ģʽOK\r\n");
#endif
}

/**********************************************************
**�������ܣBBluetooth_RF_BM_S02����͹���ģʽ������˯��ģʽ
***********************************************************/
void Bluetooth_RF_BM_S02_Sleep_Mode(void)
{
	BM_S02_EN(GPIO_OUT_HIGH);
#ifdef	BLE_DEBUG_INFO
	printf("Bluetooth����͹���ģʽOK\r\n");
#endif
}

/**********************************************************
**�������ܣ��ж�ģ���Ƿ�������
**����	ֵ��0------���ӳɹ�----IO6����͵�ƽ
**			1------����ʧ��----IO6����ߵ�ƽ
***********************************************************/
uint8_t Bluetooth_RF_BM_S02_Check_Connect_State(void)
{
	if(BM_S02_Check_Connect_State() == Bit_RESET)
	{	
#ifdef	BLE_DEBUG_INFO
//		printf("Bluetooth Connect OK\r\n");
#endif
		return BLUETOOTH_SUCCESS;
	}
	else
	{
	
#ifdef	BLE_DEBUG_INFO
//		printf("Bluetooth connect failed!\r\n");
#endif
		return BLUETOOTH_ERROR;
	}
}

/**********************************************************
**�������ܣ��ָ���������
**������0��ǳ�ָ�
		1����ָ�
***********************************************************/
void Bluetooth_RF_BM_S02_RestoreFactory(int rank)
{
	int j = 0;
	if(rank)
	{
		BM_S02_RestoreFactory(GPIO_OUT_LOW);
		for(j=0;j<21;j++)
		{
			Bluetooth_Delay_ms(1000);
		}		
		BM_S02_RestoreFactory(GPIO_OUT_HIGH);
	
#ifdef BLE_DEBUG_INFO
		printf("Bluetooth��ָ���������OK��\r\n");
#endif
	}
	else
	{
		BM_S02_RestoreFactory(GPIO_OUT_LOW);
		
		for(j=0;j<5;j++)
		{
			Bluetooth_Delay_ms(1000);
		}
		
		BM_S02_RestoreFactory(GPIO_OUT_HIGH);
	
#ifdef	BLE_DEBUG_INFO
		printf("Bluetoothǳ�ָ���������OK��\r\n");
#endif
	}
}

Pipe_t Bluetooth_pipe;
uint8_t Bluetooth_pipe_enable = 0;
TimeOut Bluetooth_TimeOut;
void Bluetooth_Init(void)
{
	Bluetooth_RF_BM_S02_Port_GPIO_Init();
	
	//��ʼ���ܵ�
	pipe_init(&Bluetooth_pipe, (uint8_t *)Bluetooth_Buff,USART3_BUFF_SIZE);
	
	//��ʱ���Ƶļ�ʱ������TIM2,��TIM2��ʱΪ2ms
	//����Bluetooth ��30ms��ʱ����
	TimeOut_Init(&Bluetooth_TimeOut,50);
	
	Bluetooth_RF_BM_S02_Connect_Mode();
	
	Bluetooth_pipe_enable = 1;
	Bluetooth_Delay_ms(500);
	
	if(Bluetooth_RF_BM_S02_getMAC() == BLUETOOTH_SUCCESS)
	{
		printf("Get MAC Success\r\n");
	}
	else printf("Get MAC Error\r\n");
}

/**********************************************************
**�������ܣ��ж�ģ���Ƿ�������
**����	ֵ��BLUETOOTH_SUCCESS------���ݽ��ճɹ�
**			BLUETOOTH_ERROR--------���ݽ���ʧ��
***********************************************************/
uint8_t IsDataPack_Check(void)
{
	if((TimeOut_IsEnable(&Bluetooth_TimeOut)==TIMEOUT_ENABLE)
					&&(Timeout_Get_Ststus(&Bluetooth_TimeOut)!= TIMEOUT_STATUS_NO))
	{
		if(Bluetooth_RF_BM_S02_Check_Connect_State() == BLUETOOTH_SUCCESS)
		{
			TimeOut_Disable(&Bluetooth_TimeOut);
			return BLUETOOTH_SUCCESS;
		}
	}
	
	return BLUETOOTH_ERROR;
}



