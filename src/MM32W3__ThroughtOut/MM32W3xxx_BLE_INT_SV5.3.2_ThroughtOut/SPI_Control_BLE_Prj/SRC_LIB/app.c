/*
    Copyright (c) 2019 Macrogiga Electronics Co., Ltd.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/
#include <string.h>
#include "HAL_conf.h"
#include "mg_api.h"

#define SOFTWARE_INFO                 "SV2.0.1"
#define MANU_INFO                     "Bluetooth"

#define TYPE_INC                      0x2802
#define TYPE_CHAR                     0x2803
#define TYPE_INFO                     0x2901
#define TYPE_CFG                      0x2902
#define TYPE_xRpRef                   0x2907
#define TYPE_RpRef                    0x2908

/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                           0x10
#define ATT_CHAR_PROP_IND                           0x20

#define UUID16_FORMAT                 0xFF
#define GATT_PRIMARY_SERVICE_UUID     0x2800

// Statement ---------------------------------------------------------------------
/* Note: make sure each notify handle by invoking function: set_notifyhandle(hd) */
u16 cur_notifyhandle = 0x12;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

// APP ---------------------------------------------------------------------------
#define MAX_NAME_LEN  24
char DeviceInfo[MAX_NAME_LEN + 1] =  {11, 'M', 'S', '1', '8', '9', '2', '-', 'U', 'A', 'R', 'T'}; /*first byte is len, max len is 24*/

u8 *getDeviceInfoData(u8 *len);

typedef struct ble_character16
{
  u16 type16;          //type2
  u16 handle_rec;      //handle
  u8  characterInfo[5];//property1 - handle2 - uuid2
  u8  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
} BLE_CHAR;

typedef struct ble_UUID128
{
  u8  uuid128[16];    //uuid128 string: little endian
} BLE_UUID128;

/**********************************************************************************
                 *****DataBase****

    01 - 06  GAP (Primary service) 0x1800
      03:04  name
    07 - 0f  Device Info (Primary service) 0x180a
      0a:0b  firmware version
      0e:0f  software version
    10 - 19  UART service (Primary service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
      11:12  6E400003-B5A3-F393-E0A9-E50E24DCCA9E(0x04)  RxNotify
      13     cfg
      14:15  6E400002-B5A3-F393-E0A9-E50E24DCCA9E(0x0C)  Tx
      16     cfg
      17:18  6E400004-B5A3-F393-E0A9-E50E24DCCA9E(0x0A)  BaudRate
      19     0x2901  info

************************************************************************************/

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] =
{
// ======  gatt =====  Do NOT Change!!
  {TYPE_CHAR, 0x0003, {ATT_CHAR_PROP_RD, 0x04, 0, 0x00, 0x2a}, UUID16_FORMAT}, //name
  //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!
  {TYPE_CHAR, 0x0008, {ATT_CHAR_PROP_RD, 0x09, 0, 0x29, 0x2a}, UUID16_FORMAT}, //manufacture
  {TYPE_CHAR, 0x000a, {ATT_CHAR_PROP_RD, 0x0b, 0, 0x26, 0x2a}, UUID16_FORMAT}, //firmware version
  {TYPE_CHAR, 0x000e, {ATT_CHAR_PROP_RD, 0x0f, 0, 0x28, 0x2a}, UUID16_FORMAT}, //sw version

// ======  User service or other services added here =====  User defined
  {TYPE_CHAR, 0x0011, {ATT_CHAR_PROP_NTF,                     0x12, 0, 0, 0}, 1/*uuid128-idx1*/ }, //RxNotify
  {TYPE_CFG, 0x0013, {ATT_CHAR_PROP_RD | ATT_CHAR_PROP_W}}, //cfg
  {TYPE_CHAR, 0x0014, {ATT_CHAR_PROP_W | ATT_CHAR_PROP_W_NORSP, 0x15, 0, 0, 0}, 2/*uuid128-idx2*/ }, //Tx
  {TYPE_CHAR, 0x0017, {ATT_CHAR_PROP_W | ATT_CHAR_PROP_RD,      0x18, 0, 0, 0}, 3/*uuid128-idx3*/ }, //BaudRate
  {TYPE_INFO, 0x0019, {ATT_CHAR_PROP_RD}} //description,"BaudRate"
};

const BLE_UUID128 AttUuid128List[] =
{
  /*for supporting the android app [nRF UART V2.0], one SHOULD using the 0x9e,0xca,0xdc.... uuid128*/
  {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 1, 0, 0x40, 0x6e}, //idx0,little endian, service uuid
  {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 3, 0, 0x40, 0x6e}, //idx1,little endian, RxNotify
  {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 2, 0, 0x40, 0x6e}, //idx2,little endian, Tx
  {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 4, 0, 0x40, 0x6e}, //idx3,little endian, BaudRate
};

u8 GetCharListDim(void)
{
  return sizeof(AttCharList) / sizeof(AttCharList[0]);
}

//////////////////////////////////////////////////////////////////////////
///STEP1:Service declare
// read by type request handle, primary service declare implementation
void att_server_rdByGrType(u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type)
{
  //!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!

  if ((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1)) //hard code for device info service
  {
    //att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
    //GAP Device Name
    u8 t[] = {0x00, 0x18};
    att_server_rdByGrTypeRspPrimaryService(pdu_type, 0x1, 0x6, (u8 *)(t), 2);
    return;
  }
  else if ((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07)) //hard code for device info service
  {
    //apply user defined (device info)service example
    u8 t[] = {0xa, 0x18};
    att_server_rdByGrTypeRspPrimaryService(pdu_type, 0x7, 0xf, (u8 *)(t), 2);
    return;
  }

  else if ((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
  {
    att_server_rdByGrTypeRspPrimaryService(pdu_type, 0x10, 0x19, (u8 *)(AttUuid128List[0].uuid128), 16);
    return;
  }
  //other service added here if any
  //to do....

  ///error handle
  att_notFd(pdu_type, attOpcode, st_hd);
}

///STEP2:data coming
/// write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/,
                   u16 att_hd, u8 *attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{
  switch (att_hd)
  {
  case 0x18://BaudRate
    ser_write_rsp_pkt(pdu_type);
    break;
  case 0x15://Tx
    //out data here....
  {
    void MouduleOutData(u8 * data, u8 len);
    MouduleOutData(attValue, valueLen_w);
  }
  //loop back for test purpose if any
  //sconn_notifydata(attValue,valueLen_w);

  case 0x12://cmd
  case 0x13://cfg
    ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
    break;

  default:
    att_notFd(pdu_type, attOpcode, att_hd);    /*the default response, also for the purpose of error robust */
    break;
  }
}

//// read response
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
  u8 tab[3];
  u8  d_len;
  u8 *ble_name = getDeviceInfoData(&d_len);

  switch (attHandle) //hard code
  {
  case 0x04: //GAP name
    att_server_rd(pdu_type, attOpcode, attHandle, ble_name, d_len);
    break;

  case 0x09: //MANU_INFO
    //att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
    att_server_rd(pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char *)get_ble_version())); //ble lib build version
    break;

  case 0x0b: //FIRMWARE_INFO
  {
    //do NOT modify this code!!!
    att_server_rd(pdu_type, attOpcode, attHandle, GetFirmwareInfo(), strlen((const char *)GetFirmwareInfo()));
    break;
  }

  case 0x0f://SOFTWARE_INFO
    att_server_rd(pdu_type, attOpcode, attHandle, (u8 *)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO) - 1);
    break;

  case 0x13://cfg
  {
    u8 t[2] = {0, 0};
    att_server_rd(pdu_type, attOpcode, attHandle, t, 2);
  }
  break;

  case 0x18://BaudRate
    tab[0] = (115200 & 0xff0000) >> 16;
    tab[1] = (112500 & 0xff00) >> 8;
    tab[2] = 115200 & 0xFF;
    att_server_rd(pdu_type, attOpcode, attHandle, tab, 3);
    break;

  case 0x19: //description
#define MG_BaudRate "BaudRate"
    att_server_rd(pdu_type, attOpcode, attHandle, (u8 *)(MG_BaudRate), sizeof(MG_BaudRate) - 1);
    break;

  default:
    att_notFd(pdu_type, attOpcode, attHandle);  /*the default response, also for the purpose of error robust */
    break;
  }
}


/* Callback --------------------------------------------------------------- */

void gatt_user_send_notify_data_callback(void)
{
  //add user sending data notify operation ....
}

/* Callback --------------------------------------------------------------- */

u8 *getDeviceInfoData(u8 *len)
{
  *len = DeviceInfo[0];
  return (u8 *)&DeviceInfo[1];
}

void updateDeviceInfoData(u8 *name, u8 len)
{
  if (len > MAX_NAME_LEN) len = MAX_NAME_LEN;
  DeviceInfo[0] = len;
  memcpy(&DeviceInfo[1], name, len);
  ble_set_name(name, len);
}


/// Queued Writes data if any
void ser_prepare_write(u16 handle, u8 *attValue, u16 attValueLen, u16 att_offset)//user's call back api
{
  //queued data:offset + data(size)
  //when ser_execute_write() is invoked, means end of queue write.

  //to do
}

void ser_execute_write(void)//user's call back api
{
  //end of queued writes

  //to do...
}

// return 1 means found
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,
                            unsigned short *hd_start_r, unsigned short *hd_end_r)
{
  if (hd_start <= 0x10)
  {
    if (uuid16 == 0xFEB3)
    {
      *hd_start_r = 0x10;
      *hd_end_r   = 0x1c;
      return 1;
    }
  }

  return 0;
}

u8 *getsoftwareversion(void)
{
  return (u8 *)SOFTWARE_INFO;
}

void ble_cmd_cache_init(void);
void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
  //[IsConnectedFlag] indicates the connection status
  void reset_uart_buf(void);

  reset_uart_buf();
}

void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP, u16 offset)
{
}

//---- MindMotion HardWare AES implemenation -----
//unsigned char aes_encrypt_HW(unsigned char *painText128bitBE,unsigned char *key128bitBE); //porting api, returns zero means not supported
unsigned char aes_encrypt_HW(unsigned char *_data, unsigned char *_key)
{
  unsigned int tmp = 0;

#ifndef AES_HW_SUPPORT
  return 0; //HW AES NOT supported

#else  //HW AES supported

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_AES, ENABLE); //AES CLK enable

  AES->CR = 0x00;
  AES->CR |= 0x03 << 20;

  AES->KEYR3 = (u32)(_key[0] << 24) | (u32)(_key[1] << 16) | (u32)(_key[2] << 8) | (u32)(_key[3] << 0);
  AES->KEYR2 = (u32)(_key[4] << 24) | (u32)(_key[5] << 16) | (u32)(_key[6] << 8) | (u32)(_key[7] << 0);
  AES->KEYR1 = (u32)(_key[8] << 24) | (u32)(_key[9] << 16) | (u32)(_key[10] << 8) | (u32)(_key[11] << 0);
  AES->KEYR0 = (u32)(_key[12] << 24) | (u32)(_key[13] << 16) | (u32)(_key[14] << 8) | (u32)(_key[15] << 0);

  AES->CR |= 0x01;  //start encryption

  AES->DINR = (u32)(_data[0] << 24) | (u32)(_data[1] << 16) | (u32)(_data[2] << 8) | (u32)(_data[3] << 0);
  AES->DINR = (u32)(_data[4] << 24) | (u32)(_data[5] << 16) | (u32)(_data[6] << 8) | (u32)(_data[7] << 0);
  AES->DINR = (u32)(_data[8] << 24) | (u32)(_data[9] << 16) | (u32)(_data[10] << 8) | (u32)(_data[11] << 0);
  AES->DINR = (u32)(_data[12] << 24) | (u32)(_data[13] << 16) | (u32)(_data[14] << 8) | (u32)(_data[15] << 0);

  //²éÑ¯Ä£Ê½
  while (1)
  {
    if ((AES->SR & 0x01))
    {
      AES->CR |=  0x1 << 7; //clear ccf flag
      break;
    }
  }

  tmp = AES->DOUTR;  //encrypted output data0
  _data[0] = tmp >> 24;
  _data[1] = tmp >> 16;
  _data[2] = tmp >>  8;
  _data[3] = tmp;
  _data += 4;

  tmp = AES->DOUTR;  //encrypted output data1
  _data[0] = tmp >> 24;
  _data[1] = tmp >> 16;
  _data[2] = tmp >>  8;
  _data[3] = tmp;
  _data += 4;

  tmp = AES->DOUTR;  //encrypted output data2
  _data[0] = tmp >> 24;
  _data[1] = tmp >> 16;
  _data[2] = tmp >>  8;
  _data[3] = tmp;
  _data += 4;

  tmp = AES->DOUTR;  //encrypted output data3
  _data[0] = tmp >> 24;
  _data[1] = tmp >> 16;
  _data[2] = tmp >>  8;
  _data[3] = tmp;

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_AES, DISABLE); //AES CLK disable

  return 1;// HW supported value
#endif
}