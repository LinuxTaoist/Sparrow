/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugSprCore.cc
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
#include <sstream>
#include <memory>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "GeneralConversions.h"
#include "GeneralUtils.h"
#include "SprObserver.h"
#include "SprMediatorIpcProxy.h"
#include "PowerManagerInterface.h"

using namespace std;
using namespace GeneralUtils;
using namespace InternalDefs;

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
                SPR_LOGD("msg id: %s %s\n", GetSigName(msg.GetMsgId()), GetCurTimeStr().c_str());
                break;
        }

        return 0;
    }
};

static void usage()
{
    SPR_LOG("------------------------------------------------------------------\n"
            "usage:\n"
            "0: NotifyAllObserver\n"
            "1: AddTimer ( 0, 1s, 3s)\n"
            "2: DelTimer case 1 \n"
            "3: AddTimer (10, 0s, 2s)\n"
            "4: DelTimer case 3 \n"
            "5: AddTimer ( 0, 0s, 20ms)\n"
            "6: PowerOn\n"
            "7: PowerOff\n"
            "8: OneNetDrv socket connect\n"
            "q: Quit\n"
            "------------------------------------------------------------------\n"
    );
}

int main(int argc, const char *argv[])
{
    PowerManagerInterface* pPowerM = PowerManagerInterface::GetInstance();
    DebugCore theDebug(MODULE_DEBUG, "Debug", make_shared<SprMediatorIpcProxy>());

    char val = 0;
    bool run = true;
    std::thread t1([&](){
        usage();
        do {
            SPR_LOGD("Input: \n");
            val = fgetc(stdin);
            getchar();
            switch(val)
            {
                case '0':
                {
                    SprMsg msg(SIG_ID_DEBUG_NOTIFY_ALL);
                    theDebug.NotifyAllObserver(msg);
                    break;
                }

                case '1':
                {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_3S, 0, 1000, 3000};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyObserver(msg);

                    break;
                }

                case '2':
                {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_3S, 0, 0, 0};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyObserver(msg);

                    break;
                }

                case '3':
                {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_2S, 10, 0, 2000};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyAllObserver(msg);

                    break;
                }

                case '4':
                {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_2S, 0, 0, 0};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyAllObserver(msg);

                    break;
                }

                case '5':
                {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST, 0, 0, 20};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyAllObserver(msg);

                    break;
                }

                case '6':
                {
                    pPowerM->PowerOn();
                    break;
                }

                case '7':
                {
                    pPowerM->PowerOff();
                    SprObserver::MainExit();
                    break;
                }

                case '8':
                {
                    SprMsg msg( MODULE_ONENET_DRIVER, SIG_ID_ONENET_DRV_SOCKET_CONNECT);
                    theDebug.NotifyAllObserver(msg);
                    break;
                }

                case 'q':
                {
                    run = false;
                    break;
                }

                default:
                    usage();
                    break;
            }
        } while(run);
    });

    SprObserver::MainLoop();

    val = 'q';
    int ret = write(STDIN_FILENO, &val, 1);
    SPR_LOGD("Exit Debug Core. ret = %d\n", ret);
    t1.join();
    return 0;
}

