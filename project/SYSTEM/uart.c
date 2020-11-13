#include "sys.h"
#include "uart.h"

#define MAX_SIZE 100
//u32 BaudRate = 9600/*115200*/; //115200;
u32 BaudRate = 115200;

extern u8 txBuf[MAX_SIZE], rxBuf[MAX_SIZE], txLen, PosW;
extern u16 RxCont;
extern unsigned int RxTimeout, TxTimeout, SysTick_Count;
u8 RX_DATA_BUFF[125];
u8 UART_RX_BUF[256];     
u8 t;
u16 UART_RX_STA = 0;
//////////////////////////////////////////////////////////////////
#ifdef __GNUC__

  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif

#ifdef USE_IAR
PUTCHAR_PROTOTYPE
{
  while ((UART1->CSR & UART_IT_TXIEN) == 0); //Send data cyclically
  UART1->TDR = (ch & (uint16_t)0x00FF);
  return ch;
}

#else
#pragma import(__use_no_semihosting)
//Support functions required by the standard library
struct __FILE
{
  int handle;
};

FILE __stdout;
void _sys_exit(int x)
{
  x = x;
}

/********************************************************************************************************
**function: fputc
**@brief         Write a character to a file.redefine function of printf function,
**               need to call this function in printf function         
**@param    ch : Data to be transmitted
**
**@param    f  : Pointer to FILE structure
**
**@return   ch
********************************************************************************************************/
int fputc(int ch, FILE *f)
{
  while ((UART1->CSR & UART_IT_TXIEN) == 0); //Send data cyclically
  UART1->TDR = (ch & (uint16_t)0x00FF);
  return ch;
}

#endif

/********************************************************************************************************
**function: uart_initwBaudRate
**@brief    Use this function to configure the basic configuration of the serial port,
**          such as serial clock, pins, interrupts
**@param    None
**
**@return   None.
********************************************************************************************************/
void uart_initwBaudRate(UART_TypeDef *UARTx)
{
  //GPIO cfg
  GPIO_InitTypeDef GPIO_InitStructure;
  UART_InitTypeDef UART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //Ê¹ÄÜUART1£¬GPIOAÊ±ÖÓ
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);

  //UART1 NVIC Cfg
  NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //Preemption priority 3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //UART1 IRQ channel enable
  NVIC_Init(&NVIC_InitStructure); //Initialize the VIC register according to the specified parameters

  //UART  initialization Cfg
  UART_InitStructure.UART_BaudRate = BaudRate; //BaudRate
  UART_InitStructure.UART_WordLength = UART_WordLength_8b;//Word length is 8-bit data format
  UART_InitStructure.UART_StopBits = UART_StopBits_1;//stop bit
  UART_InitStructure.UART_Parity = UART_Parity_No;//No parity
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//No hardware data flow control
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx; //Configure send and receive mode

  UART_Init(UART1, &UART_InitStructure); //UART1 enable
  UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE);
  UART_ITConfig(UART1, UART_IT_RXIEN, ENABLE);//Enable UART receive interrupt
  UART_Cmd(UART1, ENABLE);                   //UART1 enable

  //UART1_TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //Multiplexed push-pull output
  GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIO initialization

  //UART1_RX 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //Floating input
  GPIO_Init(GPIOA, &GPIO_InitStructure);                //GPIO initialization

}

/********************************************************************************************************
**function: ChangeBaudRate
**@brief    Use this function to configure BaudRate
**          
**@param    None.
**
**@return   None.
********************************************************************************************************/
void ChangeBaudRate(void)
{
  uint32_t apbclock = 0x00;
  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  UART_Cmd(UART1, DISABLE);
  apbclock = RCC_ClocksStatus.PCLK1_Frequency;
  UART1->BRR = (apbclock / BaudRate) / 16;
  UART1->FRA = (apbclock / BaudRate) % 16;
  UART_Cmd(UART1, ENABLE);
}

/********************************************************************************************************
**function: moduleOutData
**@brief    Data sending API functions.Serial communication can call this function to send data
**          
**@param    data : Data address
**
**@param    len : Data length
**
**@return   None.
********************************************************************************************************/
/*
void moduleOutData(u8 *data, u8 len) //api
{
  unsigned char i;
//    printf("%s\n",data); //debug
  if ((txLen + len) < MAX_SIZE) //buff not overflow
  {
    for (i = 0; i < len; i++)
    {
      txBuf[txLen + i] = *(data + i);
    }
    txLen += len;
  }
}
*/
/********************************************************************************************************
**function: UART1_IRQHandler
**@brief    Serial port interrupt function.Secondary interrupts include receive interrupts 
**          and transmit interrupts. Data can be processed in this interrupt          
**@param    None.
**
**@return   None.
********************************************************************************************************/

void UART1_IRQHandler(void)                 
{
    /*
  if (UART_GetITStatus(UART1, UART_IT_RXIEN)  != RESET)
  {
    UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
    rxBuf[RxCont] = UART_ReceiveData(UART1);
    RxTimeout = SysTick_Count + 1000;
    RxCont++;
    if (RxCont >= MAX_SIZE)
    {
      RxCont = 0;
    }
  }
  if (UART_GetITStatus(UART1, UART_IT_TXIEN)  != RESET)
  {
    UART_ClearITPendingBit(UART1, UART_IT_TXIEN);
    //
    TxTimeout = SysTick_Count + (20000 / BaudRate);
    if (PosW < txLen)
    {
      UART_SendData(UART1, txBuf[PosW++]);
      if (PosW == txLen)
      {
        txLen = 0;
        PosW = 0;
      }
    }
    else
    {
      UART_ITConfig(UART1, UART_IT_TXIEN, DISABLE);
    }
  }*/
}
