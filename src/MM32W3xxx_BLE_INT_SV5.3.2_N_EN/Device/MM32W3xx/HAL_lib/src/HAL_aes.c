/**
******************************************************************************
* @file  HAL_aes.c
* @author  AE team
* @version  V1.1.0
* @date  28/08/2019
* @brief  This file provides all the AES firmware functions.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT,MindMotion SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*/

/* Includes ------------------------------------------------------------------*/

#include "HAL_rcc.h"
#include "HAL_aes.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CR_CLEAR_MASK  ((uint32_t)0xFFFEFF80)


/** @addtogroup StdPeriph_Driver
* @{
*/

/** @defgroup AES
* @brief AES driver modules
* @{
*/

void AES_DeInit(void)
{
    /* Enable AES reset state */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, ENABLE);
    /* Release AES from reset state */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, DISABLE);
}

/**
  * @brief  Initializes the AES peripheral according to the specified parameters
  *         in the AES_InitStruct:
  *           - AES_Operation: specifies the operation mode (encryption, decryption...).
  *           - AES_Chaining: specifies the chaining mode (ECB, CBC or CTR).
  *           - AES_DataType: specifies the data swapping type: 32-bit, 16-bit, 8-bit or 1-bit.
  * @note   If AES is already enabled, use AES_Cmd(DISABLE) before setting the new
  *         configuration (When AES is enabled, setting configuration is forbidden).
  * @param  AES_InitStruct: pointer to an AES_InitTypeDef structure that contains
  *         the configuration information for AES peripheral.
  * @retval None
  */
void AES_Init(AES_InitTypeDef *AES_InitStruct)
{
    uint32_t tmpreg = 0;

    /* Check the parameters */
    assert_param(IS_AES_MODE(AES_InitStruct->AES_Operation));
    assert_param(IS_AES_CHAINING(AES_InitStruct->AES_Chaining));
    assert_param(IS_AES_DATATYPE(AES_InitStruct->AES_DataType));

    /* Get AES CR register value */
    tmpreg = AES->CR;

    /* Clear DATATYPE[1:0], MODE[1:0] and CHMOD[1:0] bits */
    tmpreg &= (uint32_t)CR_CLEAR_MASK;

    tmpreg |= (AES_InitStruct->AES_Operation | AES_InitStruct->AES_Chaining | AES_InitStruct->AES_DataType);

    AES->CR = (uint32_t) tmpreg;
}

/**
  * @brief  Initializes the AES Keys according to the specified parameters in the AES_KeyInitStruct.
  * @param  AES_KeyInitStruct: pointer to an AES_KeyInitTypeDef structure that
  *         contains the configuration information for the specified AES Keys.
  * @note   This function must be called while the AES is disabled.
  * @note   In encryption, key derivation and key derivation + decryption modes,
  *         AES_KeyInitStruct must contain the encryption key.
  *         In decryption mode, AES_KeyInitStruct must contain the decryption key.
  * @retval None
  */
void AES_KeyInit(AES_KeyInitTypeDef *AES_KeyInitStruct)
{
    AES->KEYR0 = AES_KeyInitStruct->AES_Key0;
    AES->KEYR1 = AES_KeyInitStruct->AES_Key1;
    AES->KEYR2 = AES_KeyInitStruct->AES_Key2;
    AES->KEYR3 = AES_KeyInitStruct->AES_Key3;
    AES->KEYR4 = AES_KeyInitStruct->AES_Key4;
    AES->KEYR5 = AES_KeyInitStruct->AES_Key5;
    AES->KEYR6 = AES_KeyInitStruct->AES_Key6;
    AES->KEYR7 = AES_KeyInitStruct->AES_Key7;
}

/**
  * @brief  Initializes the AES Initialization Vector IV according to
  *         the specified parameters in the AES_IVInitStruct.
  * @param  AES_KeyInitStruct: pointer to an AES_IVInitTypeDef structure that
  *         contains the configuration information for the specified AES IV.
  * @note   When ECB chaining mode is selected, Initialization Vector IV has no
  *         meaning.
  *         When CTR chaining mode is selected, AES_IV0 contains the CTR value.
  *         AES_IV1, AES_IV2 and AES_IV3 contains nonce value.
  * @retval None
  */
void AES_IVInit(AES_IVInitTypeDef *AES_IVInitStruct)
{
    AES->IVR0 = AES_IVInitStruct->AES_IV0;
    AES->IVR1 = AES_IVInitStruct->AES_IV1;
    AES->IVR2 = AES_IVInitStruct->AES_IV2;
    AES->IVR3 = AES_IVInitStruct->AES_IV3;
}

/**
  * @brief  Enable or disable the AES peripheral.
  * @param  NewState: new state of the AES peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @note   The key must be written while AES is disabled.
  * @retval None
  */
void AES_Cmd(FunctionalState NewState)
{
    /* Check the parameter */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the AES peripheral */
        AES->CR |= (uint32_t) AES_CR_EN;   /**< AES Enable */
    }
    else
    {
        /* Disable the AES peripheral */
        AES->CR &= (uint32_t)(~AES_CR_EN);  /**< AES Disable */
    }
}


/**
  * @brief  choose the key size
  * @param  keySize
    * @note     CRL_AES128_KEY 128bit key size
    * @note     CRL_AES192_KEY 192bit key size
    * @note     CRL_AES256_KEY 256bit key size
  * @retval None
  */
void AES_KeySize(uint32_t CRL_AESxxx_KEY)
{
    uint32_t temp;
    /*read AES_CR value*/
    temp = AES->CR;

    /*clear the AES_CR register KSIZE[1:0]*/
    temp &= 0xfff3ffff;

    /*write the KSIZE to AES_CR*/
    AES->CR = temp | CRL_AESxxx_KEY;

}

/**
  * @}
  */

/** @defgroup AES_Group2 Structures initialization functions
 *  @brief   Structures initialization.
 *
@verbatim
 ===============================================================================
              ##### Structures initialization functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Fills each AES_InitStruct member with its default value.
  * @param  AES_InitStruct: pointer to an AES_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void AES_StructInit(AES_InitTypeDef *AES_InitStruct)
{
    AES_InitStruct->AES_Operation = AES_Operation_Encryp;
    AES_InitStruct->AES_Chaining = AES_Chaining_ECB;
    AES_InitStruct->AES_DataType = AES_DataType_32b;
}

/**
  * @brief  Fills each AES_KeyInitStruct member with its default value.
  * @param  AES_KeyInitStruct: pointer to an AES_KeyInitStruct structure which
  *         will be initialized.
  * @retval None
  */
void AES_KeyStructInit(AES_KeyInitTypeDef *AES_KeyInitStruct)
{
    AES_KeyInitStruct->AES_Key0 = 0x00000000;
    AES_KeyInitStruct->AES_Key1 = 0x00000000;
    AES_KeyInitStruct->AES_Key2 = 0x00000000;
    AES_KeyInitStruct->AES_Key3 = 0x00000000;
    AES_KeyInitStruct->AES_Key4 = 0x00000000;
    AES_KeyInitStruct->AES_Key5 = 0x00000000;
    AES_KeyInitStruct->AES_Key6 = 0x00000000;
    AES_KeyInitStruct->AES_Key7 = 0x00000000;
}

/**
  * @brief  Fills each AES_IVInitStruct member with its default value.
  * @param  AES_IVInitStruct: pointer to an AES_IVInitTypeDef structure which
  *         will be initialized.
  * @retval None
  */
void AES_IVStructInit(AES_IVInitTypeDef *AES_IVInitStruct)
{
    AES_IVInitStruct->AES_IV0 = 0x00000000;
    AES_IVInitStruct->AES_IV1 = 0x00000000;
    AES_IVInitStruct->AES_IV2 = 0x00000000;
    AES_IVInitStruct->AES_IV3 = 0x00000000;
}

/**
  * @}
  */

/** @defgroup AES_Group3 AES Read and Write
 *  @brief   AES Read and Write.
 *
@verbatim
 ===============================================================================
                  ##### AES Read and Write functions #####
 ===============================================================================

@endverbatim
  * @{
  */
/**
  * @brief  Write data in DINR register to be processed by AES peripheral.
  * @note   To process 128-bit data (4 * 32-bit), this function must be called
  *         four times to write the 128-bit data in the 32-bit register DINR.
  * @note   When an unexpected write to DOUTR register is detected, WRERR flag is
  *         set.
  * @param  Data: The data to be processed.
  * @retval None
  */
void AES_WriteSubData(uint32_t Data)
{
    unsigned int i = 0;
    /* Write Data */
    AES->DINR = Data;
    while (i--);
}

/**
  * @brief  Returns the data in DOUTR register processed by AES peripheral.
  * @note   This function must be called four times to get the 128-bit data.
  * @note   When an unexpected read of DINR register is detected, RDERR flag is
  *         set.
  * @retval The processed data.
  */
uint32_t AES_ReadSubData(void)
{
    /* Read Data */
    return AES->DOUTR;
}


/**
  * @brief  Read the Key value.
  * @param  AES_KeyInitStruct: pointer to an AES_KeyInitTypeDef structure which
  *         will contain the key.
  * @note   When the key derivation mode is selected, AES must be disabled
  *         (AES_Cmd(DISABLE)) before reading the decryption key.
  *         Reading the key while the AES is enabled will return unpredictable
  *         value.
  * @retval None
  */
void AES_ReadKey(AES_KeyInitTypeDef *AES_KeyInitStruct)
{
    AES_KeyInitStruct->AES_Key0 = AES->KEYR0;
    AES_KeyInitStruct->AES_Key1 = AES->KEYR1;
    AES_KeyInitStruct->AES_Key2 = AES->KEYR2;
    AES_KeyInitStruct->AES_Key3 = AES->KEYR3;
    AES_KeyInitStruct->AES_Key4 = AES->KEYR4;
    AES_KeyInitStruct->AES_Key5 = AES->KEYR5;
    AES_KeyInitStruct->AES_Key6 = AES->KEYR6;
    AES_KeyInitStruct->AES_Key7 = AES->KEYR7;
}

/**
  * @brief  Read the Initialization Vector IV value.
  * @param  AES_IVInitStruct: pointer to an AES_IVInitTypeDef structure which
  *         will contain the Initialization Vector IV.
  * @note   When the AES is enabled Reading the Initialization Vector IV value
  *         will return 0. The AES must be disabled using AES_Cmd(DISABLE)
  *         to get the right value.
  * @note   When ECB chaining mode is selected, Initialization Vector IV has no
  *         meaning.
  *         When CTR chaining mode is selected, AES_IV0 contains 32-bit Counter value.
  *         AES_IV1, AES_IV2 and AES_IV3 contains nonce value.
  * @retval None
  */
void AES_ReadIV(AES_IVInitTypeDef *AES_IVInitStruct)
{
    AES_IVInitStruct->AES_IV0 = AES->IVR0;
    AES_IVInitStruct->AES_IV1 = AES->IVR1;
    AES_IVInitStruct->AES_IV2 = AES->IVR2;
    AES_IVInitStruct->AES_IV3 = AES->IVR3;
}

/**
  * @}
  */

/** @defgroup AES_Group4 DMA transfers management functions
 *  @brief   DMA transfers management function.
 *
@verbatim
 ===============================================================================
               ##### DMA transfers management functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Configures the AES DMA interface.
  * @param  AES_DMATransfer: Specifies the AES DMA transfer.
  *   This parameter can be one of the following values:
  *     @arg AES_DMATransfer_In: When selected, DMA manages the data input phase.
  *     @arg AES_DMATransfer_Out: When selected, DMA manages the data output phase.
  *     @arg AES_DMATransfer_InOut: When selected, DMA manages both the data input/output phases.
  * @param  NewState Indicates the new state of the AES DMA interface.
  *           This parameter can be: ENABLE or DISABLE.
  * @note   The DMA has no action in key derivation mode.
  * @retval None
  */
void AES_DMAConfig(uint32_t AES_DMATransfer, FunctionalState NewState)
{
    /* Check the parameter */
    assert_param(IS_AES_DMA_TRANSFER(AES_DMATransfer));

    if (NewState != DISABLE)
    {
        /* Enable the DMA transfer */
        AES->CR |= (uint32_t) AES_DMATransfer;
    }
    else
    {
        /* Disable the DMA transfer */
        AES->CR &= (uint32_t)(~AES_DMATransfer);
    }
}


/**
  * @}
  */

/** @defgroup AES_Group5 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions.
 *
@verbatim

 ===============================================================================
           ##### Interrupts and flags management functions #####
 ===============================================================================
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified AES interrupt.
  * @param  AES_IT: Specifies the AES interrupt source to enable/disable.
  *     This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt. If enabled, once CCF
  *                     flag is set an interrupt is generated.
  *     @arg AES_IT_ERR: Error Interrupt. If enabled, once a read error
  *                      flags (RDERR) or write error flag (WRERR) is set,
  *                      an interrupt is generated.
  * @param  NewState: The new state of the AES interrupt source.
  *                   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AES_ITConfig(uint32_t AES_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_AES_IT(AES_IT));

    if (NewState != DISABLE)
    {
        AES->CR |= (uint32_t) AES_IT;    /**< AES_IT Enable */
    }
    else
    {
        AES->CR &= (uint32_t)(~AES_IT);  /**< AES_IT Disable */
    }
}

/**
  * @brief  Checks whether the specified AES flag is set or not.
  * @param  AES_FLAG specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg AES_FLAG_CCF: Computation Complete Flag is set by hardware when
  *                        he computation phase is completed.
  *     @arg AES_FLAG_RDERR: Read Error Flag is set when an unexpected read
  *                          operation of DOUTR register is detected.
  *     @arg AES_FLAG_WRERR: Write Error Flag  is set when an unexpected write
  *                          operation in DINR is detected.
  * @retval FlagStatus (SET or RESET)
  */
FlagStatus AES_GetFlagStatus(uint32_t AES_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check parameters */
    assert_param(IS_AES_FLAG(AES_FLAG));

    if ((AES->SR & AES_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    /* Return the AES_FLAG status */
    return  bitstatus;
}

/**
  * @brief  Clears the AES flags.
  * @param  AES_FLAG: specifies the flag to clear.
  *         This parameter can be:
  *     @arg AES_FLAG_CCF: Computation Complete Flag is cleared by setting CCFC
  *                        bit in CR register.
  *     @arg AES_FLAG_RDERR: Read Error is cleared by setting ERRC bit in
  *                          CR register.
  *     @arg AES_FLAG_WRERR: Write Error is cleared by setting ERRC bit in
  *                          CR register.
  * @retval None
  */
void AES_ClearFlag(uint32_t AES_FLAG)
{
    /* Check the parameters */
    assert_param(IS_AES_FLAG(AES_FLAG));

    /* Check if AES_FLAG is AES_FLAG_CCF */
    if (AES_FLAG == AES_FLAG_CCF)
    {
        /* Clear CCF flag by setting CCFC bit */
        AES->CR |= (uint32_t) AES_CR_CCFC;
    }
    else     /* AES_FLAG is AES_FLAG_RDERR or AES_FLAG_WRERR */
    {
        /* Clear RDERR and WRERR flags by setting ERRC bit */
        AES->CR |= (uint32_t) AES_CR_ERRC;
    }
}


/**
  * @brief  Checks whether the specified AES interrupt has occurred or not.
  * @param  AES_IT: Specifies the AES interrupt pending bit to check.
  *         This parameter can be:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval ITStatus The new state of AES_IT (SET or RESET).
  */
ITStatus AES_GetITStatus(uint32_t AES_IT)
{
    ITStatus itstatus = RESET;
    uint32_t cciebitstatus = RESET, ccfbitstatus = RESET;

    /* Check parameters */
    assert_param(IS_AES_GET_IT(AES_IT));

    cciebitstatus = AES->CR & AES_CR_CCFIE;
    ccfbitstatus =  AES->SR & AES_SR_CCF;

    /* Check if AES_IT is AES_IT_CC */
    if (AES_IT == AES_IT_CC)
    {
        /* Check the status of the specified AES interrupt */
        if (((cciebitstatus) != (uint32_t)RESET) && ((ccfbitstatus) != (uint32_t)RESET))
        {
            /* Interrupt occurred */
            itstatus = SET;
        }
        else
        {
            /* Interrupt didn't occur */
            itstatus = RESET;
        }
    }
    else     /* AES_IT is AES_IT_ERR */
    {
        /* Check the status of the specified AES interrupt */
        if ((AES->CR & AES_CR_ERRIE) != RESET)
        {
            /* Check if WRERR or RDERR flags are set */
            if ((AES->SR & (uint32_t)(AES_SR_WRERR | AES_SR_RDERR)) != (uint16_t)RESET)
            {
                /* Interrupt occurred */
                itstatus = SET;
            }
            else
            {
                /* Interrupt didn't occur */
                itstatus = RESET;
            }
        }
        else
        {
            /* Interrupt didn't occur */
            itstatus = (ITStatus) RESET;
        }
    }

    /* Return the AES_IT status */
    return itstatus;
}

/**
  * @brief  Clears the AES's interrupt pending bits.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval None
  */
void AES_ClearITPendingBit(uint32_t AES_IT)
{
    /* Check the parameters */
    assert_param(IS_AES_IT(AES_IT));

    /* Clear the interrupt pending bit */
    AES->CR |= (uint32_t)(AES_IT >> (uint32_t) 0x00000002);
}
/**
  * @brief  Write AES's keys.
    @arg Key: big end mode.
  * @retval None
  */
void AES_Write_KEY(uint8_t *Key)
{
    u32 tempReg;
    AES->CR = tempReg;
    tempReg = (tempReg >> 18) & 0x3;
    if(tempReg == 0)
    {
        AES->KEYR3 = (u32)(Key[0]  << 24) | (u32)(Key[1]  << 16) | (u32)(Key[2]  << 8) | (u32)(Key[3]  << 0);
        AES->KEYR2 = (u32)(Key[4]  << 24) | (u32)(Key[5]  << 16) | (u32)(Key[6]  << 8) | (u32)(Key[7]  << 0);
        AES->KEYR1 = (u32)(Key[8]  << 24) | (u32)(Key[9]  << 16) | (u32)(Key[10] << 8) | (u32)(Key[11] << 0);
        AES->KEYR0 = (u32)(Key[12] << 24) | (u32)(Key[13] << 16) | (u32)(Key[14] << 8) | (u32)(Key[15] << 0);
    }
    else if(tempReg == 1)
    {
        AES->KEYR5 = (u32)(Key[0]  << 24) | (u32)(Key[1]  << 16) | (u32)(Key[2]  << 8) | (u32)(Key[3]  << 0);
        AES->KEYR4 = (u32)(Key[4]  << 24) | (u32)(Key[5]  << 16) | (u32)(Key[6]  << 8) | (u32)(Key[7]  << 0);
        AES->KEYR3 = (u32)(Key[8]  << 24) | (u32)(Key[9]  << 16) | (u32)(Key[10] << 8) | (u32)(Key[11] << 0);
        AES->KEYR2 = (u32)(Key[12] << 24) | (u32)(Key[13] << 16) | (u32)(Key[14] << 8) | (u32)(Key[15] << 0);
        AES->KEYR1 = (u32)(Key[16] << 24) | (u32)(Key[17] << 16) | (u32)(Key[18] << 8) | (u32)(Key[19] << 0);
        AES->KEYR0 = (u32)(Key[20] << 24) | (u32)(Key[21] << 16) | (u32)(Key[22] << 8) | (u32)(Key[23] << 0);
    }
    else if(tempReg == 2)
    {
        AES->KEYR7 = (u32)(Key[0]  << 24) | (u32)(Key[1]  << 16) | (u32)(Key[2]  << 8) | (u32)(Key[3]  << 0);
        AES->KEYR6 = (u32)(Key[4]  << 24) | (u32)(Key[5]  << 16) | (u32)(Key[6]  << 8) | (u32)(Key[7]  << 0);
        AES->KEYR5 = (u32)(Key[8]  << 24) | (u32)(Key[9]  << 16) | (u32)(Key[10] << 8) | (u32)(Key[11] << 0);
        AES->KEYR4 = (u32)(Key[12] << 24) | (u32)(Key[13] << 16) | (u32)(Key[14] << 8) | (u32)(Key[15] << 0);
        AES->KEYR3 = (u32)(Key[16] << 24) | (u32)(Key[17] << 16) | (u32)(Key[18] << 8) | (u32)(Key[19] << 0);
        AES->KEYR2 = (u32)(Key[20] << 24) | (u32)(Key[21] << 16) | (u32)(Key[22] << 8) | (u32)(Key[23] << 0);
        AES->KEYR1 = (u32)(Key[24] << 24) | (u32)(Key[25] << 16) | (u32)(Key[26] << 8) | (u32)(Key[27] << 0);
        AES->KEYR0 = (u32)(Key[28] << 24) | (u32)(Key[29] << 16) | (u32)(Key[30] << 8) | (u32)(Key[31] << 0);
    }
}
/**
  * @brief  Write AES's data.
    @arg Key: big end mode.
  * @retval None
  */
void AES_Write_Data(uint8_t *indat)
{
    u8 i;
    for(i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(indat[i * 4 + 3] << 0) | (u32)(indat[i * 4 + 2] << 8) | \
                         (u32)(indat[i * 4 + 1] << 16) | (u32)(indat[i * 4 + 0] << 24));
    }
}

/**
  * @brief  Write AES's data.
    @arg Key: big end mode.
  * @retval None
  */
void AES_Read_Data(uint8_t *outdat)
{
    u8 i;
    u32 temp[4];
    temp[0] = AES->DOUTR;
    temp[1] = AES->DOUTR;
    temp[2] = AES->DOUTR;
    temp[3] = AES->DOUTR;
    for(i = 0; i < 4; i ++)
    {
        outdat[i * 4 + 0] = (temp[i] >> 24) & 0xff;
        outdat[i * 4 + 1] = (temp[i] >> 16) & 0xff;
        outdat[i * 4 + 2] = (temp[i] >> 8) & 0xff;
        outdat[i * 4 + 3] = (temp[i] >> 0) & 0xff;
    }
}
/**
  * @brief  In cipher-block chaining (EBC) mode,,AES_EBC_Encrypt.
  * @param  AES_KeySize:Choose the  KeySize .
  *     @arg CRL_AES128_KEY: 128bit.
  *     @arg CRL_AES192_KEY: 192bit.
  *     @arg CRL_AES256_KEY: 256bit.
  * @retval None
  */
ErrorStatus AES_ECB_Encrypt(uint8_t *Key, uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
//    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable


    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0;//ECB Encrypt MODE
    AES->CR = tempReg;

    AES_Write_KEY( Key );
    AES->CR |= 1;//AES enable

    AES_Write_Data(Input);
    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        AES_Read_Data(Output);
    }
    return Status;
}
/**
  * @brief  In cipher-block chaining (CBC) mode,AES_CBC_Decryption.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval None
  */
ErrorStatus AES_ECB_Decrypt(uint8_t *Key, uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
//    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable

    AES_Write_KEY( Key );

    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x2 << 3; //ECB Decrypt MODE
    AES->CR = tempReg;

    AES->CR |= 1;//AES enable
    AES_Write_Data(Input);

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        AES_Read_Data(Output);
    }
    return Status;
}

/**
  * @brief  In cipher-block chaining (CBC) mode,,AES_CBC_Encrypt.
  * @param  AES_KeySize:Choose the  KeySize .
  *     @arg CRL_AES128_KEY: 128bit.
  *     @arg CRL_AES192_KEY: 192bit.
  *     @arg CRL_AES256_KEY: 256bit.
  * @retval None
  */
ErrorStatus AES_CBC_Encrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x1 << 5; //CBC Encrypt MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable
    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}
/**
  * @brief  In cipher-block chaining (CBC) mode,AES_CBC_Decryption.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval None
  */
ErrorStatus AES_CBC_Decrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x1 << 3; //Key expasion MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->CR |= 1;//AES enable

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);



    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= (0x1 << 5) | (0x2 << 3); //CBC Decrypt MODE
    AES->CR = tempReg;

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable

    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}

/**
  * @brief  In cipher-block chaining (CTR) mode,,AES_CTR_Encrypt.
  * @param  AES_KeySize:Choose the  KeySize .
  *     @arg CRL_AES128_KEY: 128bit.
  *     @arg CRL_AES192_KEY: 192bit.
  *     @arg CRL_AES256_KEY: 256bit.
  * @retval None
  */
ErrorStatus AES_CTR_Encrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x2 << 5;  //CTR Encrypt MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable

    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}
/**
  * @brief  In cipher-block chaining (CTR) mode,AES_CTR_Decryption.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval None
  */
ErrorStatus AES_CTR_Decrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;

    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x1 << 3; //Key expasion MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->CR |= 1;//AES enable

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);


    AES->CR &= 0xFFFFFFFE;//AES disable
    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= (0x2 << 5) | (0x2 << 3); //ECB Decrypt MODE
    AES->CR = tempReg;

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable

    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}



/**
  * @brief  In cipher-block chaining (EBC) mode,,AES_EBC_Encrypt.
  * @param  AES_KeySize:Choose the  KeySize .
  *     @arg CRL_AES128_KEY: 128bit.
  *     @arg CRL_AES192_KEY: 192bit.
  *     @arg CRL_AES256_KEY: 256bit.
  * @retval None
  */
ErrorStatus AES_CFB_Encrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable


    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= (0x1 << 16) | (0x0 << 5); //CFB Encrypt MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable
    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}
/**
  * @brief  In cipher-block chaining (CBC) mode,AES_CBC_Decryption.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval None
  */
ErrorStatus AES_CFB_Decrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;

    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x1 << 3; //Key expasion MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->CR |= 1;//AES enable

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);



    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= (0x1 << 16) | (0x0 << 5) | (0x2 << 3); //CFB Decrypt MODE
    AES->CR = tempReg;

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable

    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}

/**
  * @brief  In output-feedbackchaining (OFB) mode,,AES_EBC_Encrypt.
  * @param  AES_KeySize:Choose the  KeySize .
  *     @arg CRL_AES128_KEY: 128bit.
  *     @arg CRL_AES192_KEY: 192bit.
  *     @arg CRL_AES256_KEY: 256bit.
  * @retval None
  */

ErrorStatus AES_OFB_Encrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= (0x1 << 16) | (0x1 << 5); //OFB Encrypt MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable
    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
}
/**
  * @brief  In output-feedback chaining (OFB) mode,AES_CBC_Decryption.
  * @param  AES_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combinations of the following values:
  *     @arg AES_IT_CC: Computation Complete Interrupt.
  *     @arg AES_IT_ERR: Error Interrupt.
  * @retval None
  */
ErrorStatus AES_OFB_Decrypt(uint8_t *Key, uint8_t InitVectors[16], uint8_t *Input, uint32_t Ilength, uint8_t *Output)
{
    ErrorStatus Status = SUCCESS;
    int i = 0;
    u32 tempData[4];
    u32 tempReg;
    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= 0x1 << 3; //Key expasion MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->CR |= 1;//AES enable

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);


    AES->CR &= 0xFFFFFFFE;//AES disable

    tempReg = AES->CR ;
    tempReg &= 0xFFFEFF87;//mask bit3,4,5,6,16
    tempReg |= (0x1 << 16) | (0x1 << 5) | (0x2 << 3); //OFB Decrypt MODE
    AES->CR = tempReg;

    AES->KEYR0 = (u32)(Key[0] << 0) | (u32)(Key[1] << 8) | (u32)(Key[2] << 16) | (u32)(Key[3] << 24);
    AES->KEYR1 = (u32)(Key[4] << 0) | (u32)(Key[5] << 8) | (u32)(Key[6] << 16) | (u32)(Key[7] << 24);
    AES->KEYR2 = (u32)(Key[8] << 0) | (u32)(Key[9] << 8) | (u32)(Key[10] << 16) | (u32)(Key[11] << 24);
    AES->KEYR3 = (u32)(Key[12] << 0) | (u32)(Key[13] << 8) | (u32)(Key[14] << 16) | (u32)(Key[15] << 24);
    AES->KEYR4 = (u32)(Key[16] << 0) | (u32)(Key[17] << 8) | (u32)(Key[18] << 16) | (u32)(Key[19] << 24);
    AES->KEYR5 = (u32)(Key[20] << 0) | (u32)(Key[21] << 8) | (u32)(Key[22] << 16) | (u32)(Key[23] << 24);
    AES->KEYR6 = (u32)(Key[24] << 0) | (u32)(Key[25] << 8) | (u32)(Key[26] << 16) | (u32)(Key[27] << 24);
    AES->KEYR7 = (u32)(Key[28] << 0) | (u32)(Key[29] << 8) | (u32)(Key[30] << 16) | (u32)(Key[31] << 24);

    AES->IVR0 = (u32)(InitVectors[0] << 0) | (u32)(InitVectors[1] << 8) | (u32)(InitVectors[2] << 16) | (u32)(InitVectors[3] << 24);
    AES->IVR1 = (u32)(InitVectors[4] << 0) | (u32)(InitVectors[5] << 8) | (u32)(InitVectors[6] << 16) | (u32)(InitVectors[7] << 24);
    AES->IVR2 = (u32)(InitVectors[8] << 0) | (u32)(InitVectors[9] << 8) | (u32)(InitVectors[10] << 16) | (u32)(InitVectors[11] << 24);
    AES->IVR3 = (u32)(InitVectors[12] << 0) | (u32)(InitVectors[13] << 8) | (u32)(InitVectors[14] << 16) | (u32)(InitVectors[15] << 24);

    AES->CR |= 1;//AES enable

    for (i = 0; i < 4; i ++)
    {
        AES_WriteSubData((u32)(Input[i * 4 + 0] << 0) | (u32)(Input[i * 4 + 1] << 8) | \
                         (u32)(Input[i * 4 + 2] << 16) | (u32)(Input[i * 4 + 3] << 24));
    }

    do
    {
        if (AES_GetFlagStatus(AES_FLAG_CCF))
        {

            if (AES_GetFlagStatus(AES_FLAG_WRERR) | AES_GetFlagStatus(AES_FLAG_RDERR))
            {
                AES_ClearFlag(AES_FLAG_WRERR);
                Status =  ERROR;
            }
            AES_ClearFlag(AES_FLAG_CCF);
            break;
        }
        if (i == 1000)
        {
            Status =  ERROR;
        }
    }
    while (i++ < 1000);

    if (Status !=  ERROR)
    {
        tempData[0] = AES->DOUTR;
        tempData[1] = AES->DOUTR;
        tempData[2] = AES->DOUTR;
        tempData[3] = AES->DOUTR;
        for (i = 0; i < 4; i ++)
        {
            *(Output + i * 4 + 0) = (tempData[i] >> 0) & 0xff;
            *(Output + i * 4 + 1) = (tempData[i] >> 8) & 0xff;
            *(Output + i * 4 + 2) = (tempData[i] >> 16) & 0xff;
            *(Output + i * 4 + 3) = (tempData[i] >> 24) & 0xff;
        }
    }
    return Status;
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
