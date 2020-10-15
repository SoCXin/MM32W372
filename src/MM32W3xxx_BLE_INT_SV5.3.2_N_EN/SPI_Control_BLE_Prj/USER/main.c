#include "sys.h"
#include "delay.h"
#include "uart.h"
#include "spi.h"
#include "RF_IRQ.h"
#include <string.h>
#include "HAL_conf.h"
#include "mg_api.h"
#include "mg_test_api.h"
#include "iwdg.h"

unsigned char *ble_mac_addr;
u8 value_t[2];
unsigned char *get_local_addr(void) //used for ble pairing case
{
  return ble_mac_addr;
}

void  BLE_SV()
{
  unsigned char *BLE_SV = get_ble_version();
}

u8 ADV_Data[] = {'T', 'E', 'S', 'T'};
unsigned long i;
int main(void)
{
  unsigned long temp = 0x800000;
  while (temp--);
  SPIM_Init(0x06);
  IRQ_RF();
  SYSTick_Configuration();
  uart_initwBaudRate();
  SetBleIntRunningMode();
  radio_initBle(TXPWR_3DBM, &ble_mac_addr);
  IWDG_Init(4, 625);

  value_t[0] = 0xc0;
  value_t[1] = *(u8 *)0x1FFFF820; //Read FT value(FT value:The RF module is calibrated at the factory to prevent frequency deviation. The user can call the FT value in the program)

//  mg_activate(0x53);//If FT values are true£¬please open here
//  mg_writeBuf(0x4, value_t, 2); //write FT values
//  mg_activate(0x56);
    
  ble_run_interrupt_start(80);//320*0.625=200 ms
  while (1)
  {
  }
}

