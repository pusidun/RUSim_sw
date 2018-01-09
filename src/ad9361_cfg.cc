/*
 * ad9361_cfg.cc
 *
 *  Created on: 2017年12月12日
 *      Author: dg
 */

#include "ad9361_cfg.h"
#include "eeprom.h"
#include "math.h"

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
	//delay 1ms
	delay_ad9362(2000);
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
		if ((regVal & 0x1) != 0x1);
			//printf("BBPLL is not locked\n");
	} else if (addr == 0x247) {
		// Check RX RF PLL lock status (0x247[1]==1 is locked)
		regVal = SPIRead(addr);
		if ((regVal & 0x1) != 0x1);
			//printf("RX RF PLL is not locked\n");
	} else if (addr == 0x287) {
		// Check TX RF PLL lock status (0x287[1]==1 is locked)
		regVal = SPIRead(addr);
		if ((regVal & 0x1) != 1);
			//printf("Tx RF PLL is not locked\n");
	} else if(addr == 0x1e6){
		u8 reg1EB=0, reg1EC=0, reg1E6=0;
		reg1EB = SPIRead(0x1EB);
		reg1EC = SPIRead(0x1EC);
		reg1E6 = SPIRead(0x1E6);
		if(RXTIA_ADC == 0)//CONFIG_RXTIA
		{
			RXTIA_ADC = 1;

			// calculation
			u16 Cbbf = (reg1EB * 160)  + (reg1EC * 10) + 140;
			u16 R2346 = 18300 * (reg1E6 & 0x7);
			double CTIA_fF = (Cbbf * R2346 * 0.56) / 3500;
			u16 tmpBBBW_MHz = 20;

			u8 reg1DB=0;
			u16 U16_BW_MHz = (tmpBBBW_MHz);
			if( U16_BW_MHz <= 3 )
			{
				reg1DB = 0xE0;
			}
			else if( U16_BW_MHz > 3 && U16_BW_MHz <= 10 )
			{
			   reg1DB = 0x60;
			}
			else
			{
			   reg1DB = 0x20;
			}

			u8 temp = 0;
			u8 reg1DC = 0, reg1DE = 0, reg1DD = 0, reg1DF = 0;
			if( CTIA_fF > 2920.0 )
			{
				reg1DC = 0x40;
				reg1DE = 0x40;
				temp = (u8)(CTIA_fF - 400) / 320 ;
				temp = temp < 127 ? temp : 127;
				reg1DD = temp;
				reg1DF = temp;
			}
			else
			{
				temp = (u8) (CTIA_fF - 400) / 40  + 0x40;
				reg1DC = temp;
				reg1DE = temp;
				reg1DD = 0;
				reg1DF = 0;
			}

			// Update data of corresponding registers and they will be
			// written to AD9362 registers
			if(chipSelect == 0x2)//lte
			{
				eeWrAD(0x1518, 0xA, reg1DB, 0x1DB);//0x20
				eeWrAD(0x151c, 0xA, reg1DD, 0x1DD);//0
				eeWrAD(0x1520, 0xA, reg1DF, 0x1DF);//0
				eeWrAD(0x1524, 0xA, reg1DC, 0x1DC);//0x46
				eeWrAD(0x1528, 0xA, reg1DE, 0x1DE);//0x46
			}
			else if(chipSelect == 0x1)//gsm
			{
				eeWrAD(0x37dc, 0xA, reg1DB, 0x1DB);
				eeWrAD(0x37e0, 0xA, reg1DD, 0x1DD);
				eeWrAD(0x37e4, 0xA, reg1DF, 0x1DF);
				eeWrAD(0x37e8, 0xA, reg1DC, 0x1DC);
				eeWrAD(0x37ec, 0xA, reg1DE, 0x1DE);
			}
		}
		else if(RXTIA_ADC == 1)//CONFIG_ADC
		{
			RXTIA_ADC =0;
			double tmpBBBW_MHz = 0;
			if(chipSelect == 0x2)//lte
			{
				tmpBBBW_MHz = 40;
			}
			else if(chipSelect == 0x1)//gsm
			{
				tmpBBBW_MHz = 20;
			}

			//calculation
			double scale_snr_dB = (FsADC < 80 ? 0 : 2);
			u8 rxbbf_c3_msb = reg1EB, rxbbf_c3_lsb = reg1EC, rxbbf_r2346 = reg1E6;
			double d1 = (1.4 * 2 * 3.14);
			double d2 = (18300 * rxbbf_r2346);
			double d3 = 160e-15 * rxbbf_c3_msb + 10e-15 * rxbbf_c3_lsb + 140e-15;
			double d4 = tmpBBBW_MHz * 1.0e6;
			double d5 = 1.0;
			if( tmpBBBW_MHz > 18.0 )
			{
				d5 = 1.0 + 0.01 * (tmpBBBW_MHz - 18.0);
			}
			double rc_timeConst = 1.0 / ( d1 * d2 * d3 * d4 * d5 );

			double scale_res = sqrt( 1 / rc_timeConst);
			double scale_cap = scale_res;
			double scale_snr = pow(10, scale_snr_dB * 0.1 );
			double maxsnr = 640.0 / 160;

			u8 reg207,reg208,reg209,reg20a,reg20b,reg20c,reg20d,reg20e;
			u8 reg20f,reg210,reg211,reg212,reg213,reg214,reg215,reg216,reg217;
			u8 reg218,reg219,reg21a,reg21b,reg21c,reg21d,reg21e,reg21f,reg220,reg221,reg222;

			// calculate the value of 0x207
			double regVal = 0.0;
			double minVal = sqrt( maxsnr * FsADC / 640 );
			minVal = minVal < 1.0 ? minVal : 1.0;
			regVal = floor(-0.5 + 80 * scale_snr * scale_res * minVal);
			regVal = regVal < 124.0 ? regVal : 124.0;
			reg207 = regVal;

			// calculate the value of 0x208
			regVal = floor( 0.5 + (20 * 640 / FsADC) * (reg207 / 80) / (scale_res * scale_cap) );
			regVal = regVal < 255.0 ? regVal : 255.0;
			reg208 = regVal;

			// calculate the value of 0x20A
			regVal = floor(-0.5 + 77 * scale_res * minVal);
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg20a = regVal;

			// calculate the value of 0x209
			regVal = floor( 0.8 * reg20a);
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg209 = regVal;


			// calculate the value of 0x20B
			regVal = floor( 0.5 + 20 * (640 / FsADC) );
			regVal = regVal < 255.0 ? regVal : 255.0;
			reg20b = regVal;

			// calculate the value of 0x20C
			regVal = floor( -0.5 + 80 * scale_res * minVal );
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg20c = regVal;

			// calculate the value of 0x20D
			regVal = floor( -1.5 + 20 * (640 / FsADC) * (regVal / 80) / (scale_res * scale_cap) );
			regVal = regVal < 255.0 ? regVal : 255.0;
			reg20d = regVal;

			// calculate the value of 0x20E
			regVal = 21 * floor( 0.1 * 640 / FsADC );
			reg20e = regVal;

			// calculate the value of 0x20F
			regVal = (u8)(1.025 * reg207);
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg20f = regVal;

			// calculate the value of 0x210
			double maxVal = (640 / FsADC) / maxsnr;
			maxVal = maxVal < 1.0 ? 1.0 : maxVal;
			regVal = floor( reg20f * (0.98 + 0.02 * maxVal) );
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg210 = regVal;

			// calculate the value of 0x211
			reg211 = reg20f;

			// calculate the value of 0x212
			regVal = (u8)( 0.975 * reg20a );
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg212 = regVal;

			// calculate the value of 0x213
			regVal = floor( reg212  * (0.98 + 0.02 * maxVal) );
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg213 = regVal;

			// calculate the value of 0x214
			reg214 = reg212;

			// calculate the value of 0x215
			regVal = (u8)(0.975 * reg20c );
			reg215 = regVal < 127.0 ? regVal : 127.0;

			// calculate the value of 0x216
			regVal = floor( reg215  * (0.98 + 0.02 * maxVal) );
			reg216 = regVal < 127.0 ? regVal : 127.0;

			// calculate the value of 0x217
			reg217 = reg216;

			// calculate the value of 0x218
			reg218 = 0x2E;

			// calculate the value of 0x219
			minVal = 63 * (FsADC / 640 );
			minVal = minVal < 63 ? minVal : 63;
			regVal = floor(128 + minVal);
			reg219 = regVal;

			// calculate the value of 0x21A
			regVal = floor(0 + minVal * (0.92 + 0.08 * (640 / FsADC)));
			reg21a = regVal;

			// calculate the value of 0x21B
			minVal = 32 * sqrt( FsADC / 640 );
			minVal = minVal < 63 ? minVal : 63;
			regVal = floor(0 + minVal);
			reg21b = regVal;

			// calculate the value of 0x21C
			reg21c =reg219;

			// calculate the value of 0x21D
			reg21d =reg21a;

			// calculate the value of 0x21E
			reg21e = reg21b;

			// calculate the value of 0x21F
			reg21f = reg219;

			// calculate the value of 0x220
			reg220 = reg21a;

			// calculate the value of 0x221
			minVal = 63 * sqrt( FsADC / 640 );
			minVal = minVal < 63 ? minVal : 63;
			regVal = floor( 0 + minVal );
			reg221 = regVal;

			// calculate the value of 0x222
			regVal = floor( 64 * sqrt( FsADC / 640) );
			regVal = regVal < 127.0 ? regVal : 127.0;
			reg222 = regVal;


			if(chipSelect == 0x2)//lte
			{
				int Status;
				Status=eeWrAD(0x1560, 0xA, reg207, 0x207);
				Status=eeWrAD(0x1564, 0xA, reg208, 0x208);
				Status=eeWrAD(0x1568, 0xA, reg209, 0x209);
				Status=eeWrAD(0x156c, 0xA, reg20a, 0x20A);
				Status=eeWrAD(0x1570, 0xA, reg20b, 0x20B);
				Status=eeWrAD(0x1574, 0xA, reg20c, 0x20C);
				Status=eeWrAD(0x1578, 0xA, reg20d, 0x20D);//0xf
				Status=eeWrAD(0x157c, 0xA, reg20e, 0x20E);//0
				Status=eeWrAD(0x1580, 0xA, reg20f, 0x20F);//0x7f
				Status=eeWrAD(0x1584, 0xA, reg210, 0x210);
				Status=eeWrAD(0x1588, 0xA, reg211, 0x211);
				Status=eeWrAD(0x158c, 0xA, reg212, 0x212);
				Status=eeWrAD(0x1590, 0xA, reg213, 0x213);
				Status=eeWrAD(0x1594, 0xA, reg214, 0x214);
				Status=eeWrAD(0x1598, 0xA, reg215, 0x215);//0x78
				Status=eeWrAD(0x159c, 0xA, reg216, 0x216);//0x78
				Status=eeWrAD(0x15a0, 0xA, reg217, 0x217);//0x78
				Status=eeWrAD(0x15a4, 0xA, reg218, 0x218);
				Status=eeWrAD(0x15a8, 0xA, reg219, 0x219);
				Status=eeWrAD(0x15ac, 0xA, reg21a, 0x21A);
				Status=eeWrAD(0x15b0, 0xA, reg21b, 0x21B);
				Status=eeWrAD(0x15b4, 0xA, reg21c, 0x21C);
				Status=eeWrAD(0x15b8, 0xA, reg21d, 0x21D);
				Status=eeWrAD(0x15bc, 0xA, reg21e, 0x21E);
				Status=eeWrAD(0x15c0, 0xA, reg21f, 0x21F);
				Status=eeWrAD(0x15c4, 0xA, reg220, 0x220);//0x31
				Status=eeWrAD(0x15c8, 0xA, reg221, 0x221);
				Status=eeWrAD(0x15cc, 0xA, reg222, 0x222);
			}
			else if(chipSelect == 0x1)//gsm
			{
				int Status;
				Status=eeWrAD(0x3824, 0xA, reg207, 0x207);
				Status=eeWrAD(0x3828, 0xA, reg208, 0x208);
				Status=eeWrAD(0x382c, 0xA, reg209, 0x209);
				Status=eeWrAD(0x3830, 0xA, reg20a, 0x20A);
				Status=eeWrAD(0x3834, 0xA, reg20b, 0x20B);
				Status=eeWrAD(0x3838, 0xA, reg20c, 0x20C);
				Status=eeWrAD(0x383c, 0xA, reg20d, 0x20D);
				Status=eeWrAD(0x3840, 0xA, reg20e, 0x20E);
				Status=eeWrAD(0x3844, 0xA, reg20f, 0x20F);
				Status=eeWrAD(0x3848, 0xA, reg210, 0x210);
				Status=eeWrAD(0x384c, 0xA, reg211, 0x211);
				Status=eeWrAD(0x3850, 0xA, reg212, 0x212);
				Status=eeWrAD(0x3854, 0xA, reg213, 0x213);
				Status=eeWrAD(0x3858, 0xA, reg214, 0x214);
				Status=eeWrAD(0x385c, 0xA, reg215, 0x215);
				Status=eeWrAD(0x3860, 0xA, reg216, 0x216);
				Status=eeWrAD(0x3864, 0xA, reg217, 0x217);
				Status=eeWrAD(0x3868, 0xA, reg218, 0x218);
				Status=eeWrAD(0x386c, 0xA, reg219, 0x219);
				Status=eeWrAD(0x3870, 0xA, reg21a, 0x21A);
				Status=eeWrAD(0x3874, 0xA, reg21b, 0x21B);
				Status=eeWrAD(0x3878, 0xA, reg21c, 0x21C);
				Status=eeWrAD(0x387c, 0xA, reg21d, 0x21D);
				Status=eeWrAD(0x3880, 0xA, reg21e, 0x21E);
				Status=eeWrAD(0x3884, 0xA, reg21f, 0x21F);
				Status=eeWrAD(0x3888, 0xA, reg220, 0x220);
				Status=eeWrAD(0x388c, 0xA, reg221, 0x221);
				Status=eeWrAD(0x3890, 0xA, reg222, 0x222);
			}
		}

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
		//printf("WAIT_CALDONE	BBPLL,2000 fail.");
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
		//printf("WAIT_CALDONE	RXCP,100 fail.");
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
		//printf("WAIT_CALDONE	TXCP,100 fail.");
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
		//printf("WAIT_CALDONE	RXFILTER,2000 fail.");
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
		//printf("WAIT_CALDONE	TXFILTER,2000 fail.");
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
		//printf("WAIT_CALDONE	BBDC,2000 fail.");
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
		//printf("WAIT_CALDONE	RFDC,2000 fail.");
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
		//printf("WAIT_CALDONE	TXFILTER,2000 fail.");
		configStatus = XST_FAILURE;
	} else {
		//printf("for extention");
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
	//reset ad9362
	reset_ad9362();

	for (int i = 0x00; i <= 0x16c0; i += 0x4) {
		eeRdAD(i);
	}

	chipSelect = 0x01;
	for (int i = 0x16c4; i <= 0x3984; i += 0x4) {
		eeRdAD(i);
	}
}

void reset_ad9362()
{
	Write_Data(BaseAddr, 0x34, 0x0);
	Write_Data(BaseAddr, 0x34, 0x3);
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
