/*
 * Empty C++ Application
 */

#include "lmk04808.h"
#include "ad9361_cfg.h"
#include "stdio.h"
#include "xparameters.h"
#include "tempsensor.h"
#include "unistd.h"
#include "eeprom.h"


int main()
{
	int Status = 0;

	/*config lmk04808*/
	Status = init_lmk04808();

	/*
	 * Config ROC straightly, however, block RAM will be overflowed.
	 * So we abolish this way and using EEPROM.
	 * We remain these code just for debug.
	 */
	//Status = ConfigAD9361LTE();
	//Status = ConfigAD9361GSM();

	/*write ROC config data to EEPROM*/
	//writeScriptEeprom();

	/*Config ROC through reading EEPROM*/
	readScriptEeprom();

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
