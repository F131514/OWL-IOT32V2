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
#include "RTL8710_driver.h"
#include "usart_driver.h"
#include "ST_string.h"
#include "SysTick.h"

#include <stdlib.h>


#ifdef USART1_DEBUG

#define WIFI_DEBUG_INFO

#endif

/*********************************************
**�������ܣ���RTL8710����ATָ��
**********************************************/
void WIFI_RTL8710_Send_data(uint8_t* str)
{
	Usart2_Sendstr(str) ;
}




/**********************************************************
**��������:������ʱ
***********************************************************/
static void WIFI_RTL8710_Delay_ms(uint16_t ms)
{
	delay_ms(ms);
}

/**********************************************************
**��������:��BLEģ�鷢���ַ���
***********************************************************/
static int WIFI_RTL8710_SendStr(unsigned char* str)
{
	return Usart2_Sendstr(str) ;
}



/**********************************************************
**��������:����洢�ռ�buff�����ùܵ�WIFI_RTL8710_pipe
***********************************************************/
static void WIFI_RTL8710_ReSetBuff(void)
{
	Clear_USART_Buff(USART2_BUF);
	pipe_init(&WIFI_RTL8710_pipe, (uint8_t *)WIFI_MEM_BUFF,WIFI_MEM_BUFF_SIZE);
}

/**********************************************************
**��������:��ȡATָ��ͺ󷵻ص�ֵ
***********************************************************/
void WIFI_RTL8710_ReadReturn(unsigned char* buf)
{
	pipe_read(&WIFI_RTL8710_pipe, &buf[0]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[1]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[2]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[3]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[4]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[5]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[6]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[7]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[8]);
	pipe_read(&WIFI_RTL8710_pipe, &buf[9]);
}

/**********************************************************
**��������:BLEģ�鷵��ֵ�Ƚ�
***********************************************************/
static uint8_t WIFI_RTL8710_RetString_Compare(unsigned char* buf)
{
	uint16_t i = 0;
	
	for(i = 0;i < WIFI_MEM_BUFF_SIZE;i++)
	{
		if((buf[i] == 'O')&&(buf[i+1] == 'K'))
		{
			return WIFI_SUCCESS;
		}
	}

	return WIFI_ERROR;
}

/**********************************************************
**��������:����WIFI ģ�� AT ָ������
**����	ֵ:WIFI_SUCCESS-----BLE  ģ�� ATָ�������ɹ�
**				WIFI_ERROR-------BLEģ�� ATָ������ʧ��
***********************************************************/
uint8_t WIFI_RTL8710_Test_Startup(void)
{
//	uint8_t buf[10] = {0};

	
	WIFI_RTL8710_ReSetBuff();
	printf("WIFI_RTL8710_Test_Startup\r\n");
	
	if(WIFI_RTL8710_SendStr("AT\r\n") == st_strlen("AT\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
//		WIFI_RTL8710_ReadReturn( buf);
//		printf("WIFI buf: %s\r\n  len = %d\r\n",buf,st_strlen(buf));
//		printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",Usart2_buff,st_strlen(Usart2_buff));

		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}

#ifdef WIFI_DEBUG_INFO
	printf("WIFI AT cmd Error!\r\n");
#endif

	return WIFI_ERROR;
}


/*********************************************************
**�������ܣ�WIFIģ������
**********************************************************/
void WIFI_RTL8710_Reboot(void)
{
	WIFI_RTL8710_ReSetBuff();
	
	if(WIFI_RTL8710_SendStr("ATSR\r\n") == st_strlen("ATSR\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return;
		}
	}

#ifdef WIFI_DEBUG_INFO
	printf("WIFI AT cmd Error!\r\n");
#endif
}


/*************************************************************
**�������ܣ�����ģ�����ʡ���Դģʽ
**��		��:OPEN_OS_WAKELOCK-----//�򿪲���ϵͳ��(ϵͳ��ֹ˯��)
**				UNLOCK_OS_WAKELOCK---//�������ϵͳ˯����(ϵͳ����˯��)
**				GET_OS_SLEEP_STATUS---//��ȡ����ϵͳ˯����״̬
**����	ֵ��������ΪOPEN_OS_WAKELOCK/UNLOCK_OS_WAKELOCKʱ
**				    WIFI_SUCCESS------���óɹ�
**				    WIFI_ERROR------����ʧ��
**����	ֵ��������ΪGET_OS_SLEEP_STATUSʱ
**				    '1'------OPEN_OS_WAKELOCKģʽ
**				    '0'------UNLOCK_OS_WAKELOCKģʽ
**************************************************************/
uint8_t WIFI_RTL8710_Set_Sleep(uint8_t param)
{
	uint8_t buf[36] = "ATSP=";

	WIFI_RTL8710_ReSetBuff();
	
	if(param == OPEN_OS_WAKELOCK)
	{
		st_strcat(buf,"a");
		st_strcat(buf,"\r\n");
		
		if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
		{
			WIFI_RTL8710_Delay_ms(100);
			if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
			{
				#ifdef WIFI_DEBUG_INFO
				printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
				printf("WIFI AT cmd Success!\r\n");
				#endif
				return WIFI_SUCCESS;
			}
		}
	}
	else if(param == UNLOCK_OS_WAKELOCK)
	{
		st_strcat(buf,"r");
		st_strcat(buf,"\r\n");

		if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
		{
			WIFI_RTL8710_Delay_ms(100);
			if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
			{
				#ifdef WIFI_DEBUG_INFO
				printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
				printf("WIFI AT cmd Success!\r\n");
				#endif
				return WIFI_SUCCESS;
			}
		}
	}
	else if(param == GET_OS_SLEEP_STATUS)
	{
		uint16_t i = 0;
		
		st_strcat(buf,"?");
		st_strcat(buf,"\r\n");
	
		for(i = 0;i < WIFI_MEM_BUFF_SIZE;i++)
		{
			if((buf[i] == '[')&&(buf[i+1] == 'A')&&(buf[i+2] == 'T')&&(buf[i+3] == 'S')&&(buf[i+4] == 'P')
				&&(buf[i+5] == ']')&&(buf[i+6] == ' '))
			{
				return buf[i+7];
			}
		}
	}

	return WIFI_ERROR;
}

/*************************************************************
**�������ܣ����ô����Ƿ��ӡATָ��
**��		��:	AT_PRINT_ENABLE---���ô��ڴ�ӡATָ��
**				AT_PRINT_DISABLE--���ô��ڴ�ӡATָ��
**����	ֵ��WIFI_SUCCESS------���óɹ�
**				    WIFI_ERROR--------����ʧ��
**************************************************************/
uint8_t WIFI_RTL8710_Set_AT_Print(uint8_t param)
{
	uint8_t buf[36] = "ATSE=";

	WIFI_RTL8710_ReSetBuff();
	
	if(param == AT_PRINT_ENABLE)
	{
		st_strcat(buf,"1");
		st_strcat(buf,"\r\n");
	}
	else if(param == AT_PRINT_DISABLE)
	{
		st_strcat(buf,"0");
		st_strcat(buf,"\r\n");
	}

	if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}

	return WIFI_ERROR;
}

/**********************************************************
**��������:WIFIģ��ָ�Ĭ��
***********************************************************/
uint8_t WIFI_RTL8710_RestoreDefault(void)
{
	WIFI_RTL8710_ReSetBuff();
	
	if(WIFI_RTL8710_SendStr("ATSY\r\n") == st_strlen("ATSY\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}

	return WIFI_ERROR;
}

/**********************************************************
**��������:�����ʵ�����
**��		��:"2400" "4800"  "9600" "19200" "38400" "57600"
**				"74800" "115200"
***********************************************************/
uint8_t WIFI_RTL8710_Set_BaudRate(unsigned char* baudrate)
{
	uint8_t str_buf[30] = "ATSU=";
	uint8_t buf[] = ",8,1,0,0,1\r\n";

	if(baudrate == NULL)
		return WIFI_ERROR;

	st_strcat(str_buf,baudrate);
	st_strcat(str_buf,buf);

	WIFI_RTL8710_ReSetBuff();
	
	if(WIFI_RTL8710_SendStr(str_buf) == st_strlen(str_buf))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}

	return WIFI_ERROR;
}

/***********************************************
**�������ܣ�����WiFi�Ĺ���ģʽ AP--STA--APSTA
************************************************/
uint8_t Set_RTL8710_Work_mode(WIFIWork_Mode mode)
{
	uint8_t str_buf[30] = "ATPW=" ;
	
	WIFI_RTL8710_ReSetBuff();
	
	switch(mode)
	{
		case WIFI_MODE_AP:
			st_strcat(str_buf,"2\r\n");
			
			//ATPA=<ssid>,<pwd>,<chl>,<hidden>
			WIFI_RTL8710_Send_data("ATPA=OWLunio,123456,11,0\r\n");
			break;
		case WIFI_MODE_STA:
			st_strcat(str_buf,"1\r\n");
			break;
		case WIFI_MODE_APSTA:
			st_strcat(str_buf,"3\r\n");
			WIFI_RTL8710_Send_data("ATPA=OWL-IOT32,123456,11,0\r\n");
			break;
	}

	if(WIFI_RTL8710_SendStr(str_buf) == st_strlen(str_buf))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			if(mode == WIFI_MODE_STA)
				return WIFI_SUCCESS;
		}
		else return WIFI_ERROR;
	}

	if((mode == WIFI_MODE_AP)||(mode == WIFI_MODE_APSTA))
	{
		WIFI_RTL8710_ReSetBuff();
		if(WIFI_RTL8710_SendStr("ATPA=OWLunio,123456,11,0\r\n") == st_strlen("ATPA=OWLunio,123456,11,0\r\n"))
		{
			WIFI_RTL8710_Delay_ms(100);
			if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
			{
				#ifdef WIFI_DEBUG_INFO
				printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
				printf("WIFI AT cmd Success!\r\n");
				#endif
				return WIFI_SUCCESS;
			}
		}
	}

	return WIFI_ERROR;
}


/*******************************************************************
**�������ܣ�����AP
********************************************************************/
uint8_t WIFI_RTL8710_Connect_AP(uint8_t* ssid,uint8_t* passwd)
{
	uint8_t buf[50] = "ATPN=";
	
	st_strcat(buf,ssid);
	st_strcat(buf,",");
	st_strcat(buf,passwd);
	st_strcat(buf,"\r\n");
#ifdef WIFI_DEBUG_INFO	
	printf("Send: %s\r\n",buf);
#endif	
	
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
	{
		uint8_t i = 0;
		for(i = 0;i < 10;i++)
			WIFI_RTL8710_Delay_ms(1000);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
		else	//��δ���ӳɹ�,������һ��
		{
			for(i = 0;i < 20;i++)
				WIFI_RTL8710_Delay_ms(1000);
			if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
			{
				#ifdef WIFI_DEBUG_INFO
				printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
				printf("WIFI AT cmd Success!\r\n");
				#endif
				return WIFI_SUCCESS;
			}
		}
	}

	return WIFI_ERROR;
}


/*******************************************************************
**�������ܣ��Ͽ�����AP
********************************************************************/
uint8_t WIFI_RTL8710_NotConnect_AP(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATWD\r\n") == st_strlen("ATWD\r\n"))
	{
			WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}


/****************************************************************
**�������ܣ�WIFIɨ���ܱ�·����
*****************************************************************/
uint8_t WIFI_RTL8710_Scan_Router(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATWS\r\n") == st_strlen("ATWS\r\n"))
	{
		uint8_t i = 0;
		for(i = 0;i < 10;i++)
			WIFI_RTL8710_Delay_ms(1000);
//		if(WIFI_RTL8710_RetString_Compare(WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}

/*******************************************************************
**�������ܣ���������ʱ�Զ�����AP
********************************************************************/
uint8_t WIFI_RTL8710_Auto_Connect(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATPG=1\r\n") == st_strlen("ATPG=1\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}

/*******************************************************************
**�������ܣ�STAģʽ��,Ϊģ�����þ�̬IP
**ip_addr--------IP��ַ
**gateway_mask---��ʽ "192.168.1.1,255.255.255.0"
********************************************************************/
uint8_t WIFI_RTL8710_Setup_StaticIP(uint8_t* ip_addr,uint8_t* gateway_mask)
{	
	uint8_t buf[50] = "ATPE=";
	
	st_strcat(buf,ip_addr);
	st_strcat(buf,",");
	st_strcat(buf,gateway_mask);
	st_strcat(buf,"\r\n");
	//ATPE=<ip>(,<gateway>,<mask>)
//	WIFI_RTL8710_Send_data("ATPE=192.168.1.254,192.168.1.1,255.255.255.0");
#ifdef WIFI_DEBUG_INFO	
	printf("Send: %s\r\n",buf);
#endif
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}


/*******************************************************************
**�������ܣ�SmartConfig����WiFi
********************************************************************/
uint8_t WIFI_RTL8710_SmartConfig_Setup_Connect(void)
{
	printf("WIFI_RTL8710_SmartConfig_Setup_Connect!\r\n");
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATWQ\r\n") == st_strlen("ATWQ\r\n"))
	{
		WIFI_RTL8710_Delay_ms(1000);
		
//		#ifdef WIFI_DEBUG_INFO
//		printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
//		printf("WIFI AT cmd Success!\r\n");
//		#endif
		{
			uint16_t i = 0;
	
			for(i = 0;i < WIFI_MEM_BUFF_SIZE;i++)
			{
				if((WIFI_MEM_BUFF[i] == 'A')&&(WIFI_MEM_BUFF[i+1] == 'T')&&
					(WIFI_MEM_BUFF[i+2] == 'W')&&(WIFI_MEM_BUFF[i+3] == 'Q'))
				{
					#ifdef WIFI_DEBUG_INFO
					printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
					printf("WIFI AT cmd Success!\r\n");
					#endif
					return WIFI_SUCCESS;
				}
			}
		}
	}
	return WIFI_ERROR;
}

/****************************************************************
**�������ܣ��ر���������
*****************************************************************/
uint8_t WIFI_RTL8710_CloseAll_Connection(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATPD=0\r\n") == st_strlen("ATPD=0\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}

/****************************************************************
**�������ܣ�ʹ��͸��ģʽ
*****************************************************************/
uint8_t WIFI_RTL8710_Enable_TransparentTransmission_mode(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATPU=1\r\n") == st_strlen("ATPU=1\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}

/****************************************************************
**�������ܣ����봫��ģʽ
*****************************************************************/
uint8_t WIFI_RTL8710_Enter_Transmission_mode(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATPT\r\n") == st_strlen("ATPT\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}

/****************************************************************
**�������ܣ�����������Ϣ��flash
*****************************************************************/
uint8_t WIFI_RTL8710_Save_info_To_Flash(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATPL=1\r\n") == st_strlen("ATPL=1\r\n"))
	{
		WIFI_RTL8710_Delay_ms(100);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}


/*************************************************************
**�������ܣ�����WIFI RTL8710ΪTCP�ͻ���ģʽ
**��	����IP_Addr------ָ��ָ��������˿ںź�IP��ַ
**			��ʽ��42.62.41.209,04002
**����	ֵ���ɹ�
**************************************************************/
uint8_t WIFI_RTL8710_Set_TCPClient_Mode(uint8_t* IP_Addr)
{
	uint8_t buf[36] = "ATPC=0,";
	
	st_strcat(buf,IP_Addr);
	st_strcat(buf,"\r\n");
#ifdef WIFI_DEBUG_INFO	
	printf("Send: %s\r\n",buf);
#endif

	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
	{
		uint8_t i = 0;
		for(i = 0;i < 10;i++)
			WIFI_RTL8710_Delay_ms(1000);
		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
		else	//�ٵȴ�һ��ʱ��
		{
			for(i = 0;i < 20;i++)
				WIFI_RTL8710_Delay_ms(1000);
			if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
			{
				#ifdef WIFI_DEBUG_INFO
				printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
				printf("WIFI AT cmd Success!\r\n");
				#endif
				return WIFI_SUCCESS;
			}
		}
	}
	return WIFI_ERROR;
}


/*************************************************************
**�������ܣ������������״̬
**����	ֵ��WIFI_ERROR------����ʧ��
**				    WIFI_SUCCESS------���ӳɹ�
**************************************************************/
uint8_t WIFI_RTL8710_Check_NetWork_Status(void)
{
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATPI\r\n") == st_strlen("ATPI\r\n"))
	{
			WIFI_RTL8710_Delay_ms(100);
//		if(WIFI_RTL8710_RetString_Compare(WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}

/*************************************************************
**�������ܣ�ͨ��ping��֤��������
**��	����IP_Addr------IP��ַ
**			��ʽ��192.168.1.1
**����	ֵ���ɹ�
**************************************************************/
uint8_t WIFI_RTL8710_Ping(uint8_t* IP_Addr)
{
	uint8_t buf[36] = "ATPP=";
	
	st_strcat(buf,IP_Addr);
	st_strcat(buf,"\r\n");
#ifdef WIFI_DEBUG_INFO	
	printf("Send: %s\r\n",buf);
#endif

	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr(buf) == st_strlen(buf))
	{
		uint8_t i = 0;
		for(i = 0;i < 10;i++)
			WIFI_RTL8710_Delay_ms(1000);
//		if(WIFI_RTL8710_RetString_Compare((uint8_t *)WIFI_MEM_BUFF) == WIFI_SUCCESS)
		{
			#ifdef WIFI_DEBUG_INFO
			printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
			printf("WIFI AT cmd Success!\r\n");
			#endif
			return WIFI_SUCCESS;
		}
	}
	return WIFI_ERROR;
}



/**********************************************************
**�������ܣ�WIFI ���Žӿڶ��壬��ʼ��
**CHIP_EN--------------PB9
***********************************************************/
void WIFI_RTL8710_Port_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}

/*********************************************************
**�������ܣ�WIFIģ��ʹ��
**********************************************************/
void WIFI_RTL8710_Enable(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
}

/*********************************************************
**�������ܣ�WIFIģ��ʹ��
**********************************************************/
void WIFI_RTL8710_Disable(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}


/*
**��������:��ȡWiFi���ӵ���Ϣ
Wifi��Ϣ����ʾΪ��wifiģʽ��SSID,Ƶ������ȫģʽ����wep��ԿID��,����, �豸mac��ַ, �豸IP, ����
STA,TP-LINK_dfq,1,AES,wrw112358,00:09:30:98:d6:1d,192.168.1.107,192.168.1.1

[ATW?] OK
*/
uint8_t Read_wifi_Connect_Info(WIFI_CONNECT_INFO* info)
{
	uint16_t i = 0;
	uint8_t ret = WIFI_ERROR;
	uint8_t flags = WIFI_ERROR;
	
	if(info == NULL)
		return WIFI_ERROR;
	
	for(i = 0;i < WIFI_MEM_BUFF_SIZE;i++)
	{
		if((WIFI_MEM_BUFF[i] == 'O')&&(WIFI_MEM_BUFF[i+1] == 'K'))
		{
			flags = WIFI_SUCCESS;
			break;
		}
	}
	if(flags != WIFI_SUCCESS)
	{
		return WIFI_ERROR;
	}
	
	{
		uint8_t num = 0;
		uint8_t j = 0;
		uint8_t count = 0;
		
		st_memset((unsigned char*)info,0,sizeof(WIFI_CONNECT_INFO));
		//STA,TP-LINK_dfq,1,AES,wrw112358,00:09:30:98:d6:1d,192.168.1.107,192.168.1.1
		//get ap ssid
		for(i = 0;i < WIFI_MEM_BUFF_SIZE;i++)
		{
			if(WIFI_MEM_BUFF[i] == ',')
			{
				num++;
			}
			else continue;
			
			switch(num)
			{
				case 1:	//ap ssid
					count = 0;
					for(j = i+1;j < i+1+40;j++)
					{
						if(WIFI_MEM_BUFF[j] != ',')
						{
							info->ap_ssdi[count] = WIFI_MEM_BUFF[j];
							count++;
						}
						else if(WIFI_MEM_BUFF[j] == ',')
							break;
					}
					break;
				case 2: break;
				case 3: break;
				case 4:	//ap password
					count = 0;
					for(j = i+1;j < i+1+40;j++)
					{
						if(WIFI_MEM_BUFF[j] != ',')
						{
							info->ap_password[count] = WIFI_MEM_BUFF[j];
							count++;
						}
						else if(WIFI_MEM_BUFF[j] == ',')
							break;
					}
					break;
				case 5:	//MAC addr
					count = 0;
					for(j = i+1;j < i+1+18;j++)
					{
						if(WIFI_MEM_BUFF[j] != ',')
						{
							info->mac_addr[count] = WIFI_MEM_BUFF[j];
							count++;
						}
						else if(WIFI_MEM_BUFF[j] == ',')
							break;
					}
					break;
				case 6:	//ipaddr
					count = 0;
					for(j = i+1;j < i+1+16;j++)
					{
						if(WIFI_MEM_BUFF[j] != ',')
						{
							info->ipaddr[count] = WIFI_MEM_BUFF[j];
							count++;
						}
						else if(WIFI_MEM_BUFF[j] == ',')
							break;
					}
					break;
				case 7:	//gatewayip
					{
						uint8_t back = 0;
						
						count = 0;
						for(j = i+1;j < i+1+16;j++)
						{
							
							info->gatewayip[count] = WIFI_MEM_BUFF[j];
							count++;
							if(back == 3)
								break;
							if(WIFI_MEM_BUFF[j] == '.')
							{
								back++;
							}
						}
					}
					
					ret = WIFI_SUCCESS;
					break;
			}
			
			if(ret == WIFI_SUCCESS)
				break;
		}
	}
	
	return ret;
}
/*******************************************************************
**�������ܣ���ѯWiFi��ǰ������Ϣ
********************************************************************/
uint8_t WIFI_RTL8710_Info(WIFI_CONNECT_INFO* info)
{
	#ifdef WIFI_DEBUG_INFO
	printf("WIFI_RTL8710_Info!\r\n");
	#endif
	WIFI_RTL8710_ReSetBuff();
	if(WIFI_RTL8710_SendStr("ATW?\r\n") == st_strlen("ATW?\r\n"))
	{
		WIFI_RTL8710_Delay_ms(1000);
		WIFI_RTL8710_Delay_ms(1000);

		{
			uint16_t i = 0;
	
			for(i = 0;i < WIFI_MEM_BUFF_SIZE;i++)
			{
				if((WIFI_MEM_BUFF[i] == 'O')&&(WIFI_MEM_BUFF[i+1] == 'K'))
				{
					#ifdef WIFI_DEBUG_INFO
					printf("WIFI Usart2_buff: \r\n%s\r\n  len = %d\r\n",(uint8_t *)WIFI_MEM_BUFF,st_strlen((uint8_t *)WIFI_MEM_BUFF));
					printf("WIFI AT cmd Success!\r\n");
					#endif
					
					return Read_wifi_Connect_Info(info);
//					return WIFI_SUCCESS;
				}
			}
		}
	}
	return WIFI_ERROR;
}


/****************************************************************
**�������ܣ�WIFIģ���ʼ��
*****************************************************************/
void WIFI_RTL8710_Startup(void)
{
	WIFI_RTL8710_Port_GPIO_Init();
	WIFI_RTL8710_Enable();
}

uint8_t wifi_pipe_enable = 0;
Pipe_t WIFI_RTL8710_pipe;
WIFI_CONNECT_INFO wifi_info;
/****************************************************************
**�������ܣ�WIFIģ���ʼ��
*****************************************************************/
void WIFI_RTL8710_Init(void)
{
	printf("WIFI_RTL8710_Init()!!\r\n");
	

	//��ʼ���ܵ�
	pipe_init(&WIFI_RTL8710_pipe, (uint8_t *)WIFI_MEM_BUFF,USART2_BUFF_SIZE);

	wifi_pipe_enable = 1;

	{
		uint8_t timerout = 0;
		//����AT����ָ��,���AT����ʧ��10s���˳�WiFi��ʼ��
		while(WIFI_RTL8710_Test_Startup() != WIFI_SUCCESS)
		{
			timerout++;
			if(timerout > 10)
			{
				#ifdef WIFI_DEBUG_INFO
				printf(" WIFI RTL8710 Init ERROR!!\r\n");
				#endif
//				return;
				break;
			}
			WIFI_RTL8710_Delay_ms(1000);
		}
	}

	// 1.close all connection
	WIFI_RTL8710_CloseAll_Connection();

	WIFI_RTL8710_Scan_Router();

	// 3.connect to AP
	#if 0
//	Set_RTL8710_Work_mode(WIFI_MODE_STA);
	{
		uint8_t timerout = 0;
//		while(WIFI_RTL8710_Connect_AP("Orz","SZfs2016") != WIFI_SUCCESS)
		while(WIFI_RTL8710_Connect_AP("TP-LINK_dfq","wrw112358") != WIFI_SUCCESS)
		{
			timerout++;
			if(timerout > 3)
			{
				#ifdef WIFI_DEBUG_INFO
				printf(" WIFI RTL8710 Connect AP ERROR!!\r\n");
				#endif
				return;
			}
			WIFI_RTL8710_Delay_ms(100);
		}
	}
	
	#else
wifi_connect:
	if(WIFI_RTL8710_SmartConfig_Setup_Connect() == WIFI_SUCCESS)
	{
		printf("WIFI_RTL8710_SmartConfig_Setup_Connect success\r\n");
	}
	else
	{
		printf("WIFI_RTL8710_SmartConfig_Setup_Connect filed\r\n");
		WIFI_RTL8710_Delay_ms(1000);
		WIFI_RTL8710_Delay_ms(1000);
		goto wifi_connect;
	}
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	WIFI_RTL8710_Delay_ms(1000);
	
	WIFI_RTL8710_Info(&wifi_info);
	
	#ifdef WIFI_DEBUG_INFO
	printf(" AP SSID : %s\r\n",wifi_info.ap_ssdi);
	printf(" AP password : %s\r\n",wifi_info.ap_password);
	printf(" AP MAC : %s\r\n",wifi_info.mac_addr);
	printf(" AP gatewayip : %s\r\n",wifi_info.gatewayip);
	printf(" AP ipaddr : %s\r\n",wifi_info.ipaddr);
	#endif
	
	#endif
	while(1);

//	WIFI_RTL8710_Check_NetWork_Status();

	WIFI_RTL8710_Ping("192.168.1.1");

	// 4.creater TCP client,single connection
	#if 0
	{
		uint8_t timerout = 0;
		while(WIFI_RTL8710_Set_TCPClient_Mode("192.168.1.109,60000") != WIFI_SUCCESS)
		{
			timerout++;
			if(timerout > 3)
			{
				#ifdef WIFI_DEBUG_INFO
				printf(" WIFI RTL8710 Connect AP ERROR!!\r\n");
				#endif
				return;
			}
			WIFI_RTL8710_Delay_ms(100);
		}
	}

	printf("==========================\r\n");

	// 2.enable transparent transmission mode
	WIFI_RTL8710_Enable_TransparentTransmission_mode();
	#endif
	//6.Enter transmission mode
//	WIFI_RTL8710_Enter_Transmission_mode();
	

//	while(1)
//	{
//		WIFI_RTL8710_Send_data("Hello World!!\r\n");
//		delay_ms(1000);
//		delay_ms(1000);
//	}
}

