#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "wifista.h"
#include "drv_uart2.h"
#include "drv_uart1.h"
#include "drv_led.h"
#include "delay.h"
#include "config.h"


//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零UART2_RX_STA;
//     1,清零UART2_RX_STA;
void atk_8266_at_response(uint8_t mode)
{
    if (UART2_RX_STA & 0X8000)     //接收到一次数据了
    {
        UART2_RX_BUF[UART2_RX_STA & 0X7FFF] = 0; //添加结束符
        printf("%s", UART2_RX_BUF); //发送到串口
        if (mode)UART2_RX_STA = 0;
    }
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t *atk_8266_check_cmd(uint8_t *str)
{
    char *strx = 0;
    if (UART2_RX_STA & 0X8000)     //接收到一次数据了
    {
        UART2_RX_BUF[UART2_RX_STA & 0X7FFF] = 0; //添加结束符
        strx = strstr((const char *)UART2_RX_BUF, (const char *)str);
    }
    return (uint8_t *)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t atk_8266_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t  waittime)
{
    uint8_t res = 0;
    UART2_RX_STA = 0;
    u2_printf("%s\r\n", cmd);   //发送命令
    if (ack && waittime)    //需要等待应答
    {
        while (--waittime)  //等待倒计时
        {
            delay_ms(10);
            if (UART2_RX_STA & 0X8000) //接收到期待的应答结果
            {
                if (atk_8266_check_cmd(ack))
                {
                    printf("发送：%s 回应:%s\r\n", cmd, (uint8_t *)ack);
                    break;//得到有效数据
                }
                UART2_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
uint8_t atk_8266_send_data(uint8_t *data, uint8_t *ack, uint16_t  waittime)
{
    uint8_t res = 0;
    UART2_RX_STA = 0;
    u2_printf("%s", data);  //发送命令
    if (ack && waittime)    //需要等待应答
    {
        while (--waittime)  //等待倒计时
        {
            delay_ms(10);
            if (UART2_RX_STA & 0X8000) //接收到期待的应答结果
            {
                if (atk_8266_check_cmd(ack))break; //得到有效数据
                UART2_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
uint8_t atk_8266_quit_trans(void)
{
    while ((UART2->CSR & UART_IT_TXIEN) == 0); //等待发送空
    UART2->TDR = ('+' & (uint16_t)0x00FF);
    delay_ms(15);                   //大于串口组帧时间(10ms)
    while ((UART2->CSR & UART_IT_TXIEN) == 0); //等待发送空
    UART2->TDR = ('+' & (uint16_t)0x00FF);
    delay_ms(15);                   //大于串口组帧时间(10ms)
    while ((UART2->CSR & UART_IT_TXIEN) == 0); //等待发送空
    UART2->TDR = ('+' & (uint16_t)0x00FF);
    delay_ms(500);                  //等待500ms
    return atk_8266_send_cmd("AT", "OK", 20); //退出透传判断.
}

//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
uint8_t atk_8266_consta_check(void)
{
    uint8_t *p;
    uint8_t res;
    if (atk_8266_quit_trans())return 0;         //退出透传
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50); //发送AT+CIPSTATUS指令,查询连接状态
    p = atk_8266_check_cmd("+CIPSTATUS:");
    res = *p;                               //得到连接状态
    return res;
}

//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(uint8_t *ipbuf)
{
    uint8_t *p, *p1;
    if (atk_8266_send_cmd("AT+CIFSR", "OK", 50)) //获取WAN IP地址失败
    {
        ipbuf[0] = 0;
        return;
    }
    p = atk_8266_check_cmd("\"");
    p1 = (uint8_t *)strstr((const char *)(p + 1), "\"");
    *p1 = 0;
    sprintf((char *)ipbuf, "%s", p + 1);
}

//配置ESP8266位sta模式，并连接到路由器
uint8_t atk_8266_wifista_config(void)
{
    uint8_t p[200];
	char str[200];
    uint8_t TryConnectTime = 1;
	printf("准备连接\r\n");
    led_set(1, ON);
    while (atk_8266_send_cmd("AT", "OK", 20)) //检查WIFI模块是否在线
    {
        atk_8266_quit_trans();//退出透传
		delay_ms(1000);
        printf("未检测到模块\r\n");
    }
    atk_8266_send_cmd("AT+RESTORE", "OK", 200); //关闭透传模式
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    
    atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200); //关闭透传模式
    while (atk_8266_send_cmd("ATE0", "OK", 20)); //关闭回显
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);     //设置WIFI STA模式
//    atk_8266_send_cmd("AT+RST", "OK", 20);      //DHCP服务器关闭(仅AP模式有效)
	delay_ms(1000);         //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    //设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20); //0：单连接，1：多连接
    sprintf((char *)p, "AT+CWJAP=\"%s\",\"%s\"", wifista_ssid, wifista_password); //设置无线参数:ssid,密码

	sprintf((char *)str, "%s", wifista_ssid);

	while (atk_8266_send_cmd(p, "WIFI GOT IP", 300))
    {
        printf("WiFi连接失败,正在尝试第 %d 次连接\r\n", TryConnectTime++);
        if (TryConnectTime >= 250)
            TryConnectTime = 0;
        delay_ms(100);
    };                  //连接目标路由器,并且获得IP
    printf("WiFi连接成功\r\n");
    led_set(2, ON);

    return 0;
}

