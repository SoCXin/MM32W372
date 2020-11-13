#include "sys.h"
//#include "mg_api.h"


extern uint32_t SystemCoreClock ;

void WFI_SET(void)
{
  __ASM volatile("wfi");
}

/********************************************************************************************************
**function: INTX_DISABLE
**@brief    Turn off Bluetooth interrupt.This function is used in the process of BLE processing data.
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void INTX_DISABLE(void)
{
  __ASM volatile("cpsid i");
}

/********************************************************************************************************
**function: INTX_ENABLE
**@brief    Turn on Bluetooth interrupt.This function is used to open the interrupt after 
**          the BLE processing data is completed.                 
**@param    None.
**
**@return   None.
********************************************************************************************************/
void INTX_ENABLE(void)
{
  __ASM volatile("cpsie i");
}

/********************************************************************************************************
**function: SYSTick_Configuration
**@brief    This function sets the system clock
**          
**@param    None.
**
**@return   None.
********************************************************************************************************/
void SYSTick_Configuration(void)
{
  SysTick_Config(SystemCoreClock / 1000);
}

unsigned int SysTick_Count = 0;

/********************************************************************************************************
**function: GetSysTickCount
**@brief    This function gets the SysTick_Count value
**          
**@param    None
**
**@return   SysTick_Count.
********************************************************************************************************/
unsigned int GetSysTickCount(void) //porting api
{
  return SysTick_Count;
}

/********************************************************************************************************
**function: SysTick_Handler
**@brief    This function handles SysTick Handler.
**          
**@param    None
**
**@return   None.
********************************************************************************************************/
void SysTick_Handler(void)
{
  SysTick_Count ++;
//  ble_nMsRoutine();
}

