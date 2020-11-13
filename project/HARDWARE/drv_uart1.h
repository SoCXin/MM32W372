#ifndef __DRV_UART1_H__
#define __DRV_UART1_H__

#include "sys.h" 

#define LOG(fmt,args...) printf(fmt,##args)

void uart1_init(uint32_t baudrate);
void MyUartPrintf(UART_TypeDef *UARTx, char *fmt,...);

#endif


