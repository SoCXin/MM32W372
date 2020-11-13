#ifndef __CONFIG_H__
#define __CONFIG_H__

/* 1300Bytes */
#define api_type  "SSL"
#define api_port  "443"
#define api_ip    "47.102.117.253"
#define api_addr  "https://lab.isaaclin.cn/nCoV/api/overall"

#define wifista_ssid      "mm32_2019_ncov"        
#define wifista_password  "www.wangchaochao.top" 

#if 0
/* 省市数据 */
#define api_bj_data "https://lab.isaaclin.cn//nCoV/api/area?latest=1&province=%E5%8C%97%E4%BA%AC%E5%B8%82"
/* 疫情新闻 */
#define api_news_dat "https://lab.isaaclin.cn//nCoV/api/news"

/* 天行数据api: 4000Bytes */
#define api_tx_ip       "220.181.136.79"
#define api_tx_type     "TCP"
#define api_tx_port     "80"
#define api_tx_addr     "http://api.tianapi.com/txapi/ncov/index?key=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

#endif

#endif

