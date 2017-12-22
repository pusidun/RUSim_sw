/*
 * ad9361_cfg.cc
 *
 *  Created on: 2017年12月12日
 *      Author: dg
 */

#include "ad9361_cfg.h"
#include "string.h"
#include "stdio.h"
#include "eeprom.h"

u8 SPIRead(u16 addr) {
	u32 spiVal = 0x00;
	u32 regVal;
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态
	regVal = Read_Data(BaseAddr, 0x0024);
	if ((regVal & 0xC0) == 0xC0) {
		Write_Data(BaseAddr, 0x002c, chipSelect); //chip select
		//写入地址到0x001c ,地址位宽是16bit;
		Write_Data(BaseAddr, 0x001c, addr);
		//配置 write 0x0024 bit 1:0 写成 0x01
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x01;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);
		delay_ad9362(2);
		Write_Data(BaseAddr, 0x0028, 0x00);
	}
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，可以读写
	regVal = Read_Data(BaseAddr, 0x0024);
	if ((regVal & 0xC0) == 0xC0) {
		//配置 write 0x0024 bit 1:0 写成 0x00
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x00;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);
		delay_ad9362(2);
		Write_Data(BaseAddr, 0x0028, 0x00);
		//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，表示读取完成
		regVal = Read_Data(BaseAddr, 0x0024);
		if ((regVal & 0xC0) == 0xC0) {
			//从0x0020 这个寄存器读取数据
			Write_Data(BaseAddr, 0x002c, 0x0);		//chip select
			spiVal = Read_Data(BaseAddr, 0x0020);
		}
	}
	return spiVal;
}

void SPIWrite(u16 addr, u8 val) {
	u32 regVal;
	addr |= 0x8000; //set bit 15 to 1,means writing
	regVal = Read_Data(BaseAddr, 0x0024);
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，可以读写
	if ((regVal & 0xC0) == 0xC0) {
		delay_ad9362(2);
		Write_Data(BaseAddr, 0x002c, chipSelect); //chip select
		//写入地址到0x001c ,地址位宽是16bit
		Write_Data(BaseAddr, 0x001c, addr);
		//配置 write 0x0024 bit 1:0 写成 0x01
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x01;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);
		delay_ad9362(2);
		Write_Data(BaseAddr, 0x0028, 0x00);
	}
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，可以读写
	regVal = Read_Data(BaseAddr, 0x0024);
	if ((regVal & 0xC0) == 0xC0) {
		//写入数据到0x001c ,数据位宽是8bit
		Write_Data(BaseAddr, 0x001c, val);
		//配置 write 0x0024 bit 1:0 写成 0x00
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x00;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);
		delay_ad9362(2);
		Write_Data(BaseAddr, 0x0028, 0x00);
		if ((regVal & 0xC0) == 0xC0) {
			//读取数据从0x0020 这个寄存器
			Write_Data(BaseAddr, 0x002c, 0x0);
		}
	}
}

/*
 * @param: u32 time: 微秒
 */
void delay_ad9362(u32 time) {
	for (u32 i = 0; i < time; i++)
		for (u32 j = 0; j < 10; j++)
			;
}

int spi_reg_verity(u32 addr, u32 val) {
	u32 regVal = Read_Data(BaseAddr, addr);
	if (regVal == val)
		return XST_SUCCESS;
	else
		return XST_FAILURE;
}

void SPIRead_HLevel(u16 addr) {
	u8 regVal = 0x00;
	if (addr == 0x05e) {
		// Check BBPLL locked status  (0x05E[7]==1 is locked)
		regVal = SPIRead(addr);
		regVal = regVal >> 0x7;
		if ((regVal & 0x1) != 0x1)
			printf("BBPLL is not locked\n");
	} else if (addr == 0x247) {
		// Check RX RF PLL lock status (0x247[1]==1 is locked)
		regVal = SPIRead(addr);
		if ((regVal & 0x1) != 0x1)
			printf("RX RF PLL is not locked\n");
	} else if (addr == 0x287) {
		// Check TX RF PLL lock status (0x287[1]==1 is locked)
		regVal = SPIRead(addr);
		if ((regVal & 0x1) != 1)
			printf("Tx RF PLL is not locked\n");
	} else {
		//do nothing
	}
}

int WAIT_CALDONE(u16 index) {
	if (index == 1) {
		// WAIT_CALDONE	BBPLL,2000
		// Wait for BBPLL to lock, Timeout 2sec, Max BBPLL VCO Cal Time: 225.000 us (Done when 0x05E[7]==1)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x05E);//0x80
			u8 checkbit = reg & (0x1 << 7);
			if (checkbit == (0x1 << 7)) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	BBPLL,2000 fail.");
		configStatus = XST_FAILURE;
	} else if (index == 2){		//fail 40 8`b0010 1000
		//WAIT_CALDONE	RXCP,100
		// Wait for CP cal to complete, Max RXCP Cal time: 600.000 (us)(Done when 0x244[7]==1)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x244);//0xa9
			u8 checkbit = reg & (0x1 << 7);
			if (checkbit == (0x1 << 7)) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	RXCP,100 fail.");
		configStatus = XST_FAILURE;
	} else if (index == 3){		//fail  40 104 8`b0110 1000
		//WAIT_CALDONE	TXCP,100
		// Wait for CP cal to complete, Max TXCP Cal time: 600.000 (us)(Done when 0x284[7]==1)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x284);//0xa7 0xa8
			u8 checkbit = reg & (0x1 << 7);
			if (checkbit == (0x1 << 7)) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	TXCP,100 fail.");
		//configStatus = XST_FAILURE;
	} else if (index == 4) {
		//WAIT_CALDONE	RXFILTER,2000
		// Wait for RX filter to tune, Max Cal Time: 5.585 us (Done when 0x016[7]==0)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x016);//0x0
			u8 checkbit = reg & (0x1 << 7);
			if (checkbit == 0) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	RXFILTER,2000 fail.");
		configStatus = XST_FAILURE;
	} else if (index == 5) {
		//WAIT_CALDONE	TXFILTER,2000
		// Wait for TX filter to tune, Max Cal Time: 2.889 us (Done when 0x016[6]==0)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x016);//0x0
			u8 checkbit = reg & (0x1 << 6);
			if (checkbit == 0) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	TXFILTER,2000 fail.");
		configStatus = XST_FAILURE;
	} else if (index == 6) {
		//WAIT_CALDONE	BBDC,2000
		// BBDC Max Cal Time: 6575.521 us. Cal done when 0x016[0]==0
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x016);//0x0
			u8 checkbit = reg & (0x1 << 0);
			if (checkbit == 0) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	BBDC,2000 fail.");
		configStatus = XST_FAILURE;
	} else if (index == 7){		//fail 2
		//WAIT_CALDONE	RFDC,2000
		// Wait for cal to complete (Done when 0x016[1]==0)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x016);//0x0
			u8 checkbit = reg & (0x1 << 1);
			if (checkbit == 0) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else{
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	RFDC,2000 fail.");
		//configStatus = XST_FAILURE;
	} else if (index == 8) {
		//WAIT_CALDONE	TXQUAD,2000
		// Wait for cal to complete (Done when 0x016[4]==0)
		for (int i = 0; i < DELAY_TRY_COUNT; i++) {
			u32 reg = SPIRead(0x016);//0x0
			u8 checkbit = reg & (0x1 << 4);
			if (checkbit == 0) {
				waitcalStatus = XST_SUCCESS;
				return waitcalStatus;
			}
			else {
				waitcalStatus = XST_FAILURE;
				delay_ad9362(2000000 / DELAY_TRY_COUNT);
			}
		}
		printf("WAIT_CALDONE	TXFILTER,2000 fail.");
		configStatus = XST_FAILURE;
	} else {
		printf("for extention");
	}
}

int writeScriptEeprom() {
	int Status = 1;
	Write_Data(BaseAddr, I2CSelect, M24512);

	//insert LTE_eeprom_WR.txt and GSM_eeprom_WR.txt below





	//:end

	return Status;
}

void readScriptEeprom() {
	Write_Data(BaseAddr, I2CSelect, M24512);
	chipSelect = 0x02;

	//初始化FPGA SPI控制器， Write 0x0024  0x0
	Write_Data(BaseAddr, 0x0024, 0xc);
	for (int i = 0x00; i <= 0x16c0; i += 0x4) {
		eeRdAD(i);
	}

	chipSelect = 0x01;
	for (int i = 0x16c4; i <= 0x3984; i += 0x4) {
		eeRdAD(i);
	}
}
/*
 int ConfigAD9361LTE() {
 chipSelect = 0x02;

 //初始化FPGA SPI控制器， Write 0x0024  0x0
 Write_Data(BaseAddr, 0x0024, 0xc);

 //Insert Covert_LTE_AD936X_40M.txt Scripts below.


 //:end Scripts

 return configStatus;
 }

 int ConfigAD9361GSM() {
 chipSelect = 0x01;

 //初始化FPGA SPI控制器， Write 0x0024  0x0
 Write_Data(BaseAddr, 0x0024, 0xc);

 //Insert Covert_GSM_AD80305.txt Scripts below.


 //:end Scripts

 return configStatus;
 }
 */
