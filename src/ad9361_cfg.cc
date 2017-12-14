/*
 * ad9361_cfg.cc
 *
 *  Created on: 2017年12月12日
 *      Author: dg
 */

#include "ad9361_cfg.h"
#include "string.h"

u8 SPIRead(u16 addr)
{
	u32 spiVal=0x00;
	u32 regVal;
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态
	regVal = Read_Data(BaseAddr, 0x0024);
	if((regVal&0xC0) == 0xC0)
	{
		Write_Data(BaseAddr,0x002c,0x1);//chip select
		//写入地址到0x001c ,地址位宽是16bit;
		Write_Data(BaseAddr, 0x001c, addr);
		//配置 write 0x0024 bit 1:0 写成 0x01
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x01;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);
		delay_ad9362(1);
		Write_Data(BaseAddr, 0x0028, 0x00);
	}
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，可以读写
	regVal = Read_Data(BaseAddr, 0x0024);
	if((regVal&0xC0) == 0xC0)
	{
		//配置 write 0x0024 bit 1:0 写成 0x00
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x00;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);
		delay_ad9362(1);
		Write_Data(BaseAddr, 0x0028, 0x00);
		//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，表示读取完成
		regVal = Read_Data(BaseAddr, 0x0024);
		if ((regVal&0xC0) == 0xC0) {
			//从0x0020 这个寄存器读取数据
			Write_Data(BaseAddr,0x002c,0x0);//chip select
			spiVal = Read_Data(BaseAddr, 0x0020);
		}
	}
	return spiVal;
}

void SPIWrite(u16 addr, u8 val)
{
	u32 regVal;
	u32 tmp=0;
	addr |= 0x8000; //set bit 15 to 1,means writing
	regVal = Read_Data(BaseAddr, 0x0024);
	//read 0x0024 if bit7:6 is 0x11 , 表示SPI控制器是空闲状态，可以读写
	if((regVal&0xC0) == 0xC0)
	{
		delay_ad9362(1);
		Write_Data(BaseAddr,0x002c,0x1);//chip select
		//写入地址到0x001c ,地址位宽是16bit
		Write_Data(BaseAddr, 0x001c, addr);
		//配置 write 0x0024 bit 1:0 写成 0x01
		regVal = Read_Data(BaseAddr, 0x0024);
		regVal &= 0xFC;
		regVal |= 0x01;
		Write_Data(BaseAddr, 0x0024, regVal);
		//配置0x0028 , bit1 先写1 再写0
		Write_Data(BaseAddr, 0x0028, 0x01);delay_ad9362(1);
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
		Write_Data(BaseAddr, 0x0028, 0x01);delay_ad9362(1);
		Write_Data(BaseAddr, 0x0028, 0x00);
		if ((regVal&0xC0) == 0xC0) {
			//读取数据从0x0020 这个寄存器
			Write_Data(BaseAddr,0x002c,0x0);
		}
	}
}

void roc_script()
{

}


void delay_ad9362(u32 time)
{
	for(int i=0;i<time;i++)
		for(int j=0;j<20;j++);
}

int spi_reg_verity(u32 addr, u32 val)
{
	int regVal = Read_Data(BaseAddr, addr);
	if(regVal == val)
		return XST_SUCCESS;
	else
		return XST_FAILURE;
}

int spi_fpga_core_w(u32 addr, u32 val)
{
	Write_Data(BaseAddr, addr, val);
	return spi_reg_verity(addr, val);
}

int spi_fpga_core_r(u32 addr)
{
	int val = Read_Data(BaseAddr, addr);
	return val;
}

//read&write enable; 1:enable, 0:disable
int spi_rw_enable(u8 e)
{
	return spi_fpga_core_w(SPI_FPGA_CORE_WR, e);
}

//0x1 GSM , 0x2 LTE
int spi_ad_select(u32 chip)
{
	if(chip!=0x1 && chip!=0x2)
		return XST_FAILURE;
	return spi_fpga_core_w(SPI_distribution, chip);
}

