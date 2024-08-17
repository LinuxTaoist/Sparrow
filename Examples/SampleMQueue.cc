/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SampleMQueue.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include "PTimer.h"
#include "PMsgQueue.h"
#include "EpollEventHandler.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%4d MQueue D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%4d MQueue E: " fmt, __LINE__, ##args)

int main(int argc, const char *argv[])
{
    auto pMQueue = make_shared<PMsgQueue>("MQTest", 1025, [](int fd, string msg, void* arg) {
        SPR_LOGD("fd = %d, msg = %s\n", fd, msg.c_str());
    });

    shared_ptr<PTimer> pTimer = make_shared<PTimer>([&pMQueue](int fd, uint64_t time, void* arg) {
        static int cnt = 0;
        auto p = (PTimer*)arg;
        SPR_LOGD("fd = %d, time = %lu cnt = %d\n", fd, time, cnt++);

        pMQueue->Send("Hello World!");
        if (cnt > 10 && p != nullptr) {
            p->StopTimer();
        }
    });

    EpollEventHandler::GetInstance()->AddPoll(pMQueue.get());
    EpollEventHandler::GetInstance()->AddPoll(pTimer.get());
    pTimer->StartTimer(1000, 1000);
    EpollEventHandler::GetInstance()->EpollLoop(true);
    return 0;
}