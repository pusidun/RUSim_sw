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

	Status = ConfigAD9361LTE();

	Status = ConfigAD9361GSM();

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
