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

#define PAYLOAD_SIZE 300
#define AD9362_WRITE 0x1c
#define AD9362_READ 0x20
#define SPI_FPGA_CORE_WR 0x28
#define SPI_distribution 0x2c

#define Write_Data(BaseAddr, LocalAddr, value)   (*(volatile u32 *)((BaseAddr + LocalAddr)) = (value))
#define Read_Data(BaseAddr, LocalAddr)           (*(volatile u32 *)(BaseAddr + LocalAddr))
#define BaseAddr  0x44a20000

/************************** Function Prototypes ******************************/

u8 SPIRead(u16 addr);

void SPIWrite(u16 addr, u8 val);

int spi_reg_verity(u32 addr, u32 val);

int spi_fpga_core_w(u32 addr, u32 val);

int spi_fpga_core_r(u32 addr);

//read&write enable; 1:enable, 0:disable
int spi_rw_enable(u8 f);

//0x1 GSM , 0x2 LTE
int spi_ad_select(u32 chip);

void roc_script();

void delay_ad9362(u32 time);

#endif /* AD9361_CFG_H_ */
