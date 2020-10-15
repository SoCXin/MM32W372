/**
******************************************************************************
* @file    iwdg.c
* @author  AE Team
* @version V1.3.9
* @date    28/08/2019
* @brief   This file provides all the iwdg firmware functions.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MindMotion SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*/
#include "iwdg.h"

/********************************************************************************************************
**function: PVU_CheckStatus
**@brief    Check the status bits
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void PVU_CheckStatus(void)
{
  while (1)
  {
    if (IWDG_GetFlagStatus(IWDG_FLAG_PVU) == RESET)
    {
      break;
    }
  }
}

/********************************************************************************************************
**function: RVU_CheckStatus
**@brief    Check the status bits
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void RVU_CheckStatus(void)
{
  while (1)
  {
    if (IWDG_GetFlagStatus(IWDG_FLAG_RVU) == RESET)
    {
      break;
    }
  }
}

/********************************************************************************************************
**function: IWDG_Init
**@brief    Turn on watchdog
**                   
**@param    prer 
**
**@param    rlr  
**
**@return   None.
********************************************************************************************************/
void IWDG_Init(u8 prer, u16 rlr)
{
  RCC_LSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
  PVU_CheckStatus();
  IWDG_WriteAccessCmd(0x5555);
  IWDG_SetPrescaler(prer);
  RVU_CheckStatus();
  IWDG_WriteAccessCmd(0x5555);
  IWDG_SetReload(rlr & 0xfff);
  IWDG_ReloadCounter();
  IWDG_Enable();
}

/********************************************************************************************************
**function: IWDG_Feed
**@brief    Feeding dog function
**                   
**@param    None.
**
**@return   None.
********************************************************************************************************/
void IWDG_Feed(void)
{
  IWDG_ReloadCounter();
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/*-------------------------(C) COPYRIGHT 2019 MindMotion ----------------------*/




