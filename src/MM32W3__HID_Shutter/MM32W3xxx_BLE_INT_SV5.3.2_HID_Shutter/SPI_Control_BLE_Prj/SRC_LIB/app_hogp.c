/*
    Copyright (c) 2017 Macrogiga Electronics Co., Ltd.

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

#ifdef BLE_PAIR_SUPPORT

//#define AES_HW_SUPPORT  /*AES Hardware implemntation support*/

#include <string.h>
#include "HAL_conf.h"
#include "iwdg.h"
#include "mg_api.h"

extern void att_ErrorFd_eCode(u8 pdu_type, u8 attOpcode, u16 attHd , u8 errorCode);
extern  unsigned char StartEncryption;
static unsigned char gConnectedFlag=0;
u8 CanNotifyFlag = 0;
extern bool  key_shutter;
/// Characteristic Properties Bit
#define ATT_CHAR_PROP_RD                            0x02
#define ATT_CHAR_PROP_W_NORSP                       0x04
#define ATT_CHAR_PROP_W                             0x08
#define ATT_CHAR_PROP_NTF                						0x10
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
#define MANU_INFO     "MindMotion Bluetooth"
#define DeviceInfo    "MindMotion-Shutter"  /*max len is 24 bytes*/
#define HID_MAP_SIZE (sizeof(Hid_map))
u16 cur_notifyhandle = 0x1e;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);

u8* getDeviceInfoData(u8* len)
{    
    *len = sizeof(DeviceInfo);
    return (u8*)DeviceInfo;
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

typedef struct ble_character16{
    u16 type16;          //type2
    u16 handle_rec;      //handle
    u8  characterInfo[5];//property1 - handle2 - uuid2
    u8  uuid128_idx;     //0xff means uuid16,other is idx of uuid128
}BLE_CHAR;

typedef struct ble_UUID128{    
    u8  uuid128[16];//uuid128 string: little endian
}BLE_UUID128;

//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] = {
// ======  gatt =====  Do NOT Change!!
    {TYPE_CHAR,0x03,ATT_CHAR_PROP_RD, 0x04,0,0x00,0x2a,UUID16_FORMAT},//name
    //05-06 reserved
// ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR,0x08,ATT_CHAR_PROP_RD, 0x09,0,0x29,0x2a,UUID16_FORMAT},//manufacture
    {TYPE_CHAR,0x0a,ATT_CHAR_PROP_RD, 0x0b,0,0x26,0x2a,UUID16_FORMAT},//firmware version
    {TYPE_CHAR,0x0c,ATT_CHAR_PROP_RD, 0x0d,0,0x50,0x2a,UUID16_FORMAT},//PnPID
    {TYPE_CHAR,0x0e,ATT_CHAR_PROP_RD, 0x0f,0,0x28,0x2a,UUID16_FORMAT},//sw version    
    
// ======  HID =====
    //    {TYPE_INC, 0x1a,},//include, hard code!!!
    {TYPE_INC, 0x001a,0x2b,0x00, 0x2d,0x00, 0x0a,0xa0}, //hard code uuid-0aa0, I just share the memory
    {TYPE_CHAR,0x1B,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W_NORSP,0x1c,0, 0x4e,0x2a,UUID16_FORMAT},//protocol mode
    {TYPE_CHAR,0x1d,ATT_CHAR_PROP_W|ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x1e,0,0x4d,0x2a,UUID16_FORMAT},//report
    {TYPE_CFG, 0x1f,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg
    {TYPE_RpRef,0x20,ATT_CHAR_PROP_RD},//Report Reference
    
    {TYPE_CHAR,0x21,ATT_CHAR_PROP_RD, 0x22,0,0x4b,0x2a,UUID16_FORMAT},//report map
    {TYPE_xRpRef,0x23,ATT_CHAR_PROP_RD},//External Report Reference
    
    {TYPE_CHAR,0x24,ATT_CHAR_PROP_W|ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF,0x25,0,0x33,0x2a,UUID16_FORMAT},//boot mouse input report
    {TYPE_CFG, 0x26,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg
    
    {TYPE_CHAR,0x27,ATT_CHAR_PROP_RD,                      0x28,0,0x4a,0x2a,UUID16_FORMAT},//hid info
    {TYPE_CHAR,0x29,ATT_CHAR_PROP_W_NORSP,                 0x2a,0,0x4c,0x2a,UUID16_FORMAT},//hid control point

// ======  0xa00a  ======    include uuid
    {TYPE_CHAR,0x2c,ATT_CHAR_PROP_RD, 0x2d,0,0x0b,0x0a,UUID16_FORMAT},//
    
// ======  battery service  ======    
    {TYPE_CHAR,0x2f,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF,    0x30,0,0x19,0x29,UUID16_FORMAT},//battery level
    {TYPE_CFG, 0x31,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg
    
// ======  scan prameter service  ======        
    {TYPE_CHAR,0x33,ATT_CHAR_PROP_W_NORSP,    0x34,0,0x4f,0x2a,UUID16_FORMAT},//Scan Interval Window
    {TYPE_CHAR,0x35,ATT_CHAR_PROP_NTF,    0x36,0,0x31,0x2a,UUID16_FORMAT},//Scan Refresh
    {TYPE_CFG, 0x37,ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},//cfg
};

const BLE_UUID128 AttUuid128List[] = {
    {{0}}, //empty
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
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
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
void att_server_rdByGrType( u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type )
{
 //!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!
                     
    if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1))//hard code for device info service
    {
        //att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
        //GAP Device Name
        u8 t[] = {0x00,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1,0x6,(u8*)(t),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07))//hard code for device info service
    {
        //apply user defined (device info)service example
        u8 t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);
        return;
    }
    
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        u8 hid[2] = {0x12,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x19,0x2a,(u8*)(hid),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x2b)) //usr's service
    {
        u8 inc[2] = {0x0a,0xa0};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x2b,0x2d,(u8*)(inc),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x2e)) //usr's service
    {
        u8 battery[2] = {0x0f,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x2e,0x31,(u8*)(battery),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x32)) //usr's service
    {
        u8 scanp[2] = {0x13,0x18};//Scan parameter
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x32,0x37,(u8*)(scanp),2);
        return;
    }

    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
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
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{   
    switch(att_hd)
    {        
        case 0x1f://cfg
            CanNotifyFlag = 1;
        case 0x26:
        case 0x31:
        case 0x37:
            ser_write_rsp_pkt(pdu_type); /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
            break;
        
        default:
            att_notFd( pdu_type, attOpcode, att_hd );/*the default response, also for the purpose of error robust */
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
void ser_prepare_write(u16 handle, u8* attValue, u16 attValueLen, u16 att_offset)//user's call back api 
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

const u8 Hid_map[]= {
    //first Id
    0x05, 0x01,//standard keyboard    Usage Page(global)Generic Desktop Controls 
    0x09, 0x06,//                     Usage (local) Keyboard
    0xA1, 0x01, //collection((Application)
	
    0x85, 0x01,//report ID
    0x05, 0x07, //Keyboard/Keypad 
    0x19, 0xE0, //Usage Minimum
    0x29, 0xE7, //Usage Maximum
    0x15, 0,    //Logical Minimum
    0x25, 1,    //Logical Maximum
    
    0x75, 1,    //Report Size(1 bit)
    0x95, 8,    //Report Count(8)
    0x81, 2,    //input(2) - variable absolute data
    
    0x95, 1,    //Report Count(1)
    0x75, 0x08, //Report Size(8 bit)
    0x81, 3,    //input(3) - variable absolute constant, for OEM data usage
    
    0x95, 5,    //Report Count(5)
    0x75, 1,    //Report Size(1 bit)
    5, 8,       //Usage Page(global)LED
    0x19, 1,    //Usage Minimum
    0x29, 5,    //Usage Maximum	
    0x91, 2,    //output(2) - variable absolute data
    
    0x95, 1,    //Report Count(1)
    0x75, 3,    //Report Size(3 bit)
    0x91, 3,    //output(3) - variable absolute constant
    
    0x95, 6,    //Report Count(6)
    0x75, 8,    //Report Size(8 bit)
    0x15, 0,    //Logical Minimum
    0x25, 0xff, //Logical Maximum
    5, 7,       //Keyboard/Keypad 
    0x19, 0,    //Usage Minimum
    0x29, 0xff, //Usage Maximum
    0x81, 0,    //input(0) - array absolute data
    
    0xc0,             //END collection
    
// Second ID    
    
    0x05, 0x0c, //Consumer 
    0x09, 0x01, //Consumer Control  
    
    0xA1, 0x01, //collection
    
    0x85, 0x02,//report ID
    
    0x15, 0x00,//Logical Minimum
    0x25, 0x01,//Logical Maximum
    0x75, 0x01,//Report Size(1 bit)
    0x95, 0x1e,//Report Count(30)
    
    //first item start
    0x09, 0x30, //power    
    0x09, 0xb0, //play
    0x09, 0xb1, //pause
    0x09, 0xb2, //record
    0x09, 0xb3, //FF
    0x09, 0xb4, //FB    
    0x09, 0xb5, //next     
    0x09, 0xb6, //pre
    
    0x09, 0xb8, //reject 
    0x09, 0xb9, //random play    
    0x09, 0xe2, //Mute    
    0x09, 0xe9, //V+
    0x09, 0xea, //V- 
    0x09, 0x95, //help    
    0x0a, 0x24, 0x02, //AC back
    0x0a, 0x25, 0x02, //AC forward
    
    0x0a, 0x26, 0x02, //AC stop
    0x0a, 0x27, 0x02, //AC refresh
    0x0a, 0x21, 0x02, //AC search
    0x0a, 0x2a, 0x02, //AC bookmarks
    0x0a, 0x23, 0x02, //AC Home
    0x0a, 0x8a, 0x01, //AL email reader    
    0x0a, 0x83, 0x01, //AL consumer Contrl Configuration
    0x0a, 0x94, 0x01, //AL browser
    
    0x0a, 0x92, 0x01, //AL calculater
    0x0a, 0x09, 0x02, //AC property
    0x0a, 0x7f, 0x02, //AC view clock
    0x0a, 0x33, 0x02, //scroll up
    0x0a, 0x34, 0x02, //scroll down
    0x0a, 0x1F, 0x02, //AC Find
    
//end
    0x81, 0x02,  //input(2) - variable absolute data
    
    0x95, 0x01,  //Report Count(30)
    0x75, 0x02,  //Report Size(2 bit)
    0x81, 0x03,  //input(3) - variable absolute constant
    
    0xc0,                  //end collection    
}; 
 
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
    u8  d_len;
    u8* ble_name = getDeviceInfoData(&d_len);
    
    switch(attHandle) //hard code
    {
        case 0x04: //GAP name
            att_server_rd( pdu_type, attOpcode, attHandle, ble_name, d_len);
            break;
        
        case 0x09: //MANU_INFO
            //att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
            att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
            break;
        
        case 0x0b: //FIRMWARE_INFO
        {            
            //do NOT modify this code!!!
            att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(),strlen((const char*)GetFirmwareInfo()));
            break;
        }
        
        case 0x0d: //PnPID
        {
            u8 t[7] = {0,0,0,0,0,0,0};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 7);
            break;
        }
        
        case 0x0f://SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;
        
        case 0x20://Report Reference
            {
            u8 t[2] = {0,1};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
            }
            break;
        
        case 0x22://report map
            if(!StartEncryption)
            {
                att_ErrorFd_eCode(pdu_type, attOpcode, attHandle,0x0F); //pair needed
            }
            else
            {
                att_server_rd( pdu_type, attOpcode, attHandle, (u8*)Hid_map, 0x16);
            }
            break;
        
        case 0x23://external report ref
            {
            u8 t[2] = {0x0b,0x0a};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
            }
            break;

        case 0x28://hid info            
         {
            u8 t[4] = {0xdb,0xfa,0x5a,0x02};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 4);
         }
            break;
         
        case 0x30://battery level
        {
            u8 t[2] = {90};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 1);
            break;            
        }
        
        case 0x1f://cfg
        case 0x26://cfg
        case 0x31://cfg
        case 0x37://cfg
           {
            u8 t[2] = {0,0};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
           }
            break;            
        
        default:
            att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
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
                            unsigned short* hd_start_r,unsigned short* hd_end_r)
{
// example
    if((uuid16 == 0x1812) && (hd_start <= 0x19))// MUST keep match with the information save in function  att_server_rdByGrType(...)
    {
        *hd_start_r = 0x19;
        *hd_end_r = 0x2a;
        return 1;
    }
    
    return 0;
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
void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset)
{
    u16 size;
    
    if(attHandle == 0x22)//hid report map
    {
        if(offset + 0x16 <= HID_MAP_SIZE)size = 0x16;
        else size = HID_MAP_SIZE - offset;
        att_server_rd( dataHdrP, attOpcode, attHandle, (u8*)(Hid_map+offset), size);
        
        return;
    }
    
    att_notFd( dataHdrP, attOpcode, attHandle);/*the default response, also for the purpose of error robust */
}

/********************************************************************************************************
**function: NotifyApplePhoto
**@brief    This function is apple photo hid photo capture, hard code
**
**@param    None.
**
**@return
********************************************************************************************************/
u8 NotifyApplePhoto(void)//apple photo hid photo capture, hard code
{
    u8 Keyarray[5] = {2,0,8,0,0}; //VolUp,hard code
    
    sconn_notifydata(Keyarray,5);
    
    Keyarray[2] = 0;
    sconn_notifydata(Keyarray,5);
    
    return 1;
}

/********************************************************************************************************
**function: NotifyKey
**@brief    This function is hid standard keyboard key, hardcode
**
**@param    KeyIdx
**
**@return		None.
********************************************************************************************************/
u8 NotifyKey(u8 KeyIdx)//hid standard keyboard key, hardcode
{
    u8 Keyarray[9] = {1,0,0,0,0,0,0,0,0};//0xa1
    Keyarray[3] = KeyIdx;
    sconn_notifydata(Keyarray,9);
    Keyarray[3] = 0;
    sconn_notifydata(Keyarray,9);
    return 1;
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
#if 0    //auto send data notify operation ....
    static u8 count = 0;
    if((count > 30) || CanNotifyFlag) //delay usage
    {
        count = 0;
        CanNotifyFlag = 0;
        
        NotifyKey(0x28);
        NotifyApplePhoto();
    }
    count++;
#else
	if(key_shutter==TRUE)
	{
				key_shutter=FALSE;
	      NotifyKey(0x28);
        NotifyApplePhoto();
		
	}
#endif
}

/********************************************************************************************************
**function: *getDeviceInfoData
**@brief    This function is used to get device information
**
**@param    len £º Data length
**
**@return   (u8 *)DeviceInfo  :  Device Information
********************************************************************************************************/
u8* getsoftwareversion(void)
{
    return (u8*)SOFTWARE_INFO;
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
void updateDeviceInfoData(u8* name, u8 len)
{
  memset(DeviceInfo, 0, 24);
  memcpy(DeviceInfo, name, len);
  ble_set_name(name, len);
} 

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
**function: aes_encrypt_HW
**@brief    This function is AES encryption function
**
**@param    None.
**
**@return   gConnectedFlag £º connect flag      1 £ºconnect successful  0 £ºconnect failed
********************************************************************************************************/
//---- MindMotion HardWare AES implemenation -----
unsigned char aes_encrypt_HW(unsigned char *painText128bitBE,unsigned char *key128bitBE); //porting api, returns zero means not supported

unsigned char aes_encrypt_HW(unsigned char *_data, unsigned char *_key)
{
    

#ifndef AES_HW_SUPPORT    //HW AES NOT supported
    return 0; //not supported

#else                     //HW AES supported
		unsigned int tmp;
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
//    _text[0] = AES->DOUTR;  //encrypted output data
//    _text[1] = AES->DOUTR;
//    _text[2] = AES->DOUTR;
//    _text[3] = AES->DOUTR;
	
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

/********************************************************************************************************
**function: ConnectStausUpdate
**@brief    This function is about BLE connection status update function
**@param    IsConnectedFlag.  1£ºconnect ; 0£ºdisconnect
**
**@return   None.
********************************************************************************************************/
void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    
    if(!IsConnectedFlag)
    CanNotifyFlag = IsConnectedFlag; //disconnected, so can NOT notify data

    //[IsConnectedFlag] indicates the connection status
    gConnectedFlag = IsConnectedFlag;
}

/********************************************************************************************************
**function: UsrProcCallback
**@brief    This function is a callback function. This function will be called
**          every time a Bluetooth interrupt is entered.
**          and the data is processed in this function. You can do something in this function
**@param    None.
**
**@return   None.
********************************************************************************************************/
void UsrProcCallback(void) //porting api
{
    IWDG_ReloadCounter();
    
    if(gConnectedFlag){//connected
			//to do something
    }else{
			//to do something
    }
}

#endif
