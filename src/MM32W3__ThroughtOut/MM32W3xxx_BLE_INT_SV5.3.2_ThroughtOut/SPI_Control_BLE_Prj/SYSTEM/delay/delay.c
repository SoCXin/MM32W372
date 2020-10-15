#include "delay.h"
#if SYSTEM_SUPPORT_OS
  #include "includes.h"
#endif

static u8  fac_us = 0;           
static u16 fac_ms = 0;           

/********************************************************************************************************
**function: delay_init
**@brief    delay init
**          
**@param    None.
**
**@return   None.
********************************************************************************************************/
void delay_init()
{
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //  HCLK/8
  fac_us = SystemCoreClock / 8000000;   
  fac_ms = (u16)fac_us * 1000;      
}

/********************************************************************************************************
**function: delay_us
**@brief    Call this function for us delay
**          
**@param    nus  :  Delay time  
**
**@return   None.
********************************************************************************************************/
void delay_us(u32 nus)
{
  u32 temp;
  SysTick->LOAD = nus * fac_us;       
  SysTick->VAL = 0x00;                
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ; 
  do
  {
    temp = SysTick->CTRL;
  }
  while ((temp & 0x01) && !(temp & (1 << 16))); 
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
  SysTick->VAL = 0X00;               
}

/********************************************************************************************************
**function: delay_ms
**@brief    Call this function for ms delay
**          
**@param    nms  :  Delay time  
**
**@return   None.
********************************************************************************************************/
void delay_ms(u16 nms)
{
  u32 temp;
  SysTick->LOAD = (u32)nms * fac_ms;   
  SysTick->VAL = 0x00;            
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ; 
  do
  {
    temp = SysTick->CTRL;
  }
  while ((temp & 0x01) && !(temp & (1 << 16))); 
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
  SysTick->VAL = 0X00;           
}









































