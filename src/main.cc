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

	/*config lmk04808*/
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
	/*Status = ConfigAD9361LTE();*/

	//Status = ConfigAD9361GSM();

	/*
	 * write ROC config data to EEPROM
	 *should be comment when run
	 */
	//Status=writeScriptEeprom();

	/*Config ROC(ad9363) through reading EEPROM*/
	readScriptEeprom();

	chipSelect = 0x01;
	SPIWrite(0x3f4, 0xb3);
	SPIWrite(0x73, 0x40);
	SPIWrite(0x75, 0x40);
	chipSelect = 0x02;
	SPIWrite(0x3f4, 0xb3);
	SPIWrite(0x73, 0x40);
	SPIWrite(0x75, 0x40);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
