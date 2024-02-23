/**
 *---------------------------------------------------------------------------------------------------------------------
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
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
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
        switch(msg.GetMsgId())
        {
            default:
                SPR_LOGD("msg id: 0x%x\n", msg.GetMsgId());
                break;
        }

        return 0;
    }
};

int main(int agrc, const char *argv[])
{
    DebugCore theDebug(MODULE_DEBUG, "Debug", make_shared<SprMediatorIpcProxy>());

    char val = 0;
    bool run = true;
    do {
        SPR_LOGD("Input: ");
        std::cin >> val;
        switch(val)
        {
            case '0':
            {
                SprMsg msg(SIG_ID_DEBUG_NOTIFY_ALL);
                theDebug.NotifyAllObserver(msg);
                break;
            }

            case 'q':
            {
                run = false;
            }

            default:
            break;
        }
    } while(run);

    return 0;
}

