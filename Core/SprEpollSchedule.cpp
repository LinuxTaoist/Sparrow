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
#include "PLog.h"   // EpollEventHandler
// #include "LibgoAdapter.h"
#include "SprEpollSchedule.h"

#define LOG_TAG "SprEpollSch"

const uint32_t EPOLL_FD_NUM = 10;
bool SprEpollSchedule::mEnablePLog = false;

SprEpollSchedule::SprEpollSchedule(int32_t size, int32_t timeout, bool enableCoroutine)
    : EpollEventHandler(size, timeout),
      mEnableCoroutine(enableCoroutine)
{
    // if (enableCoroutine) {
    //     mCoPool.InitCoroutinePool(1024);
    //     mCoPool.Start(10, 128);
    // }

    InitPLog();
    SPR_LOGD("%s coroutine schedule!\n", enableCoroutine ? "Enable" : "Disable");
}

SprEpollSchedule::~SprEpollSchedule()
{
}

EpollEventHandler* SprEpollSchedule::GetInstance(int32_t size, int32_t timeout, bool enableCoroutine)
{
    if (!mEnablePLog) {
        mEnablePLog = true;
        InitPLog();
    }

    return EpollEventHandler::GetInstance(size, timeout);
}

void SprEpollSchedule::InitPLog()
{
    SPR_LOGD("Init epoll event handler!\n");
    PLog& theLog = PLog::GetInstance();
    theLog.RegisterPrintCallback([](int level, int line, const char* tag, const char* fmt, va_list ap) {
        char logBuf[1024] = {0};
        vsnprintf(logBuf, sizeof(logBuf), fmt, ap);
        switch (level) {
            case PLogLevel::PLOG_LEVEL_DEBUG:
                SprLog::GetInstance()->d(tag, "%4d %s", line, logBuf);
                break;
            case PLogLevel::PLOG_LEVEL_INFO:
                SprLog::GetInstance()->i(tag, "%4d %s", line, logBuf);
                break;
            case PLogLevel::PLOG_LEVEL_ERROR:
                SprLog::GetInstance()->e(tag, "%4d %s", line, logBuf);
                break;
            case PLogLevel::PLOG_LEVEL_WARN:
                SprLog::GetInstance()->w(tag, "%4d %s", line, logBuf);
                break;
            default:
                SprLog::GetInstance()->i(tag, "%4d %s", line, logBuf);
                break;
        }
    });
}

void SprEpollSchedule::HandleEpollEvent(IEpollEvent& event)
{
    if (mEnableCoroutine) {
        // 触发回调处理器
        // using GoPoolCb = co::AsyncCoroutinePool::CallbackPoint;
        // std::shared_ptr<GoPoolCb> cbp(new GoPoolCb);
        // mCoPool.AddCallbackPoint(cbp.get());

        // // 投递任务至协程，没有回调
        // mCoPool.Post([&] {
        //     EpollEventHandler::HandleEpollEvent(event);
        // }, nullptr);
        SPR_LOGW("Not support coroutine!\n");
    } else {
        EpollEventHandler::HandleEpollEvent(event);
    }
}
