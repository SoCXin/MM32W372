#include "RF_Callback.h"
#include "iwdg.h"
#include "AT_CMD.h"
#include "mg_api.h"
///////////////FIFO proc for AT cmd///////////////
#define TO_HIGH_CASE
#define comrxbuf_wr_pos RxCont
u16 comrxbuf_rd_pos = 0; //init, com rx buffer
#define MAX_AT_CMD_BUF_SIZE 52
u8 AtCmdBuf[MAX_AT_CMD_BUF_SIZE], AtCmdBufDataSize = 0;
#define MAX_SIZE 100
u8 txBuf[MAX_SIZE], rxBuf[MAX_SIZE], txLen = 0;
u16 RxCont = 0;
//u16 PosR = 0;
u8 PosW = 0;
extern u8 CanNotifyFlag;
u8 connect_data = 0;
extern unsigned char WaitSetBaud;
u32 tempI2cData = 0x32;
unsigned int TxTimeout;
extern  unsigned int SysTick_Count;
extern u32 BaudRate;
bool  key_shutter=FALSE;

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
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
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
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}
/********************************************************************************************************
**function: Key_Shutter_Init
**@brief    This function is the initialization function of the selfie stick button, used to initialize GPIO, clock, external interrupt
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void Key_Shutter_Init()
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //外部中断，需要使能AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/********************************************************************************************************
**function: EXTI15_10_IRQHandler
**@brief    Bluetooth interrupt handler
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if((EXTI_GetITStatus(EXTI_Line11)!= RESET)&&(key_shutter==FALSE))
	{
		key_shutter=TRUE;
		EXTI_ClearITPendingBit(EXTI_Line11);
		
	}
}
