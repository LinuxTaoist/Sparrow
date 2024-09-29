/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugMsg.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
#include "SprLog.h"
#include "GeneralConversions.h"
#include "SprObserver.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("DebugMsg", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("DebugMsg", fmt, ##args)

int main(int argc, const char *argv[])
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
                theEnMsg.SetBoolValue(false);
                theEnMsg.SetFrom(0x01);
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
                } testData = { 10, "hello sparrow"};

                shared_ptr<struct TestSpr> p = static_pointer_cast<struct TestSpr>(make_shared<TestSpr>(testData));
                theEnMsg.SetDatas(p, sizeof(TestSpr));

                std::string enDatas;
                theEnMsg.Encode(enDatas);

                SprMsg theDeMsg(enDatas);
                vector<uint8_t> deU8Vec = theDeMsg.GetU8Vec();
                vector<uint32_t> deU32Vec = theDeMsg.GetU32Vec();
                std::shared_ptr<struct TestSpr> dePtr = theDeMsg.GetDatas<struct TestSpr>();

                SPR_LOGD("from          : 0x%x  \n", theDeMsg.GetFrom());
                SPR_LOGD("to            : 0x%x  \n", theDeMsg.GetTo());
                SPR_LOGD("msgId         : 0x%x  \n", theDeMsg.GetMsgId());
                SPR_LOGD("boolValue     : %d    \n", theDeMsg.GetBoolValue());
                SPR_LOGD("u8Value       : %d    \n", theDeMsg.GetU8Value());
                SPR_LOGD("u16Value      : %d    \n", theDeMsg.GetU16Value());
                SPR_LOGD("string        : %s    \n", theDeMsg.GetString().c_str());
                SPR_LOGD("u8Vec[%ld]    : %s    \n", deU8Vec.size(), GeneralConversions::VectorToString(deU8Vec).c_str());
                SPR_LOGD("u32Vec[%ld]   : %s    \n", deU32Vec.size(), GeneralConversions::VectorToHexStringWithSpace(deU32Vec).c_str());
                SPR_LOGD("ptr           : %d    \n", dePtr->id);
                SPR_LOGD("                %s    \n", dePtr->buf);

                SprMsg copyMsg(theDeMsg);
                std::vector<uint8_t> copyU8Vec = copyMsg.GetU8Vec();
                std::vector<uint32_t> copyU32Vec = copyMsg.GetU32Vec();
                std::shared_ptr<struct TestSpr> copyPtr = copyMsg.GetDatas<struct TestSpr>();
                SPR_LOGD("-------------------------------------------------------------\n");
                SPR_LOGD("from          : 0x%x  \n", copyMsg.GetFrom());
                SPR_LOGD("to            : 0x%x  \n", copyMsg.GetTo());
                SPR_LOGD("msgId         : 0x%x  \n", copyMsg.GetMsgId());
                SPR_LOGD("boolValue     : %d    \n", copyMsg.GetBoolValue());
                SPR_LOGD("u8Value       : %d    \n", copyMsg.GetU8Value());
                SPR_LOGD("u16Value      : %d    \n", copyMsg.GetU16Value());
                SPR_LOGD("string        : %s    \n", copyMsg.GetString().c_str());
                SPR_LOGD("u8Vec[%ld]    : %s    \n", copyU8Vec.size(), GeneralConversions::VectorToString(copyU8Vec).c_str());
                SPR_LOGD("u32Vec[%ld]   : %s    \n", copyU32Vec.size(), GeneralConversions::VectorToHexStringWithSpace(copyU32Vec).c_str());
                SPR_LOGD("ptr           : %d    \n", copyPtr->id);
                SPR_LOGD("                %s    \n", copyPtr->buf);
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
