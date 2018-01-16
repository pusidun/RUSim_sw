/*
 * Empty C++ Application
 */

#include "lmk04808.h"
#include "ad9361_cfg.h"
#include "xparameters.h"
#include "tempsensor.h"
#include "eeprom.h"


int main()
{
	int Status = 0;
	u8 tmp2;

	/*
	Write_Data(BaseAddr, 0x002c, 0x1);
	tmp2 =Read_Data(BaseAddr, 0x002C) ;
	Write_Data(BaseAddr, 0x002c, 0x2);
	tmp2 =Read_Data(BaseAddr, 0x002C) ;
	/*tmp2 =Read_Data(BaseAddr, 0x34) ;
	Write_Data(BaseAddr, 0x34, 0x0);
	tmp2 =Read_Data(BaseAddr, 0x34) ;
	Write_Data(BaseAddr, 0x34, 0x3);
	tmp2 =Read_Data(BaseAddr, 0x34) ;
	chipSelect = 0x1;
	tmp2 = SPIRead(0x3f4);
	SPIWrite(0x3f4, 0xb3);
	tmp2 = SPIRead(0x3f4);
	SPIWrite(0x73, 0x40);
	SPIWrite(0x75, 0x40);
	tmp2 = SPIRead(0x73);
	tmp2 = SPIRead(0x75);


	config lmk04808*/
	Status = init_lmk04808();
	u32 version = Read_Data(BaseAddr, 0x0);
	/*
	 * read temperature from LM75
	 */
	float tmp=getLM75();

	/*
	 * Config ROC straightly, however, block RAM will be overflowed.
	 * So we abolish this way and using EEPROM.
	 * We remain these code just for debug.
	 */
	Status = ConfigAD9361LTE();

	tmp2 = SPIRead(0x3f4);
	SPIWrite(0x3f4, 0xb3);
	tmp2 = SPIRead(0x3f4);

	/*
	chipSelect = 0x1;
	tmp2 = SPIRead(0x3f4);
	SPIWrite(0x3f4, 0xb3);
	tmp2 = SPIRead(0x3f4);
	SPIWrite(0x73, 0x40);
	SPIWrite(0x75, 0x40);
	tmp2 = SPIRead(0x73);
	tmp2 = SPIRead(0x75);
	//Status = ConfigAD9361GSM();

	/*
	 * write ROC config data to EEPROM
	 *should be comment when run
	 */
	//Status=writeScriptEeprom();

	/*Config ROC(ad9363) through reading EEPROM*/
	//readScriptEeprom();

	tmp2 = SPIRead(0x73);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
