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
#include "RunningTiming.h"
#include "GeneralConversions.h"
#include "GeneralUtils.h"
#include "SprEpollSchedule.h"
#include "SprObserverWithMQueue.h"
#include "PowerManagerInterface.h"

using namespace std;
using namespace GeneralUtils;
using namespace InternalDefs;

#define SPR_LOG(fmt, args...)  printf(fmt, ##args)
#define SPR_LOGD(fmt, args...) printf("%d DebugCore D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugCore E: " fmt, __LINE__, ##args)

class DebugCore : public SprObserverWithMQueue
{
public:
    DebugCore(ModuleIDType id, const std::string& name) : SprObserverWithMQueue(id, name)
    {
    }

    ~DebugCore()
    {
    }

    int ProcessMsg(const SprMsg& msg)
    {
        switch(msg.GetMsgId()) {
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
            "9: Active MQTT-OneJson01\n"
            "a: Active MQTT-DEV01\n"
            "b: Active PC_TEST_01\n"
            "c: Active PC_TEST_02\n"
            "d: Test RunningTiming\n"
            "e: Test sync ntp time\n"
            "q: Quit\n"
            "------------------------------------------------------------------\n"
    );
}

int main(int argc, const char *argv[])
{
    PowerManagerInterface* pPowerM = PowerManagerInterface::GetInstance();
    DebugCore theDebug(MODULE_DEBUG, "Debug");
    theDebug.Initialize();

    char val = 0;
    bool run = true;
    std::thread t1([&](){
        usage();
        do {
            SPR_LOGD("Input: \n");
            val = fgetc(stdin);
            getchar();
            switch(val) {
                case '0': {
                    SprMsg msg(SIG_ID_DEBUG_NOTIFY_ALL);
                    theDebug.NotifyAllObserver(msg);
                    break;
                }
                case '1': {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_3S, 0, 1000, 3000};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyObserver(msg);

                    break;
                }
                case '2': {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_3S, 0, 0, 0};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyObserver(msg);

                    break;
                }
                case '3': {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_2S, 10, 0, 2000};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyAllObserver(msg);

                    break;
                }
                case '4': {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_2S, 0, 0, 0};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyAllObserver(msg);

                    break;
                }
                case '5': {
                    STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST, 0, 0, 20};
                    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));
                    SprMsg msg(MODULE_DEBUG, MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
                    msg.SetFrom(MODULE_DEBUG);
                    msg.SetDatas(pInfo, sizeof(STimerInfo));
                    theDebug.NotifyAllObserver(msg);

                    break;
                }
                case '6': {
                    pPowerM->PowerOn();
                    break;
                }
                case '7': {
                    pPowerM->PowerOff();
                    SprEpollSchedule::GetInstance()->ExitLoop();
                    break;
                }
                case '8': {
                    SprMsg msg( MODULE_ONENET_DRIVER, SIG_ID_ONENET_DRV_SOCKET_CONNECT);
                    theDebug.NotifyObserver(msg);
                    break;
                }
                case '9': {
                    SprMsg msg( MODULE_ONENET_MANAGER, SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
                    msg.SetString("MQTT-OneJson01");
                    theDebug.NotifyObserver(msg);
                    break;
                }
                case 'a': {
                    SprMsg msg( MODULE_ONENET_MANAGER, SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
                    msg.SetString("MQTT-DEV01");
                    theDebug.NotifyObserver(msg);
                    break;
                }
                case 'b': {
                    SprMsg msg( MODULE_ONENET_MANAGER, SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
                    msg.SetString("PC_TEST_01");
                    theDebug.NotifyObserver(msg);
                    break;
                }
                case 'c': {
                    SprMsg msg(MODULE_ONENET_MANAGER, SIG_ID_ONENET_MGR_ACTIVE_DEVICE_CONNECT);
                    msg.SetString("PC_TEST_02");
                    theDebug.NotifyObserver(msg);
                    break;
                }
                case 'd': {
                    RunningTiming timing;
                    usleep(10000);  // 10ms
                    SPR_LOGD("Timing 1: add 10ms %ds, %dms\n", timing.GetElapsedTimeInSec(), timing.GetElapsedTimeInMSec());
                    usleep(1000000);  // 1s
                    SPR_LOGD("Timing 2: add 1s   %ds, %dms\n", timing.GetElapsedTimeInSec(), timing.GetElapsedTimeInMSec());
                    usleep(30000);  // 30ms
                    SPR_LOGD("Timing 3: add 30ms %ds, %dms\n", timing.GetElapsedTimeInSec(), timing.GetElapsedTimeInMSec());
                    break;
                }
                case 'e': {
                    SprMsg msg(MODULE_TIMEM, SIG_ID_TIMEM_NTP_SYNC);
                    theDebug.NotifyObserver(msg);
                    break;
                }
                case 'q': {
                    run = false;
                    break;
                }

                default:
                    usage();
                    break;
            }
        } while(run);
    });

    SprEpollSchedule::GetInstance()->EpollLoop(true);

    val = 'q';
    int ret = write(STDIN_FILENO, &val, 1);
    SPR_LOGD("Exit Debug Core. ret = %d\n", ret);
    t1.join();
    return 0;
}

