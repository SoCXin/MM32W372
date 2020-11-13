#ifndef __WIFISTA_H
#define __WIFISTA_H

#include "sys.h"

void atk_8266_init(void);
void atk_8266_at_response(u8 mode);
u8* atk_8266_check_cmd(u8 *str);
u8 atk_8266_send_data(u8 *data,u8 *ack,uint16_t waittime);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,uint16_t waittime);
u8 atk_8266_quit_trans(void);
u8 atk_8266_consta_check(void);
void atk_8266_get_wanip(u8* ipbuf);
void atk_8266_get_ip(u8 x,u8 y);
u8 atk_8266_wifista_config(void);
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA º”√‹∑Ω Ω
extern const u8* wifista_password; 	//WIFI STA √‹¬Î

extern const u8* ATK_ESP8266_ECN_TBL[5];


#endif



