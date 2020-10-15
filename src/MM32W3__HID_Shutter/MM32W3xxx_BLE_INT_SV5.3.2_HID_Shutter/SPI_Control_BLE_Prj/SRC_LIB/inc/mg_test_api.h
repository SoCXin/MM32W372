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
/* This lib is for test use, should be used together with mg_BLE*.lib later than v3.5.0(MS179x)/v4.2.0((MS1793S))/v5.2.0(MS1892) */

#ifndef _MG_TESTAPI_H_
#define _MG_TESTAPI_H_




///////////////////////////test APIs/////////////////////////////////

//Parameters: freq - input, 0~80, center frequency(2400+freq)MHz, txpwr - input, 0x20~0x4A, txpower
//return: None. in testing, add while(1); after calling this function
void test_SRRCCarrier(unsigned char  freq, unsigned char txpwr);

//Parameters: freq - input, 0~80, center frequency(2400+freq)MHz, txpwr - input, 0x20~0x4A, txpower
//return: None. in testing, add while(1); after calling this function
void test_PRBS9(unsigned char freq, unsigned char txpwr);

//Parameters: freq - input, 0~80, center frequency(2400+freq)MHz
//return: None. in testing, add while(1); after calling this function
void test_RX(unsigned char freq);


//Parameters: freq - input, 0~80, center frequency(2400+freq)MHz, txpwr - input, 0x20~0x4A, txpower
//return: None. in testing, add while(1); after calling this function
void test_carrier(unsigned char freq, unsigned char txpwr);

unsigned char* GetTestLibVer(void);

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////app implemented/porting functions//////////////////////////////
//unsigned int GetSysTickCount(void);


#endif
