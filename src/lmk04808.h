/*
 * lmk04808.h
 *
 *  Created on: 2017Äê12ÔÂ8ÈÕ
 *      Author: dg
 */

#ifndef LMK04808_H_
#define LMK04808_H_

#include "xbasic_types.h"
#include "xstatus.h"

#define Write_Data(BaseAddr, LocalAddr, value)   (*(volatile u32 *)((BaseAddr + LocalAddr)) = (value))
#define Read_Data(BaseAddr, LocalAddr)           (*(volatile u32 *)(BaseAddr + LocalAddr))
#define BaseAddr  0x44a20000

/*Function Prototype*/
void delay(int time);
int init_lmk04808();

/*Instance*/
int init_lmk04808() {
	u32 ReadBuffer[2];
	u32 WriteBuffer[27] = { //RU2.0 -cpri rate3
					0x001f001f, 0x80160180, 0x00140600, 0x00140301, 0x00140602,
					0x00140603, 0x80140304, 0x80140305, 0x110C0006, 0x0C0C0007,
					0x00000008, 0x55555549, 0x9102410a, 0x0405100b, 0x0b0c006c,
					0x1302806d, 0x0230000e, 0x8000800f, 0xc1550410, 0x00000058,
					0x02c01919, 0xafa8001a, 0x1c000a1b, 0x0010041c, 0x0080019d,
					0x0200019e, 0x001f003f };

	for (int k = 0; k < 27; k++) {
		ReadBuffer[0] = Read_Data(BaseAddr, 0x18);
		ReadBuffer[0] &= 0x3;

		while (ReadBuffer[0] == 0x3) {
			ReadBuffer[0] = 0x0;
			Write_Data(BaseAddr, 0xC, WriteBuffer[k]);
			Write_Data(BaseAddr, 0x14, WriteBuffer[0]);
			delay(1);
			Write_Data(BaseAddr, 0x14, WriteBuffer[1]);
			Write_Data(BaseAddr, 0x14, WriteBuffer[0]);
			delay(100);
		}
	}
	ReadBuffer[1] = Read_Data(BaseAddr, 0x18);
	return XST_SUCCESS;
}

void delay(int time) {
	int i, j;
	for (i = 0; i < time; i++)
		for (j = 0; j < 20; j++)
			;
}
#endif /* LMK04808_H_ */
