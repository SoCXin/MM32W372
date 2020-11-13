#ifndef __DRV_UART2_H
#define __DRV_UART2_H
#include "sys.h"

#define UART2_MAX_RECV_LEN     2000                    //最大接收缓存字节数
#define UART2_MAX_SEND_LEN     600                 //最大发送缓存字节数
#define UART2_RX_EN            1                   //0,不接收;1,接收.

extern uint8_t  UART2_RX_BUF[UART2_MAX_RECV_LEN];      //接收缓冲,最大UART2_MAX_RECV_LEN字节
extern uint8_t  UART2_TX_BUF[UART2_MAX_SEND_LEN];      //发送缓冲,最大UART2_MAX_SEND_LEN字节
extern uint16_t UART2_RX_STA;                          //接收数据状态

void uart2_init(u32 bound);                //串口2初始化
void u2_printf(char *fmt, ...);


#endif













