### 主要功能

由于显示屏分辨率太小，所以数据仅显示了疫情数据的部分内容。

- ESP8266发送HTTPS请求，获取疫情API数据
- MM32串口与ESP8266的AT指令交互
- 1.8寸128*160显示屏的驱动，字符的显示，文字的显示
- 现存确诊和新增，累计治愈和新增，累计死亡和新增，3组数据的显示
- 国内和全球的疫情数据分为两个界面，每10秒左右轮播一次，每10分钟更新一次最新的数据。

![](https://wcc-blog.oss-cn-beijing.aliyuncs.com/img/200626/global.jpg)

WiFi账号和密码，及API信息保存在`USER\config.h`文件中。

### 硬件环境

- 开发板：灵动eMiniBoard-MB-021，基于MM32W373PSB
- WiFi模块：ESP8266-01S
- 显示屏：1.8寸TFT，分辨率128*160，SPI接口，驱动芯片ST7735R
- 调试器：开发板板载MM-Link调试器

### 软件环境

- Keil MDK：V5.26.2
- MM32 HAL库版本：V1.1.0
- DFP器件包：MindMotion.MM32W3xxB_DFP.1.0.7
- MM32-Link：HW:A，FW:1.23，dll:1.06  Build date: 2019-06-23

### 硬件连接

- WiFi模块

UART2_RX-PA3/UART2_TX-PA2

- TFT显示屏

SCL - PA4/SDA - PA5/RES - PA6/DC  - PA7/CS  - PC4/BL  - PC5

- 按键

K1 - PB1 - 下拉/K2 - PB2 - 上拉/K3 - PB10 - 上拉/K4 - PB11 - 上拉

- LED

LD1 - PA15 - 红色/LD2 - PC10 - 绿色/LD3 - PC11 - 蓝色/LD4 - PC12 - 黄色

### 疫情数据API

- 连接类型：`HTTPS SSL`
- IP地址：`47.102.117.253`
- 端口号：`443`
- API地址：https://lab.isaaclin.cn/nCoV/api/overall

JSON数据：

```JSON
{
	"results": [{
		"currentConfirmedCount": 493,
		"currentConfirmedIncr": 20,
		"confirmedCount": 85148,
		"confirmedIncr": 29,
		"suspectedCount": 1895,
		"suspectedIncr": 2,
		"curedCount": 80007,
		"curedIncr": 8,
		"deadCount": 4648,
		"deadIncr": 1,
		"seriousCount": 97,
		"seriousIncr": 0,
		"globalStatistics": {
			"currentConfirmedCount": 4518719,
			"confirmedCount": 9557790,
			"curedCount": 4550467,
			"deadCount": 488604,
			"currentConfirmedIncr": -28848,
			"confirmedIncr": -27293,
			"curedIncr": 2764,
			"deadIncr": 1
		},
        ...省略的一些信息...
		"updateTime": 1593141861088
	}],
	"success": true
}
```

这个接口仅包含了全球和国内的疫情数据，如累计确诊、现存确诊、累计治愈、累计死亡等。数据大小约1300字节。疫情新闻，省市数据，辟谣信息等API接口可以查看：https://lab.isaaclin.cn/nCoV

作者的主页：https://isaaclin.cn

**特此感谢。**

### 其他项目

- [基于PC端Windows/Linux的疫情监控平台](http://www.wangchaochao.top/2020/02/15/qt-ncov-2/)

  https://github.com/whik/qt_2019_ncov

- [基于嵌入式Linux的疫情监控平台](http://www.wangchaochao.top/2020/03/22/YA157C-4-Functional-interface-redesign/)

  https://github.com/whik/qte_2019_ncov

### 关于我

- Github：https://github.com/whik
- E-Mail：wangchao149@foxmail.com
- Home：www.wangchaochao.top
- 公众号：电子电路开发学习（ID: MCU149）


> 欢迎Star、Fork！谢谢支持！

