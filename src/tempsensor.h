/*
 * tempsensor.h
 *
 *  Created on: 2017年12月13日
 *      Author: dg
 */

#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_
/*
#include "xparameters.h"
#include "xiic_l.h"

#define IIC_BASE_ADDRESS	         XPAR_IIC_0_BASEADDR
#define TEMP_SENSOR_ONCHIP_ADDRESS   0x4B     //LM75温度传感器设备地址

int LowLevelTempSensorExample(u32 IicBaseAddress,
				u8 TempSensorAddress,
				u8 *TemperaturePtr);

int tempsensor(void)
{
	u8 TemperaturePtr[2]={0x00}; //存储读取的二字节温度值
	float Temperature = 0.0;

	int ByteCount;

	ByteCount = XIic_Recv(IIC_BASE_ADDRESS,
			              TEMP_SENSOR_ONCHIP_ADDRESS,
			              TemperaturePtr,
			              2,
			              XIIC_STOP);
	TemperaturePtr[1] = TemperaturePtr[1]>>7;
	TemperaturePtr[1] &= 0x01;
	Temperature = (TemperaturePtr[0] + TemperaturePtr[1]) * 0.5; //温度值

	return ByteCount;
}
*/


#endif /* TEMPSENSOR_H_ */
