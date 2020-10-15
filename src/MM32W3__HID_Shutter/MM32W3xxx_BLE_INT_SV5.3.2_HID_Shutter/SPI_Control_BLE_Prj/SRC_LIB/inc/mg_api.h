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
/*lib release: v5.2.5*/

#ifndef _MG_API_H_
#define _MG_API_H_

//TX power
#define TXPWR_3DBM 0x48
#define TXPWR_0DBM 0x43
#define	TXPWR__3DBM	 64
#define	TXPWR__6DBM	 61
#define	TXPWR__8DBM	 58
#define	TXPWR__15DBM 48


// ATT Error Codes
#define ATT_ERR_INVALID_HANDLE           0x01
#define ATT_ERR_READ_NOT_PERMITTED       0x02
#define ATT_ERR_WRITE_NOT_PERMITTED      0x03
#define ATT_ERR_INVALID_PDU              0x04
#define ATT_ERR_INSUFFICIENT_AUTHEN      0x05
#define ATT_ERR_UNSUPPORTED_REQ          0x06
#define ATT_ERR_INVALID_OFFSET           0x07
#define ATT_ERR_INSUFFICIENT_AUTHOR      0x08
#define ATT_ERR_PREPARE_QUEUE_FULL       0x09
#define ATT_ERR_ATTR_NOT_FOUND           0x0a
#define ATT_ERR_ATTR_NOT_LONG            0x0b
#define ATT_ERR_INSUFFICIENT_KEY_SIZE    0x0c
#define ATT_ERR_INVALID_VALUE_SIZE       0x0d
#define ATT_ERR_UNLIKELY                 0x0e
#define ATT_ERR_INSUFFICIENT_ENCRYPT     0x0f
#define ATT_ERR_UNSUPPORTED_GRP_TYPE     0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES   0x11

//adv header type
#define ADV_HDR_TYPE_PUBLIC_IND         0x00
#define ADV_HDR_TYPE_RANDOM_IND         0x80
#define ADV_HDR_TYPE_PUBLIC_NONCONN_IND  0x02
#define ADV_HDR_TYPE_RANDOM_NONCONN_IND  0x82

///////////////////////////lib provided APIs//////////////////////////////////////

//Function: radio_initBle
//Parameters: txpwr - input, txpower;   addr - output, BLE device address
//return: None
void radio_initBle(unsigned char txpwr, unsigned char** addr/*out*/);  //init ble mode, should be called first after spi initialization

//Function: radio_initBle_TO
//Parameters: txpwr - input, txpower;   addr - output, BLE device address;   ms_timeout - timeout for BLE initialization, recommend value 10~50
//return: 0 - fail;  none 0 - success
unsigned char radio_initBle_TO(unsigned char txpwr, unsigned char** addr, unsigned short ms_timeout);

//Function: radio_initBle_recover
//Parameters: txpwr - input, txpower;   addr - output, BLE device address
//return: None
void radio_initBle_recover(unsigned char txpwr, unsigned char** addr);

//Function: radio_setCal_nonBlocking
//Parameters: nonblocking - 0: blocking;  1: non blocking
//return: none
void radio_setCal_nonBlocking(unsigned nonblocking);

//Function: radio_standby
//this function is to set rf to standby mode, I ~ 3uA
//Parameters: none
//return: None
//called in UsrProcCallback or when ble_run_interrupt_McuCanSleep()>0
void radio_standby(void);

//Function: radio_resume
//this function is to recover rf from standby mode
//Parameters: none
//return: None
void radio_resume(void);
    
//Function: radio_setXtal
//this function is to config the params of xtal
//Parameters: xoib:0~f, xocc:0
//return: None
void radio_setXtal(unsigned char xoib, unsigned char xocc);

//Function: radio_setRxGain
//this function is to config the params of RX
//Parameters: lna_gain:0,5,6,7, preambe_th: 0x20 when lna_gain=0, 0x38 when lna_gain=5,6,7
//return: 0-input param error, 1-ok
unsigned char radio_setRxGain(unsigned char lna_gain, unsigned char preamble_th);

//Function: ble_run
//Parameters: interv_adv - advertise packet interval, unit 0.625ms
//return: None
//Remarks: never reurn!!!
void ble_run(unsigned short interv_adv);

//Function: ble_set_adv_data
//Parameters: adv - advertise packet payload; len - payload len
//return: None
void ble_set_adv_data(unsigned char* adv, unsigned char len);

//Function: ble_set_adv_rsp_data
//Parameters: rsp - advertise response payload; len - payload len
//return: None
void ble_set_adv_rsp_data(unsigned char* rsp, unsigned char len);

//Function: ble_set_name
//this function IS available when using default scan response data
//Parameters: name - device name; len - name len
//return: None
void ble_set_name(unsigned char* name,unsigned char len);

//Function: ble_set_adv_type
//Parameters: type - advertisement type, 0-adv_ind, 2-adv_nonconn_ind. default 0x80
//                   addr type,      0x80 - RANDOM, 0x00 - PUBLIC
//return: None
void ble_set_adv_type(unsigned char type);

//Function: ble_set_interval
//Parameters: interval - advertisement interval, unit 0.625ms
//return: None
void ble_set_interval(unsigned short interval);

//Function: ble_set_wakeupdly
//Parameters: counter - wake up delay time, unit 16uS
//return: 1
unsigned char ble_set_wakeupdly(unsigned short counter);

//Function: ble_set_adv_enableFlag
//this function is to enable/disable ble adv
//Parameters: sEnableFlag - 0 to disable adv, 1 to enable adv
//return: None
void ble_set_adv_enableFlag(char sEnableFlag);

//Function: ble_set_role
//this function is to set ble role to peripheral(0) or central(1), by default ble role is peripheral(0)
//Parameters: role_new - 0 peripheral, 1 central
//            scan_window - scan window for central rol. range: 0x0004~0x4000 (2.5ms ~ 10.24s)
//return: 0 - fail, 1 - success
unsigned char ble_set_role(unsigned char role_new, unsigned short scan_window);

//Function: ble_disconnect
//this function is to disconnected the ble connection
//Parameters: none
//return: None
void ble_disconnect(void);

unsigned char* GetFirmwareInfo(void); //such as "FVxx.2.0.2mmx"
unsigned char* get_ble_version(void); //such as "MG_BLE_LIB_V1.0"
unsigned char GetRssiData(void); //only valid after receive a packet

void att_notFd(unsigned char pdu_type, unsigned char attOpcode, unsigned short attHd );
void att_ErrorFd_eCode(unsigned char pdu_type, unsigned char attOpcode, unsigned short attHd, unsigned char errorCode);
void att_server_rdByGrTypeRspDeviceInfo(unsigned char pdu_type);
void att_server_rdByGrTypeRspPrimaryService(unsigned char pdu_type, unsigned short start_hd, unsigned short end_hd, unsigned char*uuid, unsigned char uuidlen);
void att_server_rd( unsigned char pdu_type, unsigned char attOpcode, unsigned short att_hd, unsigned char* attValue, unsigned char datalen );

void ser_write_rsp_pkt(unsigned char pdu_type);

unsigned char sconn_notifydata(unsigned char* data, unsigned char len);//returns data size has been sent, ******* user's safe API *********
unsigned char sconn_indicationdata(unsigned char* data, unsigned char len);
//Interval Min (2 octets)Defines minimum value for the connection event interval in the following manner:
//connIntervalMin = Interval Min * 1.25 ms. Interval Min range: 6 to 3200
//Interval Max (2 octets)Defines maximum value for the connection event interval in the following manner:
//connIntervalMax = Interval Max * 1.25 ms. Interval Max range: 6 to 3200
//Interval Max shall be equal to or greater than the Interval Min.
//slaveLatency shall have a valuein the range of 0 to ((connSupervisionTimeout / (connIntervalMax*2)) -1).
//slaveLatency shall be less than 500.
//TimeoutMultiplier (2 octets) Defines connection timeout parameter in the following manner:
//connSupervisionTimeout = Timeout Multiplier * 10 ms. The Timeout Multiplier range: 10 to 3200.
void SIG_ConnParaUpdateReq(unsigned short IntervalMin, unsigned short IntervalMax, unsigned short slaveLatency, unsigned short TimeoutMultiplier);
unsigned short sconn_GetConnInterval(void);//get current used interval in the unit of 1.25ms

//Get current (or the latest) connected master device's MAC
//returns mac(6 Bytes, Little-Endian format) and the type(MacType, 0 means public type, others mean random type)
unsigned char* GetMasterDeviceMac(unsigned char* MacType);

///////////////////////////PAIR APIs/////////////////////////////////
void SetLePinCode(unsigned char *PinCode/*6 0~9 digitals*/);

//Get current connected device's long term KEY's info(EDIV)
//returns u8* EDivData /*2 Bytes*/ (encrypted)
//        newFlag: 1 means new paired device's info, 0 means old paired device's info.
//Remarks: 
//  1. This function shall be invoked when [StartEncryption == 1].
//  2. This function is ONLY supported in pairing cases.
unsigned char* GetLTKInfo(unsigned char* newFlag);

//security manager module request for pair
//Remarks:
//  1. This function shall be invoked when [connected status == 1].
//  2. This function is ONLY supported in pairing cases.
void s_llSmSecurityReq(void);




///////////////////////////OTA APIs/////////////////////////////////
//return: 
// OTA_OK             0
// OTA_SN_ERROR       1
// OTA_CHECKSUM_ERROR 2
// OTA_FORMAT_ERROR   3
// OTA_UNKNOWN_ERROR  255
unsigned char OTA_Proc(unsigned char *data, unsigned short len);


///////////////////////////interrupt running mode APIs/////////////////////////////////
//Function: SetBleIntRunningMode
//this function SHOULD be invoked before init the ble
//Parameters: None
//return: None
void SetBleIntRunningMode(void);

//Function: ble_run_interrupt_start
//   this function SHOULD be invoked to start the interrupt running mode in the main routine.
//   ble_run(0) function SHOULD ONLY be invoked in the ble irq interrupt service subroutine.
//Parameters:  interv_adv - advertise packet interval, unit 0.625ms
//return: None
void ble_run_interrupt_start(unsigned short interv_adv);

//Function: ble_nMsRoutine
//this function SHOULD be invoked every 1ms tick, one can invoke this function inside the systick routine
//Parameters: None
//return: None
void ble_nMsRoutine(void);

//Function: ble_run_interrupt_McuCanSleep
//this function CAN be invoked at main task, one can invoke this function to detect the BLE status then goto MCU sleep.
//Before doing so, one SHOULD configure the BLE irq down wakeup pin's function enabled.  
//Parameters: None
//return: None-zero means MCU can enter into stop/sleep mode.
//example:
//void IrqMcuGotoSleepAndWakeup(void)
//{
//    if(ble_run_interrupt_McuCanSleep())
//    {
//        //to do MCU sleep and wakeup steps
//    }
//}
unsigned char ble_run_interrupt_McuCanSleep(void);


///////////////////////////debug APIs/////////////////////////////////
//Parameters: isFixCh37Flag - input, 1-adv on ch37 only, 0-adv on ch37,38,39. default:0
void SetFixAdvChannel(unsigned char isFixCh37Flag);



///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////app implemented/porting functions//////////////////////////////
//unsigned int GetSysTickCount(void);
//char IsIrqEnabled(void);

/*unsigned char SPI_WriteBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len)
{
    unsigned char result = 0;
    unsigned char i;
    
    SPI_CS_Enable_();
    
    SPI_Write(reg);
    
    for (i=0;i<len;i++)
    {
        SPI_Write(*pBuf++);
    }
    
    SPI_CS_Disable_();
    
    return result;
}*/
/*unsigned char SPI_ReadBuf(unsigned char reg, unsigned char *pBuf, unsigned char len)
{
    unsigned char result = 0;
    unsigned char i;
    
    SPI_CS_Enable_();
    
    result = SPI_Write(reg);
    
    for (i=0;i<len;i++)
    {
        *(pBuf+i) = SPI_Read(0xff);
    }
    
    SPI_CS_Disable_();
    
    return result;
}*/
    
//void gatt_user_send_notify_data_callback(void);
//void ser_prepare_write(unsigned short handle, unsigned char* attValue, unsigned short attValueLen, unsigned short att_offset);
//void ser_execute_write(void);
//void att_server_rdByGrType( unsigned char pdu_type, unsigned char attOpcode, unsigned short st_hd, unsigned short end_hd, unsigned short att_type );
//void ser_write_rsp(unsigned char pdu_type/*reserved*/, unsigned char attOpcode/*reserved*/, 
//                   unsigned short att_hd, unsigned char* attValue/*app data pointer*/, unsigned char valueLen_w/*app data size*/);
//void server_rd_rsp(unsigned char attOpcode, unsigned short attHandle, unsigned char pdu_type);
//void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset);
//int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
//                            unsigned short uuid16,   
//                            unsigned short* hd_start_r,unsigned short* hd_end_r);

//void ConnectStausUpdate(unsigned char IsConnectedFlag);
//unsigned char* getDeviceInfoData(unsigned char* len);
//void UsrProcCallback(void);

//unsigned char aes_encrypt_HW(unsigned char *painText128bitBE,unsigned char *key128bitBE); //porting api, returns zero means not supported

//void McuGotoSleepAndWakeup(void);


////////////////////////For OTA application use//////////////////////////
//void OtaSystemReboot(void);
//void WriteFlashE2PROM(u8* data, u16 len, u32 pos, u8 flag); //4 bytes aligned
//u32 GetOtaAddr(void);

#endif
