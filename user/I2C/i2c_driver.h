#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_

 
#include <inttypes.h>
#include "board_config.h"
#include "SysTick.h"

#ifdef DEVICE_I2C


#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */

//����һ���˿ڵĽṹ����
struct Sof_i2c_Init
{
	uint32_t SDA_Periph_Clock;	//SDA RCCʱ��
	uint32_t SCL_Periph_CLOCK;	//SCL RCCʱ��
	GPIO_TypeDef* SDA_GPIOx;		//GPIO��,����GPIOB 
	GPIO_TypeDef* SCL_GPIOx;		//GPIO��,����GPIOB 
};

//������ I2c�Ĳ���������
struct sof_i2c_fops
{
	void (*gpio_set)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);	//����GPIO�����
	void (*gpio_reset)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);	//����GPIO�����
	uint8_t (*gpio_read_bit)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//��ȡGPIO��ƽ
	void (*delay_ms)(uint16_t nms);	//������ʱ
	void (*delay_us)(uint32_t nus);	//΢����ʱ
};

typedef struct __arg_Sof_i2c_TypeDef
{
	struct Sof_i2c_Init port;	//�����˿ڵĽṹ����
	uint32_t sda;	//������
	uint32_t scl;	//ʱ����
	uint32_t timeout;	//��ʱʱ��
	struct sof_i2c_fops fops;	//����������
}Sof_i2c_TypeDef;


typedef struct __arg_i2c_device{
	Sof_i2c_TypeDef* sof_i2c;
	uint8_t slave_addr;
}i2c_device;


extern Sof_i2c_TypeDef sof_i2c1;


void I2C_GPIOInitConfig(Sof_i2c_TypeDef* Sof_i2c_inode);
void I2C_Start(Sof_i2c_TypeDef* Sof_i2c_inode);
void I2C_Stop(Sof_i2c_TypeDef* Sof_i2c_inode);
void I2C_SendACK(Sof_i2c_TypeDef* Sof_i2c_inode,uint8_t ack);
uint8_t I2C_RecvACK(Sof_i2c_TypeDef* Sof_i2c_inode);
void I2C_SendByte(Sof_i2c_TypeDef* Sof_i2c_inode,uint8_t dat);
uint8_t I2C_RecvByte(Sof_i2c_TypeDef* Sof_i2c_inode);
uint8_t I2C_CheckDevice(Sof_i2c_TypeDef* Sof_i2c_inode,uint8_t _Address);
void Sof_I2C_Init(void);


uint8_t I2C_dev_ReadBytes(i2c_device* dev,
							uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t I2C_dev_WriteBytes(i2c_device* dev,
							uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

#endif
