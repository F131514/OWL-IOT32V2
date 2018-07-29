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
#include "i2c_driver.h"



#ifdef DEVICE_I2C


Sof_i2c_TypeDef sof_i2c1 = {
	.port = {
		.SDA_Periph_Clock = RCC_APB2Periph_GPIOB,
		.SCL_Periph_CLOCK = RCC_APB2Periph_GPIOB,
		.SDA_GPIOx = GPIOB,
		.SCL_GPIOx = GPIOB,
	},
	.sda = GPIO_Pin_7,
	.scl = GPIO_Pin_6,
	.fops = {
		.gpio_set = GPIO_SetBits,
		.gpio_reset = GPIO_ResetBits,
		.gpio_read_bit = GPIO_ReadInputDataBit,
	},
};



void I2C_GPIOInitConfig(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(Sof_i2c_inode->port.SDA_Periph_Clock,ENABLE); 
	RCC_APB2PeriphClockCmd(Sof_i2c_inode->port.SCL_Periph_CLOCK,ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = Sof_i2c_inode->scl;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(Sof_i2c_inode->port.SCL_GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Sof_i2c_inode->sda;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(Sof_i2c_inode->port.SDA_GPIOx, &GPIO_InitStructure);

	GPIO_SetBits(Sof_i2c_inode->port.SCL_GPIOx, Sof_i2c_inode->scl);
	GPIO_SetBits(Sof_i2c_inode->port.SDA_GPIOx, Sof_i2c_inode->sda);
}

static void SDA_OUT(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = Sof_i2c_inode->sda;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(Sof_i2c_inode->port.SDA_GPIOx, &GPIO_InitStructure);

	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
}

static void SDA_IN(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = Sof_i2c_inode->sda;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(Sof_i2c_inode->port.SCL_GPIOx, &GPIO_InitStructure);

	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
}
/**************************************
**************************************/
void I2C_Start(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	SDA_OUT(Sof_i2c_inode);
	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
    delay_us(8);
	Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
    delay_us(8);
	Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
}

/**************************************
**************************************/
void I2C_Stop(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	SDA_OUT(Sof_i2c_inode);
	Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
	delay_us(8);
    Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
    delay_us(8);
}

/**************************************
**************************************/
void I2C_SendACK(Sof_i2c_TypeDef* Sof_i2c_inode,uint8_t ack)
{
	SDA_OUT(Sof_i2c_inode);
	Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
	delay_us(8);
	if(ack)
		Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
	else Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
	delay_us(8);
	Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
	delay_us(8);
}

/**************************************
**************************************/
uint8_t I2C_RecvACK(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	u8 ucErrTime=0;
	uint8_t value = 0;

 	SDA_IN(Sof_i2c_inode);
	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
	delay_us(4);	   
	Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
	delay_us(4);
	
	value = Sof_i2c_inode->fops.gpio_read_bit(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
	while(value)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop(Sof_i2c_inode);
			return 1;
		}
	}
	Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
	return 0;  
}

/**************************************
**************************************/
void I2C_SendByte(Sof_i2c_TypeDef* Sof_i2c_inode,uint8_t dat)
{
	uint8_t t;
	
	SDA_OUT(Sof_i2c_inode); 	    
    Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
    for(t=0;t<8;t++)
    {
		if(dat&0x80)
			Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
		else Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
        dat<<=1; 	  
		delay_us(5);
		Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
		delay_us(5); 
		Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
		delay_us(5);
    }
}

/**************************************
**************************************/	

uint8_t I2C_RecvByte(Sof_i2c_TypeDef* Sof_i2c_inode)
{
	int i = 0;
	uint8_t byte = 0;
	uint8_t value = 0;
	
	SDA_IN(Sof_i2c_inode);
	for(i = 0;i < 8;i++)
	{
		delay_us(5);
		Sof_i2c_inode->fops.gpio_set(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
		delay_us(5);
		byte <<= 1;
		value = Sof_i2c_inode->fops.gpio_read_bit(Sof_i2c_inode->port.SDA_GPIOx,Sof_i2c_inode->sda);
		if(value)
		{
			byte |= 0x01;
		}
		Sof_i2c_inode->fops.gpio_reset(Sof_i2c_inode->port.SCL_GPIOx,Sof_i2c_inode->scl);
		delay_us(5);
	}
	return byte;
}


/****************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
****************************************************************************/
uint8_t I2C_CheckDevice(Sof_i2c_TypeDef* Sof_i2c_inode,uint8_t _Address)
{
	uint8_t ucAck;

	I2C_Start(Sof_i2c_inode);		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	I2C_SendByte(Sof_i2c_inode,_Address | I2C_WR);
	ucAck = I2C_RecvACK(Sof_i2c_inode);	/* ����豸��ACKӦ�� */

	I2C_Stop(Sof_i2c_inode);			/* ����ֹͣ�ź� */

	return ucAck;
}



void Sof_I2C_Init(void)
{
	I2C_GPIOInitConfig(&sof_i2c1);
}

/******************************************************************
*	�� �� ��: eeprom_ReadBytes
*	����˵��: �Ӵ���EEPROMָ����ַ����ʼ��ȡ��������
*	��    �Σ�dev-------:I2C�����ϵ��豸
*			 _usAddress : ��ʼ��ַ
*			 _usSize : ���ݳ��ȣ���λΪ�ֽ�
*			 _pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
********************************************************************/
uint8_t I2C_dev_ReadBytes(i2c_device* dev,
							uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;
	
	I2C_Start(dev->sof_i2c);
	I2C_SendByte(dev->sof_i2c,dev->slave_addr|I2C_WR);
	
	/* ��3��������ACK */
	if (I2C_RecvACK(dev->sof_i2c) != 0)
	{
		goto cmd_fail;	
	}

	/* ��4���������ֽڵ�ַ */
	I2C_SendByte(dev->sof_i2c,(uint8_t)_usAddress&0xff);
	
	/* ��5��������ACK */
	if (I2C_RecvACK(dev->sof_i2c) != 0)
	{
		goto cmd_fail;	/* ������Ӧ�� */
	}
	
	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	I2C_Start(dev->sof_i2c);
	
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	I2C_SendByte(dev->sof_i2c,dev->slave_addr|I2C_RD);
	/* ��8��������ACK */
	if (I2C_RecvACK(dev->sof_i2c) != 0)
	{
		goto cmd_fail;	/* ������Ӧ�� */
	}	
	
	/* ��9����ѭ����ȡ���� */
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = I2C_RecvByte(dev->sof_i2c);	/* ��1���ֽ� */
		
		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != _usSize - 1)
		{
			I2C_SendACK(dev->sof_i2c,0);	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			I2C_SendACK(dev->sof_i2c,1);	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop(dev->sof_i2c);
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop(dev->sof_i2c);
//	printf("Read fild!\r\n");
	return 0;
}


/**********************************************************************************
*	�� �� ��: eeprom_WriteBytes
*	����˵��: ����EEPROMָ����ַд���������ݣ�����ҳд�������д��Ч��
*	��    �Σ�dev-------:I2C�����ϵ��豸
*			 _usAddress : ��ʼ��ַ
*			 _usSize : ���ݳ��ȣ���λΪ�ֽ�
*			 _pWriteBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
**********************************************************************************/
uint8_t I2C_dev_WriteBytes(i2c_device* dev,
							uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;
	

	usAddr = _usAddress;
	for (i = 0; i < _usSize; i++)
	{
		/* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
		if (i == 0)
		{
			/*���ڣ�������ֹͣ�źţ������ڲ�д������*/
			I2C_Stop(dev->sof_i2c);
			
			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms 			
				CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
			*/
			for (m = 0; m < 100; m++)
			{				
				/* ��1��������I2C���������ź� */
				I2C_Start(dev->sof_i2c);
				
				/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
				I2C_SendByte(dev->sof_i2c,dev->slave_addr|I2C_WR);
				
				/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
				if (I2C_RecvACK(dev->sof_i2c) == 0)
				{
					break;
				}
			}
			/* ��4���������ֽڵ�ַ*/
			
			I2C_SendByte(dev->sof_i2c,(uint8_t)usAddr%256);
			
			/* ��5��������ACK */
			if (I2C_RecvACK(dev->sof_i2c) != 0)
			{
				goto cmd_fail;	/* ������Ӧ�� */
			}
		}
	
		/* ��6������ʼд������ */
		I2C_SendByte(dev->sof_i2c,_pWriteBuf[i]);
	
		/* ��7��������ACK */
		if (I2C_RecvACK(dev->sof_i2c) != 0)
		{
			goto cmd_fail;	/* ������Ӧ�� */
		}

		usAddr++;	/* ��ַ��1 */		
	}
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	I2C_Stop(dev->sof_i2c);
	return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop(dev->sof_i2c);
//	printf("Write fild!\r\n");
	return 0;
}



#endif
