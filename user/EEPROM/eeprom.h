#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "stm32f10x.h" 

#include "i2c_driver.h"

#define EE_DEV_ADDR			0xA0		/* 24xx04���豸��ַ */
#define EE_PAGE_SIZE		16			  /* 24xx04��ҳ���С */
#define EE_SIZE				512			  /* 24xx04������ */


uint8_t eeprom_CheckOk(i2c_device *dev);
uint8_t eeprom_ReadBytes(i2c_device *dev,
							uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t eeprom_WriteBytes(i2c_device *dev,
							uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
void eeprom_Erase(i2c_device *dev);
void eeprom_Test(void);



#endif
