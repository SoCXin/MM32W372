#include "RF_IRQ.h"
#include "mg_api.h"

/********************************************************************************************************
**function: IsIrqEnabled
**@brief    This function is used to determine whether a Bluetooth interrupt is generated
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
char IsIrqEnabled(void) //porting api
{
  return (!(GPIO_ReadInputData(GPIOC) & 0x0200));
}

/********************************************************************************************************
**function: IRQ_RF
**@brief    This function is the IRQ initialization function.  Configure Clock GPIO and NVIC in this function.
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void IRQ_RF(void)//PC9 Wake UP MCU from STOP Mode
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  //IRQ - PC9
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);

  EXTI_InitStructure.EXTI_Line = EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_SetPriority(EXTI9_5_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

/********************************************************************************************************
**function: EXTI9_5_IRQHandler
**@brief    Bluetooth interrupt handler
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line9);
  ble_run(0);
}
