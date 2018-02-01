/*
 * uart.h
 *
 *  Created on: 2018Äê1ÔÂ22ÈÕ
 *      Author: dg
 */

#ifndef UART_H_
#define UART_H_

#include "xparameters.h"
#include "xuartlite.h"
#include "xuartlite_l.h"
#include "xintc.h"
#include "xil_exception.h"

#define UARTLITE_DEVICE_ID      XPAR_UARTLITE_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define UARTLITE_INT_IRQ_ID     XPAR_INTC_0_UARTLITE_0_VEC_ID
#define UARTLITE_BASEADDR	   XPAR_UARTLITE_0_BASEADDR

#define MESSAGE_SIZE 16
#define Write_Data(BaseAddr, LocalAddr, value)   (*(volatile u32 *)((BaseAddr + LocalAddr)) = (value))
#define Read_Data(BaseAddr, LocalAddr)           (*(volatile u32 *)(BaseAddr + LocalAddr))

static XUartLite UartLite;            /* The instance of the UartLite Device */
static XIntc InterruptController;     /* The instance of the Interrupt Controller */

static volatile int UartTotalReceivedCount;
static volatile int UartTotalSentCount;
/************************** Function Prototypes ******************************/
void UartInterruptInit();

void handleMessage();

int UartLiteIntrExample(u16 DeviceId);

int SetupInterruptSystem(XUartLite *UartLitePtr);

void SendHandler(void *CallBackRef, unsigned int EventData);

void RecvHandler(void *CallBackRef, unsigned int EventData);

#endif /* UART_H_ */
