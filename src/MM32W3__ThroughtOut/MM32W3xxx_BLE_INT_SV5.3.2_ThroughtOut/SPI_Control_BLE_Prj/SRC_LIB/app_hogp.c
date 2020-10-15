/*
    Copyright (c) 2015 Macrogiga Electronics Co., Ltd.

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

#ifndef BLE_PAIR_SUPPORT //default cfg

#include <string.h>
#include "app.h"
#include "HAL_conf.h"
#include "mg_api.h"
u8 connect_flag = 0;
extern u32 BaudRate;
extern u8 txBuf[], rxBuf[], txLen, PosW;
extern u16 RxCont;
u16 NotifyCont = 0;
u8 CanNotifyFlag = 0;
extern unsigned char SleepStop;
extern unsigned char pld_adv[];
extern unsigned char len_adv;
#define MAX_SIZE 100
#define NOTIFYSIZE  20

void moduleOutData(u8 *data, u8 len);
void ChangeBaudRate(void);

/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                           0x10
#define ATT_CHAR_PROP_IND                           0x20
#define GATT_PRIMARY_SERVICE_UUID                   0x2800

#define TYPE_CHAR      0x2803
#define TYPE_CFG       0x2902
#define TYPE_INFO      0x2901
#define TYPE_xRpRef    0x2907
#define TYPE_RpRef     0x2908
#define TYPE_INC       0x2802
#define UUID16_FORMAT  0xff

#define SOFTWARE_INFO "SV5.3.2"
#define MANU_INFO     "MINDMOTION Bluetooth"
char DeviceInfo[24] =  "MINDMOTION";  /*max len is 24 bytes*/

u16 cur_notifyhandle = 0x12;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

/********************************************************************************************************
**function: *getDeviceInfoData
**@brief    This function is used to get device information
**
**@param    len £º Data length
**
**@return   (u8 *)DeviceInfo  :  Device Information
********************************************************************************************************/
u8 *getDeviceInfoData(u8 *len)
{
  *len = sizeof(DeviceInfo);
  return (u8 *)DeviceInfo;
}

/********************************************************************************************************
**function: updateDeviceInfoData
**@brief    This function updates BLE device information
**
**@param    name £º Device information that needs to be updated
**
**@param    len £º Data length
**
**@return   None.
********************************************************************************************************/
void updateDeviceInfoData(u8 *name, u8 len)
{
  memset(DeviceInfo, 0, 24);
  memcpy(DeviceInfo, name, len);
  ble_set_name(name, len);
}

/**********************************************************************************
                 *****DataBase****

01 - 06  GAP (Primary service) 0x1800
  03:04  name
07 - 0f  Device Info (Primary service) 0x180a
  0a:0b  firmware version
  0e:0f  software version
10 - 19  LED service (Primary service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
  11:12  6E400003-B5A3-F393-E0A9-E50E24DCCA9E(0x04)  RxNotify
  13     cfg
  14:15  6E400002-B5A3-F393-E0A9-E50E24DCCA9E(0x0C)  Tx
  16     cfg
  17:18  6E400004-B5A3-F393-E0A9-E50E24DCCA9E(0x0A)  BaudRate
  19     0x2901  info
************************************************************************************/

typedef struct ble_character16
{
  u16 type16;          //type2
  u16 handle_rec;      //handle
  u8  characterInfo[5];//property1 - handle2 - uuid2
  u8  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
} BLE_CHAR;

typedef struct ble_UUID128
{
  u8  uuid128[16];//uuid128 string: little endian
} BLE_UUID128;

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] =
{
// ======  gatt =====  Do NOT Change!!
  {TYPE_CHAR, 0x03, ATT_CHAR_PROP_RD, 0x04, 0, 0x00, 0x2a, UUID16_FORMAT}, //name
  //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!
  {TYPE_CHAR, 0x08, ATT_CHAR_PROP_RD, 0x09, 0, 0x29, 0x2a, UUID16_FORMAT}, //manufacture
  {TYPE_CHAR, 0x0a, ATT_CHAR_PROP_RD, 0x0b, 0, 0x26, 0x2a, UUID16_FORMAT}, //firmware version
  {TYPE_CHAR, 0x0e, ATT_CHAR_PROP_RD, 0x0f, 0, 0x28, 0x2a, UUID16_FORMAT}, //sw version

// ======  User service or other services added here =====  User defined
  {TYPE_CHAR, 0x11, ATT_CHAR_PROP_NTF,                  0x12, 0, 0, 0, 1/*uuid128-idx1*/ }, //RxNotify
  {TYPE_CFG, 0x13, ATT_CHAR_PROP_RD | ATT_CHAR_PROP_W}, //cfg
  {TYPE_CHAR, 0x14, ATT_CHAR_PROP_W | ATT_CHAR_PROP_W_NORSP,              0x15, 0, 0, 0, 2/*uuid128-idx2*/ }, //Tx
  {TYPE_CHAR, 0x17, ATT_CHAR_PROP_W | ATT_CHAR_PROP_RD,                   0x18, 0, 0, 0, 3/*uuid128-idx3*/ }, //BaudRate
  {TYPE_INFO, 0x19, ATT_CHAR_PROP_RD}, //description,"BaudRate"
};

const BLE_UUID128 AttUuid128List[] =
{
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 1, 0, 0x40, 0x6e}, //idx0,little endian, service uuid
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 3, 0, 0x40, 0x6e}, //idx1,little endian, RxNotify
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 2, 0, 0x40, 0x6e}, //idx2,little endian, Tx
  {0x9e, 0xca, 0x0dc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 4, 0, 0x40, 0x6e}, //idx3,little endian, BaudRate
};

/********************************************************************************************************
**function: GetCharListDim
**@brief    This function is used to query the number of lists
**
**@param    None.
**
**@return   sizeof(AttCharList) / sizeof(AttCharList[0]) :  Number of lists
********************************************************************************************************/
u8 GetCharListDim(void)
{
  return sizeof(AttCharList) / sizeof(AttCharList[0]);
}

///STEP1:Service declare
// read by type request handle, primary service declare implementation
/********************************************************************************************************
**function: att_server_rdByGrType
**@brief    This function maps the UUID to the service,The handle value of the service
**          should be set from small to large, otherwise an error will occur,
**@param    pdu_type  :  PDU type parameters. Directly reference the corresponding parameter
**          in the callback function att_server_rdByGrType
**@param    attOpcode :  corresponding value of attOpcode operation
**
**@param    st_hd     :  Start handle corresponding to the service
**
**@param    end_hd    :  End handle corresponding to the service
**
**@param    att_type  :  ATT type parameter
**
**@return   None.
********************************************************************************************************/
void att_server_rdByGrType(u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type)
{
//!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!

  if ((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1)) //hard code for device info service
  {
    att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
    //apply user defined (device info)service example
    //{
    //    u8 t[] = {0xa,0x18};
    //    att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);
    //}
    return;
  }

  //hard code
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
///write response, data coming....
/********************************************************************************************************
**function: ser_write_rsp
**@brief    This function is the reply function after the BLE device receives the write request
**
**@param    pdu_type  :  PDU type parameters. Directly reference the corresponding parameter
**          in the callback function att_server_rdByGrType
**@param    attOpcode :  corresponding value of attOpcode operation
**
**@param    att_hd    :  BLE service handle value
**
**@param    attValue  :  Received data
**
**@param    valueLen_w :  Data length
**
**@return   None.
********************************************************************************************************/
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/,
                   u16 att_hd, u8 *attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{

  switch (att_hd)
  {
  case 0x18://BaudRate
    BaudRate = ((*(attValue + 2)) << 16) | ((*(attValue + 1)) << 8) | (*attValue);
    ser_write_rsp_pkt(pdu_type);
    ChangeBaudRate();
    break;
  case 0x15://Tx
#ifdef USE_UART
    moduleOutData(attValue, valueLen_w);
#endif
  case 0x12://cmd
  case 0x13://cfg
    ser_write_rsp_pkt(pdu_type);  /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
    break;
  default:
    att_notFd(pdu_type, attOpcode, att_hd);    /*the default response, also for the purpose of error robust */
    break;
  }
}

///STEP2.1:Queued Writes data if any
/********************************************************************************************************
**function: ser_prepare_write
**@brief    This function calls the function for sending long data to the phone
**
**@param    handle     :  BLE service handle value
**
**@param    attValue   :  Received data
**
**@param    valueLen_w :  Data length
**
**@param    att_offset :  Address offset
**
**@return   None.
********************************************************************************************************/
void ser_prepare_write(u16 handle, u8 *attValue, u16 attValueLen, u16 att_offset)//user's call back api
{
  //queued data:offset + data(size)
  //when ser_execute_write() is invoked, means end of queue write.
  //to do
}

/********************************************************************************************************
**function: ser_execute_write
**@brief    This function calls the function at the end of sending long data to the phone
**
**@param    None.
**
**@return   None.
********************************************************************************************************/
void ser_execute_write(void)//user's call back api
{
  //end of queued writes
  //to do...
}

///STEP3:Read data
//// read response
/********************************************************************************************************
**function: server_rd_rsp
**@brief    This function is a write function for BLE devices
**
**@param    handle     :  BLE service handle value
**
**@param    attValue   :  Received data
**
**@param    valueLen_w :  Data length
**
**@param    att_offset :  Address offset
**
**@return   None.
********************************************************************************************************/
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
  u8 tab[3];
  switch (attHandle) //hard code
  {
  case 0x04: //MANU_INFO
    att_server_rd(pdu_type, attOpcode, attHandle, "MINDMOTION", 10);  //ble lib build version
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
    tab[0] = (BaudRate & 0xff0000) >> 16;
    tab[1] = (BaudRate & 0xff00) >> 8;
    tab[2] = BaudRate;
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

//return 1 means found
/********************************************************************************************************
**function: GetPrimaryServiceHandle
**@brief    This function determines the handle value of the service
**
**@param    hd_start   :   Start handle corresponding to the service
**
**@param    hd_end     :   End handle corresponding to the service
**
**@param    uuid16     :   UUID
**
**@param    hd_start_r :   Start record handle
**
**@param    hd_end_r   :   End record handle
**
**@return   return 1 means found   / return 0 means not found
********************************************************************************************************/
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,
                            unsigned short *hd_start_r, unsigned short *hd_end_r)
{
// example
//    if((uuid16 == 0x1812) && (hd_start <= 0x19))// MUST keep match with the information save in function  att_server_rdByGrType(...)
//    {
//        *hd_start_r = 0x19;
//        *hd_end_r = 0x2a;
//        return 1;
//    }

  return 0;
}

/********************************************************************************************************
**function: gatt_user_send_notify_data_callback
**@brief    This callback function can be used to actively send data to the Bluetooth module
**          The protocol stack will callback (asynchronously) this function when the system allows it. It must not block! !!
**@param    None.
**
**@return   None.
********************************************************************************************************/
void gatt_user_send_notify_data_callback(void)
{
  //to do if any ...
  //add user sending data notify operation ....
//    sconn_notifydata(pld_adv,len_adv);
}

/********************************************************************************************************
**function: getsoftwareversion
**@brief    Get the version information for the BLE device
**
**@param    None.
**
**@return   None.
********************************************************************************************************/
u8 *getsoftwareversion(void)
{
  return (u8 *)SOFTWARE_INFO;
}

static unsigned char gConnectedFlag = 0;
/********************************************************************************************************
**function: GetConnectedStatus
**@brief    Gets the connection status of the BLE device
**
**@param    None.
**
**@return   gConnectedFlag £º connect flag      1 £ºconnect successful  0 £ºconnect failed
********************************************************************************************************/
char GetConnectedStatus(void)
{
  return gConnectedFlag;
}

/********************************************************************************************************
**function: server_blob_rd_rsp
**@brief    This function is called when the phone reads long characters
**
**@param    attOpcode   :   corresponding value of attOpcode operation
**
**@param    attHandle   :   BLE service handle value
**
**@param    dataHdrP    :
**
**@param    offset      :   Address offset
**
**@return
********************************************************************************************************/
void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP, u16 offset)
{
//    u16 size;
//
//    if(attHandle == 0x22)//hid report map
//    {
//        if(offset + 0x16 <= HID_MAP_SIZE)size = 0x16;
//        else size = HID_MAP_SIZE - offset;
//        att_server_rd( dataHdrP, attOpcode, attHandle, (u8*)(Hid_map+offset), size);
//
//        return;
//    }
//
  att_notFd(dataHdrP, attOpcode, attHandle); /*the default response, also for the purpose of error robust */
}

/********************************************************************************************************
**function: ConnectStausUpdate
**@brief    Update the connection status of BLE devices
**
**@param    IsConnectedFlag     :  indicates the connection status
**
**@return   None.
********************************************************************************************************/
void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
  //[IsConnectedFlag] indicates the connection status

  if (IsConnectedFlag != gConnectedFlag)
  {
    gConnectedFlag = IsConnectedFlag;
#ifdef USE_UART
    if (gConnectedFlag)
    {
      SleepStop = 1;
      CanNotifyFlag = 1;
    }
    else
    {
      SleepStop = 2;
    }
#endif
  }
}

/********************************************************************************************************
**function: UpdateLEDValueAll
**@brief    This function is to update the data of PWN, this function will be called in lib
**          This file is the calling file for Bluetooth control RGB led.
**@param    None.
**
**@return   None.
********************************************************************************************************/
void UpdateLEDValueAll(void) //porting function
{
  //to do ......
}

unsigned char aes_encrypt_HW(unsigned char *_data, unsigned char *_key)
{
#ifndef AES_HW_SUPPORT
	
	return 0; //HW AES NOT supported

#else  //HW AES supported
	unsigned int tmp = 0;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, ENABLE); //AES CLK enable
	
	AES->CR = 0x00;	
	AES->CR|=0x03<<20;

	AES->KEYR3 = (u32)(_key[0] << 24)|(u32)(_key[1] << 16)|(u32)(_key[2] << 8)|(u32)(_key[3] << 0);
	AES->KEYR2 = (u32)(_key[4] << 24)|(u32)(_key[5] << 16)|(u32)(_key[6] << 8)|(u32)(_key[7] << 0);
	AES->KEYR1 = (u32)(_key[8] << 24)|(u32)(_key[9] << 16)|(u32)(_key[10] << 8)|(u32)(_key[11] << 0);
	AES->KEYR0 = (u32)(_key[12] << 24)|(u32)(_key[13] << 16)|(u32)(_key[14] << 8)|(u32)(_key[15] << 0);	
	
	AES->CR |= 0x01;  //start encryption
	
	AES->DINR = (u32)(_data[0] << 24)|(u32)(_data[1] << 16)|(u32)(_data[2] << 8)|(u32)(_data[3] << 0);
	AES->DINR = (u32)(_data[4] << 24)|(u32)(_data[5] << 16)|(u32)(_data[6] << 8)|(u32)(_data[7] << 0);
	AES->DINR = (u32)(_data[8] << 24)|(u32)(_data[9] << 16)|(u32)(_data[10] << 8)|(u32)(_data[11] << 0);
	AES->DINR = (u32)(_data[12] << 24)|(u32)(_data[13] << 16)|(u32)(_data[14] << 8)|(u32)(_data[15] << 0);	
	
	//²éÑ¯Ä£Ê½
	while(1)
	{
			if((AES->SR & 0x01) )
			{
					AES->CR |=  0x1<<7; //clear ccf flag
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

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, DISABLE); //AES CLK disable

	return 1;// HW supported value
#endif
}
#endif



