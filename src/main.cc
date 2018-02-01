#include "ad9361_cfg.h"
#include "xparameters.h"
#include "tempsensor.h"
#include "eeprom.h"
#include "uart.h"
#include "lmk04808.h"
#include "common.h"


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
	//float tmp=getLM75();

	UartInterruptInit();

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

	while(1);
}
