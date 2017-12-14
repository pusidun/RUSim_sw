/*
 * common.h
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: dg
 */

#ifndef COMMON_H_
#define COMMON_H_

//delay ms microseconds
void delay_us(u32 ms)
{
	int i, j;
	for (i = 0; i < time; i++)
		for (j = 0; j < 10; j++)//20:2us 1:100ns
			;
}



#endif /* COMMON_H_ */
