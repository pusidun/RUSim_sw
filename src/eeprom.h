/*
 * eeprom.h
 *
 *  Created on: 2017Äê12ÔÂ19ÈÕ
 *      Author: dg
 */

#ifndef EEPROM_H_
#define EEPROM_H_


typedef u16 AddressType;

#define Write_Data(BaseAddr, LocalAddr, value)   (*(volatile u32 *)((BaseAddr + LocalAddr)) = (value))
#define Read_Data(BaseAddr, LocalAddr)           (*(volatile u32 *)(BaseAddr + LocalAddr))

#define BaseAddr                     0x44a20000
#define IIC_BASE_ADDRESS	         0x40800000

#define TEMP_SENSOR_ONCHIP_ADDRESS   0x4B //LM75 Device Address
#define EEPROM_ADDRESS	             0x50 //M24512 Device Address

#define LM75                         0x00
#define M24512                       0X01
#define I2CSelect                    0x30


//eeprom read ad9362 scripts
int eeRdAD(u16 eepromAddr);

//eeprom write ad9362 scripts
int eeWrAD(u16 eepromAddr, u8 type, u8 value, u16 addr);

float getLM75();
unsigned EepromWriteByte(AddressType Address, u8 *BufferPtr, u16 ByteCount);
unsigned EepromReadByte(AddressType Address, u8 *BufferPtr, u16 ByteCount);

#endif /* EEPROM_H_ */
