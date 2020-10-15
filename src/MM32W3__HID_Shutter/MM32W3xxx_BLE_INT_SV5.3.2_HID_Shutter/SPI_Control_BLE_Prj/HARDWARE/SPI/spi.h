#ifndef __SPI_H__
#define __SPI_H__
#include "sys.h"

#define READ          0x03
#define FAST_READ     0x0B
#define RDID          0x9F
#define WREN      0x06
#define WRDI      0x04
#define SE        0xD8
#define BE        0xC7
#define PP        0x02
#define RDSR      0x05
#define WRSR      0x01
#define DP        0xB9
#define RES       0xAB

//#define SPI_INT_MODE
//#define SPI_DMA
void SPI_CS_Disable_(void);

void SPIM_Init(unsigned short spi_baud_div);
u8 SPIM_ReadID(SPI_TypeDef *SPIx);
void SPIM_WriteEnable(SPI_TypeDef *SPIx);
void SPIM_checkStatus(SPI_TypeDef *SPIx);
void SPIM_WriteDisable(SPI_TypeDef *SPIx);
void SPIM_PageRead(SPI_TypeDef *SPIx, unsigned long address, unsigned char *p, unsigned int number);
void SPIM_PageProgram(SPI_TypeDef *SPIx, unsigned long address, unsigned char *p, unsigned int number);
void SPIM_SectorErase(SPI_TypeDef *SPIx, unsigned long address);
void SPIM_BlockErase(SPI_TypeDef *SPIx);
void SPIM_Close(SPI_TypeDef *SPIx);
void data_clear(u8 *data, u8 size);

#endif
