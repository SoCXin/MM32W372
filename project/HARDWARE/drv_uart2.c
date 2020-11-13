#include "delay.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "drv_timer.h"
#include "drv_uart2.h"

uint8_t UART2_RX_BUF[UART2_MAX_RECV_LEN];              //接收缓冲,最大UART2_MAX_RECV_LEN个字节.
uint8_t  UART2_TX_BUF[UART2_MAX_SEND_LEN];             //发送缓冲,最大UART2_MAX_SEND_LEN字节
uint16_t UART2_RX_STA = 0;

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度

void UART2_IRQHandler(void)
{
    uint8_t res;
    if (UART_GetITStatus(UART2, UART_IT_RXIEN)  != RESET)
    {
        res = UART_ReceiveData(UART2);
        UART_ClearITPendingBit(UART2, UART_IT_RXIEN);

        if ((UART2_RX_STA & (1 << 15)) == 0) //接收完的一批数据,还没有被处理,则不再接收其他数据
        {
            if (UART2_RX_STA < UART2_MAX_RECV_LEN) //还可以接收数据
            {
                TIM_SetCounter(TIM3, 0); //计数器清空                         //计数器清空
                if (UART2_RX_STA == 0)             //使能定时器3的中断
                {
                    TIM_Cmd(TIM3, ENABLE); //使能定时器3
                }
                UART2_RX_BUF[UART2_RX_STA++] = res; //记录接收到的值
            }
            else
            {
                UART2_RX_STA |= 1 << 15;           //强制标记接收完成
            }
        }
    }
}

void uart2_init(u32 bound)
{
    GPIO_InitTypeDef gpio_initstruct;
    UART_InitTypeDef UART_initstruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);
    //PA2   TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_2;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);
    //PA3   RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_3;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    UART_initstruct.UART_BaudRate = bound;
    UART_initstruct.UART_HardwareFlowControl = UART_HardwareFlowControl_None;        //无硬件流控
    UART_initstruct.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;                        //接收和发送
    UART_initstruct.UART_Parity = UART_Parity_No;                                    //无校验
    UART_initstruct.UART_StopBits = UART_StopBits_1;                             //1位停止位
    UART_initstruct.UART_WordLength = UART_WordLength_8b;                            //8位数据位
    UART_Init(UART2, &UART_initstruct);

    NVIC_InitStruct.NVIC_IRQChannel = UART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;      //3
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;             //2
    NVIC_Init(&NVIC_InitStruct);
    
    UART_Cmd(UART2, ENABLE);                                                      //使能串口

    UART_ITConfig(UART2, UART_IT_RXIEN, ENABLE);

    tim3_init(1000 - 1, 9600 - 1);  //10ms中断
    UART2_RX_STA = 0;      //清零
    TIM_Cmd(TIM3, DISABLE);         //关闭定时器7
}

void u2_printf(char *fmt, ...)
{
    uint16_t i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)UART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)UART2_TX_BUF);    //此次发送数据的长度
    for (j = 0; j < i; j++)                     //循环发送数据
    {
        while ((UART2->CSR & UART_IT_TXIEN) == 0); //Send data cyclically
        UART_SendData(UART2, UART2_TX_BUF[j]);
    }
}


