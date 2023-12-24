/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Sample1.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
#include "Convert.h"
#include "SprObserver.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d DebugMsg D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugMsg E: " fmt, __LINE__, ##args)

int main(int agrc, const char *argv[])
{
    char val = 0;
    bool run = true;
    do {
        SPR_LOGD("Input: ");
        std::cin >> val;
        switch(val)
        {
            case '0':   // debug SprMsg
            {
                SprMsg theEnMsg((uint32_t)SIG_ID_DEBUG_MSG_SERIAL);
                theEnMsg.SetU8Value(8);
                theEnMsg.SetU16Value(16);
                theEnMsg.SetU32Value(32);
                theEnMsg.SetString("Test Spr Core");
                std::vector<uint8_t> u8Vec1 = {'s', 'p', 'a', 'r', 'r', 'o', 'w'};
                theEnMsg.SetU8Vec(u8Vec1);
                std::vector<uint32_t> u32Vec1 = {0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555};
                theEnMsg.SetU32Vec(u32Vec1);

                struct TestSpr
                {
                    int id;
                    char buf[20];
                };

                TestSpr testData = { 10, "hello sparrow"};
                shared_ptr<struct TestSpr> p = static_pointer_cast<struct TestSpr>(make_shared<TestSpr>(testData));
                theEnMsg.SetDatas(p, sizeof(TestSpr));

                std::string enDatas;
                theEnMsg.Encode(enDatas);

                SprMsg theDeMsg(enDatas);
                vector<uint8_t> u8Vec = theDeMsg.GetU8Vec();
                vector<uint32_t> u32Vec = theDeMsg.GetU32Vec();
                std::shared_ptr<struct TestSpr> dePtr = theDeMsg.GetDatas<struct TestSpr>();
                std::string u8Str(u8Vec1.begin(), u8Vec.end());

                SPR_LOGD("msgId:    0x%x  \n", theDeMsg.GetMsgId());
                SPR_LOGD("u8Value:  %d  \n", theDeMsg.GetU8Value());
                SPR_LOGD("u16Value: %d  \n", theDeMsg.GetU16Value());
                SPR_LOGD("string:   %s  \n", theDeMsg.GetString().c_str());
                SPR_LOGD("u8Vec[%ld]:  %s \n", u8Vec.size(), u8Str.c_str());
                SPR_LOGD("u32Vec[%ld]: %s \n", u32Vec.size(), Convert::vectorToHexString(u32Vec).c_str());
                SPR_LOGD("ptr:      %d  \n", dePtr->id);
                SPR_LOGD("          %s  \n", dePtr->buf);
                break;
            }

            case 'q':
                run = false;
                break;

            default:
                SPR_LOGE("No this option: %c", val);
            break;
        }
    } while(run);

    return 0;
}
