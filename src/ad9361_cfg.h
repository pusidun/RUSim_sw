/*
 * ad9361_cfg.h
 *
 *  Created on: 2017Äê12ÔÂ12ÈÕ
 *      Author: dg
 */

#ifndef AD9361_CFG_H_
#define AD9361_CFG_H_

/***************************** Include Files *********************************/

#include "xbasic_types.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/

#define AD9362_WRITE 0x1c
#define AD9362_READ 0x20
#define SPI_FPGA_CORE_WR 0x28
#define SPI_distribution 0x2c

#define Write_Data(BaseAddr, LocalAddr, value)   (*(volatile u32 *)((BaseAddr + LocalAddr)) = (value))
#define Read_Data(BaseAddr, LocalAddr)           (*(volatile u32 *)(BaseAddr + LocalAddr))
#define BaseAddr  0x44a20000

#define DELAY_TRY_COUNT 5
#define FsADC 491.520
extern u32 chipSelect ;

/************************** Function Prototypes ******************************/
int writeScriptEeprom();

void readScriptEeprom();

int ConfigAD9361LTE();

int ConfigAD9361GSM();

void SPIRead_HLevel(u16 addr);

/*
 * Index:      Command

    1)      	WAIT_CALDONE	    BBPLL,2000	// Wait for BBPLL to lock, Timeout 2sec, Max BBPLL VCO Cal Time: 225.000 us (Done when 0x05E[7]==1)
	2)			WAIT_CALDONE	RXCP,100	// Wait for CP cal to complete, Max RXCP Cal time: 600.000 (us)(Done when 0x244[7]==1)
	3)			WAIT_CALDONE	TXCP,100	// Wait for CP cal to complete, Max TXCP Cal time: 600.000 (us)(Done when 0x284[7]==1)
	4)			WAIT_CALDONE	RXFILTER,2000	// Wait for RX filter to tune, Max Cal Time: 5.585 us (Done when 0x016[7]==0)
	5)			WAIT_CALDONE	TXFILTER,2000	// Wait for TX filter to tune, Max Cal Time: 2.889 us (Done when 0x016[6]==0)
	6)			WAIT_CALDONE	BBDC,2000	// BBDC Max Cal Time: 6575.521 us. Cal done when 0x016[0]==0
	7)			WAIT_CALDONE	RFDC,2000	// Wait for cal to complete (Done when 0x016[1]==0)
	8)			WAIT_CALDONE	TXQUAD,2000	// Wait for cal to complete (Done when 0x016[4]==0)
 */
int WAIT_CALDONE(u16 index);

u8 SPIRead(u16 addr);

void SPIWrite(u16 addr, u8 val);

int spi_reg_verity(u32 addr, u32 val);

void delay_ad9362(u32 time);

void reset_ad9362();

#endif /* AD9361_CFG_H_ */
