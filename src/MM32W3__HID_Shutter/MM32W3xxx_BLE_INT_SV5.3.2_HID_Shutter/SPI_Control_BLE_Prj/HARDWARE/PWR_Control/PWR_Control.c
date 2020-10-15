#include "pwr_control.h"

unsigned char SleepStop = 0x02;
unsigned char SleepStatus = 0;
extern  unsigned int SysTick_Count;
extern unsigned int RxTimeout, TxTimeout, SysTick_Count;
/********************************************************************************************************
**function: McuGotoSleepAndWakeup
**@brief    This function is the MCU auto goto sleep AND wakeup, porting api
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
  if ((SleepStop) &&
      (TxTimeout < SysTick_Count) &&
      (RxTimeout < SysTick_Count))
  {
    if (SleepStop == 1) //sleep
    {
			NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP,ENABLE);
      __WFI();
			RCC_HSICmd(ENABLE); 
			RCC_PLLCmd(ENABLE);
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    }
  }
}
