/*
 * Empty C++ Application
 */

#include "lmk04808.h"
#include "ad9361_cfg.h"
#include "stdio.h"
#include "xparameters.h"
#include "tempsensor.h"
#include "unistd.h"

int main()
{
	int Status = 0;
	//config lmk04808
	Status = init_lmk04808();

	//³õÊ¼»¯FPGA SPI¿ØÖÆÆ÷£¬ Write 0x0024  0x0
	Write_Data(BaseAddr,0x0024,0xc);

	//test case for SPIWrite SPIRead
	u32 reg;
	SPIWrite(0x3F4,0x2c);
	reg = SPIRead(0x3F4);
	SPIWrite(0x3F5, 0x1d);
	reg = SPIRead(0x3F5);
	SPIWrite(0x3F4, 0x2b);
	reg = SPIRead(0x3F4);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
