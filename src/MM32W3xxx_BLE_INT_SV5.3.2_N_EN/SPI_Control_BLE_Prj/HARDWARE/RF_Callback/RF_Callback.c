#include "RF_Callback.h"
#include "iwdg.h"
#include "AT_CMD.h"
#include "mg_api.h"

#define TO_HIGH_CASE
#define comrxbuf_wr_pos RxCont
u16 comrxbuf_rd_pos = 0; //init, com rx buffer
#define MAX_AT_CMD_BUF_SIZE 52
u8 AtCmdBuf[MAX_AT_CMD_BUF_SIZE], AtCmdBufDataSize = 0;
#define MAX_SIZE 100
u8 txBuf[MAX_SIZE], rxBuf[MAX_SIZE], txLen = 0;
u16 RxCont = 0;
u8 PosW = 0;

extern u8 CanNotifyFlag;
u8 connect_data = 0;
extern unsigned char WaitSetBaud;
u32 tempI2cData = 0x32;
unsigned int TxTimeout;
extern  unsigned int SysTick_Count;
extern u32 BaudRate;

unsigned int RxTimeout;
extern char GetConnectedStatus(void);
extern void CheckAtCmdInfo(void);
extern void ChangeBaudRate(void);
extern void moduleOutData(u8 *data, u8 len);

extern unsigned char SleepStop;
extern unsigned char SleepStatus;

/********************************************************************************************************
**function: CheckComPortInData
**@brief    This function is used to pass data through Bluetooth.
**
**@param    None.
**
**@return   None.
********************************************************************************************************/
void CheckComPortInData(void) //at cmd NOT supported
{
  u16 send;

  if (comrxbuf_wr_pos != comrxbuf_rd_pos) //not empty
  {
    if (!GetConnectedStatus())
    {
      comrxbuf_rd_pos = comrxbuf_wr_pos; //empty the buffer if any
    }
    else //connected
    {
      if (comrxbuf_wr_pos > comrxbuf_rd_pos)
      {
        send = sconn_notifydata(rxBuf + comrxbuf_rd_pos, comrxbuf_wr_pos - comrxbuf_rd_pos);
        comrxbuf_rd_pos += send;
      }
      else
      {
        send = sconn_notifydata(rxBuf + comrxbuf_rd_pos, MAX_SIZE - comrxbuf_rd_pos);
        comrxbuf_rd_pos += send;
        comrxbuf_rd_pos %= MAX_SIZE;
      }
    }
  }
}

/********************************************************************************************************
**function: UsrProcCallback
**@brief    This function is a callback function. This function will be called
**          every time a Bluetooth interrupt is entered.
**          and the data is processed in this function. You can do something in this function
**@param    None.
**
**@return   None.
********************************************************************************************************/
void UsrProcCallback(void) //porting api
{
  IWDG_Feed();
#ifdef USE_AT_CMD       //AT MODE
  CheckAtCmdInfo();
#else
  CheckComPortInData();//THROUGHTOUT MODE
#endif
  if ((2 != SleepStop) || (!(GPIO_ReadInputData(GPIOA) & 0x800))) //CTL low PA11
  {
    if ((txLen) && (0 == PosW))
    {
      UART_ITConfig(UART2, UART_IT_TXIEN, ENABLE);
      UART_SendData(UART2, txBuf[PosW++]);
      TxTimeout = SysTick_Count + (20000 / BaudRate);
    }
  }
  if ((SleepStop == 2) && (RxTimeout < SysTick_Count))
  {
    RxTimeout = SysTick_Count + (20000 / BaudRate);
  }

#ifdef USE_AT_CMD
  if (WaitSetBaud)
  {
    WaitSetBaud++;
    if (WaitSetBaud > 2)
    {
      ChangeBaudRate();
      WaitSetBaud = 0;
      moduleOutData((u8 *)"IND:OK\n", 7);
    }
  }
#endif
}

static char dis_int_count = 0;

/********************************************************************************************************
**function: DisableEnvINT
**@brief    Turn off Bluetooth interrupt.This function is used in the process of BLE processing data.
**
**@param    None.
**
**@return   None.
********************************************************************************************************/
void DisableEnvINT(void)
{
  //to disable int
  __ASM volatile("cpsid i");
  dis_int_count ++;
}

/********************************************************************************************************
**function: EnableEnvINT
**@brief    Turn on Bluetooth interrupt.This function is used to open the interrupt after
**          the BLE processing data is completed.
**@param    None.
**
**@return   None.
********************************************************************************************************/
void EnableEnvINT(void)
{
  //to enable/recover int
  dis_int_count --;
  if (dis_int_count <= 0) //protection purpose
  {
    dis_int_count = 0; //reset
    __ASM volatile("cpsie i");
  }
}




