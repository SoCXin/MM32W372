//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
一、程序说明：
    BLE 库以系统中断服务程序方式运行， 适合于实现用户某功能需要占用较长 CPU 时间但可以被打断的应用场景。 需要用到两个中断服务程序，
    一个是 IRQ 中断 pin 对应的 GPIO 中断， 一个是实现 SysTick 对应的中断。 IRQ 对应的中断服务程序用于运行蓝牙协议， 需要有较高的中
    断优先级（ 针对其他用户中断来说） 。

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
二、硬件说明：
    配合蓝牙协议栈， 系统应用实现了 SPI 通信、 定时器等接口。 主要函数包括：
1、 unsigned char SPI_WriteBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len);

2、 unsigned char SPI_ReadBuf(unsigned char reg, unsigned char *pBuf, unsigned char len);
  以上两个函数在 spi.c 文件中实现， 和 BLE 的通信相关， 请不要修改。 为保证 BLE 工作， spi 时钟应大于6MHz， 小于 10MHz。
  
3、 Char IsIrqEnabled(void) ; 判断 IRQ 信号是否产生中断（低电平为中断有效） 。

4、 void IRQ_RF(void);
以上两个函数在 irq_rf.c 文件中实现， 和 BLE 的通信相关，请不要修改。

5、 unsigned int GetSysTickCount(void); 获取毫秒定时器累积值，用于计时等功能。

6、 void SysTick_Handler(void)
以上两个函数在 bsp.c 文件中实现， 和 BLE 通信相关，请不要修改。系统嘀嗒定时器精度可以改变，比如从1ms 到 10ms，不影响 BLE 正常运行。

7、 低功耗功能实现示例：
void IrqMcuGotoSleepAndWakeup(void)
{
   if(ble_run_interrupt_McuCanSleep())
   {
     //to do MCU sleep/stop/standby
   }
} 此函数在 main()函数的用户程序循环调用。

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
三、MCU与射频控制引脚：

   MCU      RF
   PB5      MOSI 
   PB4      MISO(需外部硬件连接)
   PB3      SCK
   PD2      CSN
   PC12     IRQ(需外部硬件连接)
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
四、接口函数：
1) void radio_initBle(unsigned char txpwr, unsigned char**addr/*Output*/);
2) void radio_standby(void);
3) void ble_run_interrupt_start(unsigned short adv_interval);
4) void SetBleIntRunningMode(void);
5) void ble_set_adv_type(unsigned char type);
6) void ble_set_adv_data(unsigned char* adv, unsigned char len);
7) void ble_set_adv_rsp_data(unsigned char* rsp, unsigned char len);
8) void ble_set_name(unsigned char* name,unsigned char len);
9) void ble_set_interval(unsigned short interval);
10) void ble_set_adv_enableFlag(char sEnableFlag);
11) unsigned char ble_set_role(unsigned char role_new, unsigned short scan_window);
12) void ble_disconnect(void);
13) unsigned char *get_ble_version(void);
14) unsigned char *GetFirmwareInfo(void);
15) void ser_write_rsp_pkt(unsigned char pdu_type);
16) void att_notFd(unsigned char pdu_type, unsigned char attOpcode, unsigned short attHd );
17) void att_ErrorFd_ecode(unsigned char pdu_type, unsigned char attOpcode, unsigned short attHd, unsigned char errCode );
18) void att_server_rdByGrTypeRspDeviceInfo(unsigned char pdu_type);
19) void att_server_rdByGrTypeRspPrimaryService(unsigned char pdu_type,
                                                unsigned short start_hd,
                                                unsigned short end_hd,
                                                unsigned char*uuid,
                                                unsigned char uuidlen);
20) void att_server_rd(unsigned char pdu_type,
                        unsigned char attOpcode,
                        unsigned short att_hd,
                        unsigned char* attValue,
                        unsigned char datalen );
21) unsigned char sconn_notifydata(unsigned char* data, unsigned char len);
22) unsigned char sconn_indicationdata(unsigned char* data, unsigned char len);
23) void SetFixAdvChannel(unsigned char isFixCh37Flag);
24) void test_carrier(unsigned char freq, unsigned char txpwr);
25) void radio_setBleAddr(u8 addr[6]);
26) void SIG_ConnParaUpdateReq(unsigned short IntervalMin, unsigned short IntervalMax, unsigned short SlaveLatency,unsigned short TimeoutMultiplier);
27) unsigned short sconn_GetConnInterval(void);
28) Unsigned char GetRssiData(void);
29)unsigned char radio_initBle_TO(unsigned char txpwr, unsigned char** addr, unsigned short ms_timeout);
30)void radio_initBle_recover(unsigned char txpwr, unsigned char** addr);
31)void radio_setCal_nonBlocking(unsigned nonblocking);
32)void radio_resume(void);
33)void radio_fixSPI(unsigned char cs,unsigned char sck,unsigned char mosi);
34)void radio_setXtal(unsigned char xoib, unsigned char xocc);
35)unsigned char radio_setRxGain(unsigned char lna_gain, unsigned char preamble_th);
36)void ble_run(unsigned short interv_adv);
37)unsigned char ble_set_wakeupdly(unsigned short counter);
38)unsigned char ble_run_interrupt_McuCanSleep(void);
39)void ser_write_rsp_pkt(unsigned char pdu_type);
40)unsigned char* GetMasterDeviceMac(unsigned char* MacType);
41)void SetLePinCode(unsigned char *PinCode/*6 0~9 digitals*/);
42)unsigned char* GetLTKInfo(unsigned char* newFlag);
43)void s_llSmSecurityReq(void);
44)void Led_getInfo(unsigned char* data);
45)void SetLEDLum(int r, int g, int b, int L); //rgb[0~255], L[0~100,101] 101 means not used 
46)void UpdateLEDValueFading(unsigned char flag_fade); //1-fading, 0-now
47)unsigned char OTA_Proc(unsigned char *data, unsigned short len);
48)void SetBleIntRunningMode(void);
49)void ble_run_interrupt_start(unsigned short interv_adv);
50)void ble_nMsRoutine(void);
51)GetOtaAddr;
52)void WriteFlashE2PROM(u8* data, u16 len, u32 pos, u8 flag);
53)void OtaSystemReboot(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
五、上电信息：

此版本为透传与中断综合版本，通过USE_AT_CMD的宏来进行模式切换
波	   特	  率：		9600
