/*********************************OWLuino*********************************                                      
 
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
#include "tlsr8266_ble_driver.h"
#include "usart_driver.h"
#include "ST_string.h"
#include "SysTick.h"

#ifdef USART1_DEBUG

#define BLE_DEBUG_INFO

#endif


/***********************************************************
**B_RESET---------PC3----------��λ
**B_EN------------PA1---------ʹ��
**B_CAST----------PB13---------�㲥ָʾ
**B_CONN ---------PB14---------����ָʾ
**USART3_TX-------PB10
**USART3_RX-------PB11
************************************************************/
/**********************************************************
**��������:BLE  GPIO��ʼ��
***********************************************************/
void TLSR8266_GPIO_InitConfig(void)
{
	//Output
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//B_RESET
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//B_EN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);

	//Inputs
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}


/**********************************************************
**��������:������ʱ
***********************************************************/
static void TLSR8266_Delay_ms(uint16_t ms)
{
	delay_ms(ms);
}


/**********************************************************
**��������:BLE  ��λ
***********************************************************/
void TLSR8266_ReSet(void)
{
	__BLE_RESET_PORT(BLE_LOW);
	TLSR8266_Delay_ms(100);
	__BLE_RESET_PORT(BLE_HIGH);
}


/**********************************************************
**��������:BLE  ʹ��
***********************************************************/
void TLSR8266_Enable(void)
{
	__BLE_ENABLE_PORT(BLE_LOW);
}


/**********************************************************
**��������:BLE ʧ��
***********************************************************/
void TLSR8266_Disble(void)
{
	__BLE_ENABLE_PORT(BLE_HIGH);
}



/**********************************************************
**��������:���BLE ģ���Ƿ�㲥
**����	ֵ: BLE_LOW-----��ʾģ�����ڹ㲥
**				 BLE_HIGH----��ʾģ��δ�㲥
***********************************************************/
uint8_t TLSR8266_Chack_Radio(void)
{
	return __BLE_READ_RADIO_PORT();
}


/**********************************************************
**��������:���BLE �Ƿ�����
**����	ֵ: BLE_LOW-----BLE ģ���Ѿ�����
**				 BLE_HIGH-----BLEģ��δ����
***********************************************************/
uint8_t TLSR8266_Chack_Connect(void)
{
	return __BLE_READ_CONNECT_PORT();
}



/**********************************************************
**��������:��BLEģ�鷢���ַ���
***********************************************************/
static int TLSR8266_SendStr(unsigned char* str)
{
	return Usart3_Sendstr(str) ;
}

/**********************************************************
**��������:
***********************************************************/
int TLSR8266_SendData(unsigned char buff[],unsigned int len)
{
	return Usart3_SendData(buff, len);
}


/**********************************************************
**��������:��ȡATָ��ͺ󷵻ص�ֵ
***********************************************************/
static void TLSR8266_ReadReturn(unsigned char* buf)
{
	pipe_read(&TLSR8266_pipe, &buf[0]);
	pipe_read(&TLSR8266_pipe, &buf[1]);
	pipe_read(&TLSR8266_pipe, &buf[2]);
	pipe_read(&TLSR8266_pipe, &buf[3]);
	pipe_read(&TLSR8266_pipe, &buf[4]);
	pipe_read(&TLSR8266_pipe, &buf[5]);
}

/**********************************************************
**��������:����洢�ռ�buff�����ùܵ�TLSR8266_pipe
***********************************************************/
static void TLSR8266_ReSetBuff(void)
{
	Clear_USART_Buff(USART3_BUF);
	pipe_init(&TLSR8266_pipe, (uint8_t *)Usart3_buff,USART3_BUFF_SIZE);
}


/**********************************************************
**��������:BLEģ�鷵��ֵ�Ƚ�
***********************************************************/
static uint8_t TLSR8266_RetString_Compare(unsigned char* buf)
{
	if(st_strncmp(TLSR8266_RETURN_STRING_SUCCESS, buf, TLSR8266_RETURN_STRING_SIZE) == 0)
	{
		#ifdef	BLE_DEBUG_INFO
		printf("BLE AT cmd Success!\r\n");
		#endif

		return BLE_SUCCESS;
	}

	return BLE_ERROR;
}


/**********************************************************
**��������:����BLE ģ�� AT ָ������
**����	ֵ:BLE_SUCCESS-----BLE  ģ�� ATָ�������ɹ�
**				BLE_ERROR-------BLEģ�� ATָ������ʧ��
***********************************************************/
uint8_t TLSR8266_Test_Startup(void)
{
	uint8_t buf[6] = {0};
	uint8_t val = 0;

	TLSR8266_ReSetBuff();
	
	if(TLSR8266_SendStr("AT") == st_strlen("AT"))
	{
		printf("+++++++++++++++++++\r\n");
		TLSR8266_Delay_ms(200);
		TLSR8266_ReadReturn(buf);
		#if 1
		printf("buf : %s\r\n",buf);
		printf("Usart3_buff : %slen= %d\r\n",Usart3_buff,st_strlen(Usart3_buff));

		for(val = 0;val < 5;val++)
		{
			if(Usart3_buff[val] == 'O')
			{
				printf("OK len = %d",val);
				break;
			}
		}
		#endif
//		if(st_strncmp("\r\nOK\r\n", buf, 6) == 0)
		if(st_strncmp(TLSR8266_RETURN_STRING_SUCCESS, buf, TLSR8266_RETURN_STRING_SIZE) == 0)
		{
			#ifdef	BLE_DEBUG_INFO
			printf("BLE AT cmd Startup Success!\r\n");
			#endif

			return BLE_SUCCESS;
		}
	}

	return BLE_ERROR;
}



/**********************************************************
**��������:ͨ��ATָ�λģ��
***********************************************************/
uint8_t TLSR8266_CMD_ReSet(void)
{
	uint8_t buf[6] = {0};
	
	TLSR8266_ReSetBuff();
	
	if(TLSR8266_SendStr("AT+RST") == st_strlen("AT+RST"))
	{
		TLSR8266_Delay_ms(200);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}


/**********************************************************
**��������:BLEģ��ָ�Ĭ��
***********************************************************/
uint8_t TLSR8266_RestoreDefault(void)
{
	uint8_t buf[6] = {0};
	
	TLSR8266_ReSetBuff();
	
	if(TLSR8266_SendStr("AT+RESTORE") == st_strlen("AT+RESTORE"))
	{
		TLSR8266_Delay_ms(200);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}



/**********************************************************
**��������:BLE�㲥��������
***********************************************************/
uint8_t TLSR8266_Set_RadioName(unsigned char* name)
{
	uint8_t buf[6] = {0};
	uint8_t name_buf[30] = "AT+NAME=";
	
	TLSR8266_ReSetBuff();

	st_strcat(name_buf,name);
	
	printf("name_buf = %s\r\n",name_buf);
	if(TLSR8266_SendStr(name_buf) == st_strlen(name_buf))
	{
		TLSR8266_Delay_ms(800);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}



/**********************************************************
**��������:���ù㲥��/��
***********************************************************/
uint8_t TLSR8266_Set_Radio_OnOff(uint8_t status)
{
	uint8_t buf[6] = {0};
	uint8_t str_buf[30] = "AT+EN_ADV=";
	
	TLSR8266_ReSetBuff();

	if(status == TLSR8266_ON)
	{
		st_strcat(str_buf,"1");
	}
	else if(status == TLSR8266_OFF)
	{
		st_strcat(str_buf,"0");
	}
	
	if(TLSR8266_SendStr(str_buf) == st_strlen(str_buf))
	{
		TLSR8266_Delay_ms(200);
		TLSR8266_ReadReturn(buf);

		if(status == TLSR8266_ON)
		{
			if(st_strncmp("\r\nOn\r\n", buf, 6) == 0)
			{
				#ifdef	BLE_DEBUG_INFO
				printf("BLE AT cmd Success!\r\n");
				#endif

				return BLE_SUCCESS;
			}
		}
		else if(status == TLSR8266_OFF)
		{
			
			if(st_strncmp("\r\nOff\r\n", buf, 7) == 0)
			{
				#ifdef	BLE_DEBUG_INFO
				printf("BLE AT cmd Success!\r\n");
				#endif

				return BLE_SUCCESS;
			}
		}
	}

	return BLE_ERROR;
}


/**********************************************************
**��������:�����ʵ�����
**��		��:"2400" "4800"  "9600" "19200" "38400" "57600"
**				"74800" "115200"
***********************************************************/
uint8_t TLSR8266_Set_BaudRate(unsigned char* baudrate)
{
	uint8_t buf[6] = {0};
	uint8_t str_buf[30] = "AT+UART=";
	
	TLSR8266_ReSetBuff();

	st_strcat(str_buf,baudrate);
	
	if(TLSR8266_SendStr(str_buf) == st_strlen(str_buf))
	{
		TLSR8266_Delay_ms(200);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}






/**********************************************************
**��������:��ȡBLE MAC��ַ
***********************************************************/
void Get_TLSR8266_MacAddr(unsigned char* mac_addr)
{
	TLSR8266_ReSetBuff();
	if(TLSR8266_SendStr("AT+MAC") == st_strlen("AT+MAC"))
	{
		TLSR8266_Delay_ms(800);
		st_memcpy(mac_addr, (uint8_t *)BLE_Buff, 12);
	}
}


/**********************************************************
**��������:��ȡBLE �㲥����
***********************************************************/
void Get_TLSR8266_RadioName(unsigned char* name)
{
	TLSR8266_ReSetBuff();
	if(TLSR8266_SendStr("AT+MAC") == st_strlen("AT+MAC"))
	{
		TLSR8266_Delay_ms(800);
		st_memcpy(name, (uint8_t *)BLE_Buff, 30);
	}
}

/**********************************************************
**��������:
***********************************************************/
void Get_TLSR8266_BaudRate(unsigned char* baudrate)
{
	TLSR8266_ReSetBuff();
	if(TLSR8266_SendStr("AT+UART") == st_strlen("AT+UART"))
	{
		TLSR8266_Delay_ms(800);
		st_memcpy(baudrate, (uint8_t *)BLE_Buff, 6);
	}
}



/**********************************************************
**��������:
***********************************************************/
/**********************************************************
**��������:���� BLE Server UUID
**TLSR8266_Set_ServerUUID("16","11223344556677889900AABBCCEEDDFF")
***********************************************************/
uint8_t TLSR8266_Set_ServerUUID(unsigned char* len,unsigned char* uuid)
{
	uint8_t buf[6] = {0};
	uint8_t name_buf[60] = "AT+ServiceUUID=";
	
	//AT+ServiceUUID=2,FFF0
	st_strcat(name_buf,len);
	st_strcat(name_buf,",");
	st_strcat(name_buf,uuid);
	
	printf("name_buf = %s\r\n",name_buf);
	
	TLSR8266_ReSetBuff();
	
	if(TLSR8266_SendStr(name_buf) == st_strlen(name_buf))
	{
		TLSR8266_Delay_ms(800);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}


/**********************************************************
**��������:���� BLE Notify Characteristic UUID
***********************************************************/
uint8_t TLSR8266_Set_NotifyCharUUID(unsigned char* len,unsigned char* uuid)
{
	uint8_t buf[6] = {0};
	uint8_t name_buf[60] = "AT+NotifyChar=";
	
	//AT+NotifyChar=2,FFF0
	st_strcat(name_buf,len);
	st_strcat(name_buf,",");
	st_strcat(name_buf,uuid);
	
	printf("name_buf = %s\r\n",name_buf);
	
	TLSR8266_ReSetBuff();
	
	if(TLSR8266_SendStr(name_buf) == st_strlen(name_buf))
	{
		TLSR8266_Delay_ms(800);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}


/**********************************************************
**��������:���� BLE Write Characteristic UUID
***********************************************************/
uint8_t TLSR8266_Set_WriteCharUUID(unsigned char* len,unsigned char* uuid)
{
	uint8_t buf[6] = {0};
	uint8_t name_buf[60] = "AT+WriteChar=";
	
	//AT+WriteChar=2,FFF0
	st_strcat(name_buf,len);
	st_strcat(name_buf,",");
	st_strcat(name_buf,uuid);
	
	printf("name_buf = %s\r\n",name_buf);
	
	TLSR8266_ReSetBuff();
	
	if(TLSR8266_SendStr(name_buf) == st_strlen(name_buf))
	{
		TLSR8266_Delay_ms(800);
		TLSR8266_ReadReturn(buf);

		return TLSR8266_RetString_Compare(buf);
	}

	return BLE_ERROR;
}




Pipe_t TLSR8266_pipe;
uint8_t TLSR8266_pipe_enable = 0;
/**********************************************************
**��������:
***********************************************************/
void TLSR8266_Init(void)
{
	printf("TLSR8266_Init \r\n");
	TLSR8266_SendStr("====Hello\r\n") ;
	TLSR8266_GPIO_InitConfig();
	
	//��ʼ���ܵ�
	pipe_init(&TLSR8266_pipe, (uint8_t *)Usart3_buff,USART3_BUFF_SIZE);
	
	TLSR8266_pipe_enable = 1;

	TLSR8266_Enable();
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);

	if(TLSR8266_Test_Startup() == BLE_SUCCESS)
		printf("TLSR8266_Test_Startup BLE_SUCCESS \r\n");
	else printf("TLSR8266_Test_Startup BLE_ERROR \r\n");
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);
	TLSR8266_SendStr("\r\n") ;
	TLSR8266_Set_RadioName("OWL-IOT32V2.1");
//	TLSR8266_Set_ServerUUID("2","FEE0");
//	TLSR8266_Set_NotifyCharUUID("2","FEE1");
//	TLSR8266_Set_WriteCharUUID("2","FEE2");
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);
	TLSR8266_Delay_ms(1000);

connect:	
	if(!TLSR8266_Chack_Connect())
	{
		printf("BLE Connect SUCCESS \r\n");
		while(1)
		{
			TLSR8266_SendStr("Hello World!!\r\n") ;
			TLSR8266_Delay_ms(1000);
		}
	}
	else
	{
		printf("BLE Connect filed \r\n");
		TLSR8266_Delay_ms(1000);
		goto connect;
	}
}





