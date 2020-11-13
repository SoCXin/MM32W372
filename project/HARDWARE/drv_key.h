#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__


#include "sys.h"

/*
K1 - PB1 - 下拉
K2 - PB2 - 上拉
K3 - PB10 - 上拉
K4 - PB11 - 上拉
*/

//#define USE_EXIT

#define K1 PBin(1)
#define K2 PBin(2)
#define K3 PBin(10)
#define K4 PBin(11)

void key_init(void);//IO初始化
u8 key_get(u8);    //按键扫描函数


#endif
