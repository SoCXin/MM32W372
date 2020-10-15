#include "spi.h"

/********************************************************************************************************
**function: SPI_WriteRead
**@brief    SPI2 read and write functions 
**                   
**@param    SendData :  SPI2 read or write data
**
**@param    WriteFlag :  SPI2 read or write flag
**
**@return   SPI2->RXREG :  SPI2 received data
********************************************************************************************************/
unsigned char SPI_WriteRead(unsigned char SendData, unsigned char WriteFlag) //porting api
{
  SPI1->TXREG = SendData;
  while (!(SPI1->CSTAT & SPI_FLAG_TXEPT));
  while (!(SPI1->CSTAT & SPI_FLAG_RXAVL));
  return SPI1->RXREG;
}

/********************************************************************************************************
**function: SPI_CS_Enable_
**@brief    SPI2 CS enable
**                   
**@return   None.
********************************************************************************************************/
void SPI_CS_Enable_(void) //porting api
{
  //SPI2->SCSR &= SPI_CS_BIT0;
  GPIO_ResetBits(GPIOD, GPIO_Pin_2);
}

/********************************************************************************************************
**function: SPI_CS_Disable_
**@brief    SPI2 CS disable
**
**@return   None.
********************************************************************************************************/
void SPI_CS_Disable_(void) //porting api
{
  //SPI2->SCSR |= ~SPI_CS_BIT0;
  GPIO_SetBits(GPIOD, GPIO_Pin_2);
}

/********************************************************************************************************
**function: SPIM_TXEn
**@brief    SPI2 tx enable
**                   
**@param    SPIx £ºspi selection.such as SPI1 , SPI2
**
**@return   None.
********************************************************************************************************/
void SPIM_TXEn(SPI_TypeDef *SPIx)
{
  //Transmit Enable bit TXEN
  SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
}

/********************************************************************************************************
**function: SPIM_TXDisable
**@brief    SPI2 tx disable
**                   
**@param    SPIx £ºspi selection.such as SPI1 , SPI2
**
**@return   None.
********************************************************************************************************/
void SPIM_TXDisable(SPI_TypeDef *SPIx)
{
  //disable TXEN
  SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Tx);
}

/********************************************************************************************************
**function: SPIM_RXEn
**@brief    SPI2 rx enable.
**                   
**@param    SPIx £ºspi selection.such as SPI1 , SPI2.
**
**@return   None.
********************************************************************************************************/
void SPIM_RXEn(SPI_TypeDef *SPIx)
{
  //enable RXEN
  SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
}

/********************************************************************************************************
**function: SPIM_RXDisable
**@brief    SPI2 rx disable.
**                   
**@param    SPIx £ºspi selection.such as SPI1 , SPI2.
**
**@return   None.
********************************************************************************************************/
void SPIM_RXDisable(SPI_TypeDef *SPIx)
{
  //disable RXEN
  SPI_BiDirectionalLineConfig(SPIx, SPI_Disable_Rx);
}

/********************************************************************************************************
**function: SPIMReadWriteByte
**@brief    SPI2 read and write functions 
**                   
**@param    SPIx :  SPI2 selection.
**
**@param    spi_baud_div £º Set the SPI transmission.
**
**@return   SPI_ReceiveData.
********************************************************************************************************/
unsigned int SPIMReadWriteByte(SPI_TypeDef *SPIx, unsigned char tx_data)
{
  SPI_SendData(SPIx, tx_data);
  while (1)
  {
    if (SPI_GetFlagStatus(SPIx, SPI_FLAG_TXEPT))
    {
      break;
    }
  }
  while (1)
  {
    if (SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL))
    {
      return SPI_ReceiveData(SPIx);
    }
  }
}

/********************************************************************************************************
**function: SPIM_Init
**@brief    This function is the spi initialization function.  Configure gpio and spi in this function.
**                   
**@param    SPIx :  SPI2 selection.
**
**@param    spi_baud_div £º Set the SPI transmission.
**
**@return   None.
********************************************************************************************************/
void SPIM_Init(unsigned short spi_baud_div)
{
  SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);  //SPI1 clk enable

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SPI_CS_Disable_();
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;          // mode0 SPI_CPOL_Low, SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;        // mode3 SPI_CPOL_High,SPI_CPHA_2Edge
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = spi_baud_div;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPI1, &SPI_InitStructure);

  SPI_Cmd(SPI1, ENABLE);              
  SPIM_TXEn(SPI1);
  SPIM_RXEn(SPI1);

}

/********************************************************************************************************
**function: SPI_WriteBuf           
**@param    pBuf :  data
**
**@param    len :  data length
**
**@return   result
********************************************************************************************************/
unsigned char SPI_WriteBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len)
{
  unsigned char result = 0;
  unsigned char i;
  SPI_CS_Enable_();
  SPI_WriteRead(reg, 1);
  for (i = 0; i < len; i++)
  {
    SPI_WriteRead(*pBuf++, 1);
  }
  SPI_CS_Disable_();
  return result;
}

/********************************************************************************************************
**function: SPI_ReadBuf             
**@param    pBuf :  data
**
**@param    len :  data length
**
**@return   result
********************************************************************************************************/
unsigned char SPI_ReadBuf(unsigned char reg, unsigned char *pBuf, unsigned char len)
{
  unsigned char result = 0;
  unsigned char i;
  SPI_CS_Enable_();
  result = SPI_WriteRead(reg, 1);
  for (i = 0; i < len; i++)
  {
    *(pBuf + i) = SPI_WriteRead(0xff, 0);
  }
  SPI_CS_Disable_();
  return result;
}



