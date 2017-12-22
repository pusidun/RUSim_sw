/*
 * eeprom.cc
 *
 *  Created on: 2017Äê12ÔÂ19ÈÕ
 *      Author: dg
 */

#include "xparameters.h"
#include "xiic.h"
#include "xil_io.h"
#include "eeprom.h"
#include "ad9361_cfg.h"

#define  EEPROM_TEST_START_ADDRESS	0x0000  //The Starting address in the IIC EEPROM#define  PAGE_SIZE	                0x04  //M24512 MAX page size is 128 byte

int ErrorCount;
u8 EepromIicAddr;
u8 WriteBuffer[PAGE_SIZE];
u8 ReadBuffer[PAGE_SIZE];
u8 WriteLMBuffer[1] = { 0x00 };

/*
 * Read eeprom data to config roc
 */
int eeRdAD(u16 eepromAddr)
{
	EepromIicAddr = EEPROM_ADDRESS;
	unsigned BytesRead = EepromReadByte(eepromAddr, ReadBuffer, PAGE_SIZE);
	u8 type = ReadBuffer[0];
	int Status=0;
	if(type == 0xA)
	{
		u8 val = ReadBuffer[1];
		u16 addr = ReadBuffer[2];
		addr = addr << 8;
		addr = addr | ReadBuffer[3];
		SPIWrite(addr, val);
	}
	else if(type == 0xB)
	{
		u16 addr;
		addr = ReadBuffer[1];
		addr = (addr<<8)|ReadBuffer[2];
		SPIRead_HLevel(addr);
	}
	else if(type == 0xC)
	{
		u8 time;
		time = ReadBuffer[1];
		delay_ad9362(time*1000);
	}
	else if(type == 0xD)
	{
		u8 timetype;
		timetype = ReadBuffer[1];
		Status = WAIT_CALDONE(timetype);
		if(Status == XST_FAILURE)
			return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * write roc config data to eeprom
 */
int eeWrAD(u16 eepromAddr, u8 type, u8 value, u16 addr)
{
	EepromIicAddr = EEPROM_ADDRESS;
	if(type == 0xA)
	{
		WriteBuffer[0] = 0xA;
		WriteBuffer[1] = value;
		WriteBuffer[2] = addr >> 8;
		WriteBuffer[3] = addr & 0xff;
	}else if(type == 0xB)
	{
		WriteBuffer[0] = 0xB;
		WriteBuffer[1] = addr >> 8;
		WriteBuffer[2] = addr & 0xff;
		WriteBuffer[3] = 0x0;
	}else if(type == 0xC)
	{
		WriteBuffer[0] = 0xC;
		WriteBuffer[1] = value;
		WriteBuffer[2] = 0x0;
		WriteBuffer[3] = 0x0;
	} else if (type == 0xD) {
		WriteBuffer[0] = 0xD;
		WriteBuffer[1] = value;
		WriteBuffer[2] = 0x0;
		WriteBuffer[3] = 0x0;
	}
	unsigned bytesWrite = EepromWriteByte(eepromAddr, WriteBuffer, PAGE_SIZE);
	unsigned BytesRead = EepromReadByte(eepromAddr, ReadBuffer, PAGE_SIZE);
	for (int Index = 0; Index < 4; Index++) {
		if (ReadBuffer[Index] != WriteBuffer[Index]) {
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}

float getLM75() {
	u8 TemperaturePtr[3] = { 0x00 };
	float ActualTemperature;
	int Status, ByteCount;
	volatile unsigned ReceByteCount;

	Write_Data(BaseAddr, I2CSelect, LM75);//select I2C,default Select=0
	do {
		Status = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
		if (!(Status & XIIC_SR_BUS_BUSY_MASK)) {
			ReceByteCount = XIic_Send( IIC_BASE_ADDRESS,
			TEMP_SENSOR_ONCHIP_ADDRESS, WriteLMBuffer, 1, XIIC_STOP);
			if (ReceByteCount != 1) {
				// Send is aborted so reset Tx FIFO
				XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
						XIIC_CR_TX_FIFO_RESET_MASK);
				XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
						XIIC_CR_ENABLE_DEVICE_MASK);
			}
		}
	} while (ReceByteCount != 1);

	ByteCount = XIic_Recv( IIC_BASE_ADDRESS, TEMP_SENSOR_ONCHIP_ADDRESS,
			TemperaturePtr, 2,
			XIIC_STOP);

	TemperaturePtr[0] = TemperaturePtr[0] << 1;
	TemperaturePtr[1] = TemperaturePtr[1] >> 7;
	TemperaturePtr[2] = TemperaturePtr[0] + TemperaturePtr[1];
	ActualTemperature = TemperaturePtr[2] * 0.5;

	return ActualTemperature;
}

/*****************************************************************************
 * @param	Address contains the address in the EEPROM to write to.
 * @param	BufferPtr contains the address of the data to write.
 * @param	ByteCount contains the number of bytes in the buffer to be written.
 *		Note that this should not exceed the page size of the EEPROM as
 *		noted by the constant PAGE_SIZE.
 * @return	The number of bytes written, a value less than that which was
 *		specified as an input indicates an error.
 ****************************************************************************/
unsigned EepromWriteByte(AddressType Address, u8 *BufferPtr, u16 ByteCount) {
	volatile unsigned SentByteCount;
	volatile unsigned AckByteCount;
	u8 WriteBuffer[sizeof(Address) + PAGE_SIZE];
	int Index;

	/*
	 * A temporary write buffer must be used which contains both the address
	 * and the data to be written, put the address in first based upon the
	 * size of the address for the EEPROM.
	 */
	if (sizeof(AddressType) == 2) {
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
	} else if (sizeof(AddressType) == 1) {
		WriteBuffer[0] = (u8) (Address);
		EepromIicAddr |= (EEPROM_TEST_START_ADDRESS >> 8) & 0x7;
	}

	// Put the data in the write buffer following the address.
	for (Index = 0; Index < ByteCount; Index++) {
		WriteBuffer[sizeof(Address) + Index] = BufferPtr[Index];
	}

	// Write a page of data at the specified address to the EEPROM.
	SentByteCount = XIic_Send(IIC_BASE_ADDRESS, EepromIicAddr, WriteBuffer,
			sizeof(Address) + PAGE_SIZE,
			XIIC_STOP);

	// Return the number of bytes written to the EEPROM
	return SentByteCount - sizeof(Address);
}

/*****************************************************************************
 * This function reads a number of bytes from the IIC serial EEPROM into a
 * specified buffer.
 * @param	Address contains the address in the EEPROM to read from.
 * @param	BufferPtr contains the address of the data buffer to be filled.
 * @param	ByteCount contains the number of bytes in the buffer to be read.
 *		 This value is not constrained by the page size of the device
 *		    such that up to 64K may be read in one call.
 * @return	The number of bytes read. A value less than the specified input
 *		 value indicates an error.
 ******************************************************************************/
unsigned EepromReadByte(AddressType Address, u8 *BufferPtr, u16 ByteCount) {
	volatile unsigned ReceivedByteCount;
	u16 StatusReg;

	u8 WriteBuffer[sizeof(Address)];   //add by gongwei
	if (sizeof(AddressType) == 2) {
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
	}

	do {
		StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
		if (!(StatusReg & XIIC_SR_BUS_BUSY_MASK)) {
			ReceivedByteCount = XIic_Send( IIC_BASE_ADDRESS, EepromIicAddr,
					WriteBuffer, sizeof(Address), XIIC_STOP);
			if (ReceivedByteCount != sizeof(Address)) {
				// Send is aborted so reset Tx FIFO
				XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
						XIIC_CR_TX_FIFO_RESET_MASK);
				XIic_WriteReg(IIC_BASE_ADDRESS, XIIC_CR_REG_OFFSET,
						XIIC_CR_ENABLE_DEVICE_MASK);
			}
		}
	} while (ReceivedByteCount != sizeof(Address));

	// Read the number of bytes at the specified address from the EEPROM.
	ReceivedByteCount = XIic_Recv(IIC_BASE_ADDRESS, EepromIicAddr, BufferPtr,
			ByteCount, XIIC_STOP);
	// Return the number of bytes read from the EEPROM.
	return ReceivedByteCount;
}

