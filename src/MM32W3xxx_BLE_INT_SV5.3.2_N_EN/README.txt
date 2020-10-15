//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
һ������˵����
    BLE ����ϵͳ�жϷ������ʽ���У� �ʺ���ʵ���û�ĳ������Ҫռ�ýϳ� CPU ʱ�䵫���Ա���ϵ�Ӧ�ó����� ��Ҫ�õ������жϷ������
    һ���� IRQ �ж� pin ��Ӧ�� GPIO �жϣ� һ����ʵ�� SysTick ��Ӧ���жϡ� IRQ ��Ӧ���жϷ������������������Э�飬 ��Ҫ�нϸߵ���
    �����ȼ��� ��������û��ж���˵�� ��

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
����Ӳ��˵����
    �������Э��ջ�� ϵͳӦ��ʵ���� SPI ͨ�š� ��ʱ���Ƚӿڡ� ��Ҫ����������
1�� unsigned char SPI_WriteBuf(unsigned char reg, unsigned char const *pBuf, unsigned char len);

2�� unsigned char SPI_ReadBuf(unsigned char reg, unsigned char *pBuf, unsigned char len);
  �������������� spi.c �ļ���ʵ�֣� �� BLE ��ͨ����أ� �벻Ҫ�޸ġ� Ϊ��֤ BLE ������ spi ʱ��Ӧ����6MHz�� С�� 10MHz��
  
3�� Char IsIrqEnabled(void) ; �ж� IRQ �ź��Ƿ�����жϣ��͵�ƽΪ�ж���Ч�� ��

4�� void IRQ_RF(void);
�������������� irq_rf.c �ļ���ʵ�֣� �� BLE ��ͨ����أ��벻Ҫ�޸ġ�

5�� unsigned int GetSysTickCount(void); ��ȡ���붨ʱ���ۻ�ֵ�����ڼ�ʱ�ȹ��ܡ�

6�� void SysTick_Handler(void)
�������������� bsp.c �ļ���ʵ�֣� �� BLE ͨ����أ��벻Ҫ�޸ġ�ϵͳ��શ�ʱ�����ȿ��Ըı䣬�����1ms �� 10ms����Ӱ�� BLE �������С�

7�� �͹��Ĺ���ʵ��ʾ����
void IrqMcuGotoSleepAndWakeup(void)
{
   if(ble_run_interrupt_McuCanSleep())
   {
     //to do MCU sleep/stop/standby
   }
} �˺����� main()�������û�����ѭ�����á�

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
����MCU����Ƶ�������ţ�

   MCU      RF
   PB5      MOSI 
   PB4      MISO(���ⲿӲ������)
   PB3      SCK
   PD2      CSN
   PC12     IRQ(���ⲿӲ������)
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
�ġ��ӿں�����
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
�塢�ϵ���Ϣ��

�˰汾Ϊ͸�����ж��ۺϰ汾��ͨ��USE_AT_CMD�ĺ�������ģʽ�л�
��	   ��	  �ʣ�		9600
