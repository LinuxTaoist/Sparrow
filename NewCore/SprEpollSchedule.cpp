/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprEpollSchedule.cpp
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
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include "SprLog.h"
#include "LibgoAdapter.h"
#include "SprEpollSchedule.h"

#define SPR_LOGD(fmt, args...) LOGD("SprEpollSch", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("SprEpollSch", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprEpollSch", fmt, ##args)

const uint32_t EPOLL_FD_NUM = 10;

SprEpollSchedule::SprEpollSchedule(uint32_t size) : EpollEventHandler(size)
{
}

SprEpollSchedule::~SprEpollSchedule()
{
}

void SprEpollSchedule::HandleEpollEvent(IEpollEvent& event)
{
    // 触发回调处理器
    // using GoPoolCb = co::AsyncCoroutinePool::CallbackPoint;
    // std::shared_ptr<GoPoolCb> cbp(new GoPoolCb);
    // mCoPool.AddCallbackPoint(cbp.get());

    // 投递任务至协程，没有回调
    mCoPool.Post([&] {
        EpollEventHandler::HandleEpollEvent(event);
    }, nullptr);

}
