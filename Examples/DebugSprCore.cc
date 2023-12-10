/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugSprCore.cc
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
#include "SprObserver.h"
#include "SprMediatorIpcProxy.h"

using namespace std;
using namespace InternalEnum;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d DebugCore D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugCore E: " fmt, __LINE__, ##args)

class DebugCore : public SprObserver
{
public:
    DebugCore(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr)
        : SprObserver(id, name, mMsgMediatorPtr)
    {
    }

    ~DebugCore()
    {
    }

    int ProcessMsg(const SprMsg& msg)
    {
        return 0;
    }
};

int main(int agrc, const char *argv[])
{
    DebugCore theDebug(MODULE_LOGM, "Debug", make_shared<SprMediatorIpcProxy>());

    char val = 0;
    bool run = true;
    do {
        SPR_LOGD("Input: ");
        std::cin >> val;
        switch(val)
        {
            case '0':   // debug SprMsg
            {
                SprMsg theEnMsg((uint32_t)DEBUG_MSG_SERIAL);
                theEnMsg.setU8Value(8);
                theEnMsg.setU16Value(16);
                theEnMsg.setU32Value(32);
                theEnMsg.setString("Test Spr Core");
                std::vector<uint8_t> u8Vec1 = {'s', 'p', 'a', 'r', 'r', 'o', 'w'};
                theEnMsg.setU8Vec(u8Vec1);
                std::vector<uint32_t> u32Vec1 = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555};
                theEnMsg.setU32Vec(u32Vec1);

                struct TestSpr
                {
                    int id;
                    char buf[20];
                };

                TestSpr testData = { 10, "hello sparrow"};
                shared_ptr<struct TestSpr> p = static_pointer_cast<struct TestSpr>(make_shared<TestSpr>(testData));
                theEnMsg.setDatas(p, sizeof(TestSpr));

                std::string enDatas;
                theEnMsg.encode(enDatas);

                SprMsg theDeMsg(enDatas);
                vector<uint8_t> u8Vec = theDeMsg.getU8Vec();
                vector<uint32_t> u32Vec = theDeMsg.getU32Vec();
                std::shared_ptr<struct TestSpr> dePtr = theDeMsg.getDatas<struct TestSpr>();

                SPR_LOGD("msgId:    %d  \n", theDeMsg.getMsgId());
                SPR_LOGD("u8Value:  %d  \n", theDeMsg.getU8Value());
                SPR_LOGD("u16Value: %d  \n", theDeMsg.getU16Value());
                SPR_LOGD("string:   %s  \n", theDeMsg.getString().c_str());
                SPR_LOGD("u8Vec:    %ld \n", u8Vec.size());
                SPR_LOGD("u32Vec:   %ld \n", u32Vec.size());
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

