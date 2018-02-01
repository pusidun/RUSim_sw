/*
 * uart.cc
 *
 *  Created on: 2018Äê1ÔÂ22ÈÕ
 *      Author: dg
 */
#include "uart.h"
#include "xuartlite_l.h"
#include "ad9361_cfg.h"

void UartInterruptInit()
{
	int Status=1;
	Status = XUartLite_Initialize(&UartLite, UARTLITE_DEVICE_ID);
	Status = XUartLite_SelfTest(&UartLite);
	XUartLite_ResetFifos(&UartLite);
	Status = SetupInterruptSystem(&UartLite);
	XUartLite_InterruptHandler(&UartLite);
	XUartLite_SetRecvHandler(&UartLite, RecvHandler, &UartLite);
	XUartLite_SetSendHandler(&UartLite, SendHandler, &UartLite);
	XUartLite_EnableInterrupt(&UartLite);
}

void RecvHandler(void *CallBackRef, unsigned int EventData)
{
	UartTotalReceivedCount = EventData;
	handleMessage();
}

/*
 * MESSAGE DEFINATION
 * 16Byte, Index 0-15
 * [0:1]read:0x1 write:0x2
 * [2:5]BaseAddr
 * [6:9]OffsetAddr
 * [10:13]WriteData
 * [14]
 * EOF 0x2a
 * eg:00 01 44 a2 00 00 00 00 00 00 2a
 */
void handleMessage()
{
	u8 RecvBuffer[16];
	u8 SendBuffer[16];
	for(int i=0 ;i<16; i++)
	{
		RecvBuffer[i] = 0;
		SendBuffer[i] = 0;
	}
	//block when FIFO is empty
	//while(XUartLite_IsReceiveEmpty(UARTLITE_BASEADDR));

	for(int i=0; i<16; i++)
	{
		RecvBuffer[i] = XUartLite_RecvByte(UARTLITE_BASEADDR);
		if(RecvBuffer[i]==0x2a)    //if EOF
			break;
	}

	if(RecvBuffer[1]==0x1)
	{
		u32 tBaseAddr = 0;
		u32 OffsetAddr = 0;
		//calculate BaseAddr and OffsetAddr
		for(int i=2; i<6; i++)
		{
			tBaseAddr <<= 8;
			tBaseAddr |= RecvBuffer[i];
		}
		for(int i=6; i<10; i++)
		{
			OffsetAddr <<= 8;
			OffsetAddr |= RecvBuffer[i];
		}
		u32 regData = Read_Data(tBaseAddr, OffsetAddr);
		for(int i=0; i<4; i++)
		{
			SendBuffer[3-i] = regData&0xFF;
			regData >>= 8;
		}
		for(int i=0; i<4; i++)
			XUartLite_SendByte(UARTLITE_BASEADDR, SendBuffer[i]);
		XUartLite_SendByte(UARTLITE_BASEADDR, '\n');
	}
	else if(RecvBuffer[1]==0x2)
	{
		u32 tBaseAddr = 0;
		u32 OffsetAddr = 0;
		u32 writeData = 0;
		for(int i=2; i<6; i++)
		{
			tBaseAddr <<= 8;
			tBaseAddr |= RecvBuffer[i];
		}
		for(int i=6; i<10; i++)
		{
			OffsetAddr <<= 8;
			OffsetAddr |= RecvBuffer[i];
		}
		for(int i=10; i<14; i++)
		{
			writeData <<= 8;
			writeData |= RecvBuffer[i];
		}
		Write_Data(tBaseAddr, OffsetAddr, writeData);
	}
	else if(RecvBuffer[1]==0x3)
	{

		 /* spiwrite
		 * [0:1]0x3
		 * [2:5]4Byte addr
		 * [6:7]2Byte value
		 * [8]0x2a
		 */

		u32 addr = 0;
		u8 value = 0;
		for(int i=2; i<6; i++)
		{
			addr <<= 8;
			addr |= RecvBuffer[i];
		}
		for(int i=6; i<8; i++)
		{
			value <<=8;
			value |= RecvBuffer[i];
		}

		SPIWrite(addr, value);
	}
	else if(RecvBuffer[1]==0x04)
	{

		 /* spiread
		 * [0:1]0x04
		 * [2:5] addr
		 * [6]0x2a
		 * */

		u32 addr = 0;
		for(int i=2; i<6; i++)
		{
			addr <<= 8;
			addr |= RecvBuffer[i];
		}
		u8 regData = SPIRead(addr);
		for(int i=0; i<2; i++)
		{
			SendBuffer[1-i] = regData&0xFF;
			regData >>= 8;
		}
		for(int i=0; i<2; i++)
			XUartLite_SendByte(UARTLITE_BASEADDR, SendBuffer[i]);
		XUartLite_SendByte(UARTLITE_BASEADDR, '\n');
	}

}

void SendHandler(void *CallBackRef, unsigned int EventData)
{
	UartTotalSentCount = EventData;
}

int SetupInterruptSystem(XUartLite *UartLitePtr)
{

	int Status;


	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, UARTLITE_INT_IRQ_ID,
			   (XInterruptHandler)XUartLite_InterruptHandler,
			   (void *)UartLitePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the UartLite can cause interrupts through the interrupt controller.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt for the UartLite device.
	 */
	XIntc_Enable(&InterruptController, UARTLITE_INT_IRQ_ID);

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XIntc_InterruptHandler,
			 &InterruptController);

	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
