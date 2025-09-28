/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : AsyncEvent.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/09/27
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/09/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __ASYNC_EVENT_H__
#define __ASYNC_EVENT_H__

#include <thread>
#include <string>
#include <stdint.h>
#include "CommonTypeDefs.h"

typedef void(*EventCallback)(int32_t eventID, void* data, int32_t size);

#define ASYNC_NOTIFY(eventID, args, size)   AsyncEvent::GetInstance()->EventNotify(eventID, args, size)

class AsyncEvent
{
public:
    static AsyncEvent* GetInstance();

    int AsWriter(const std::string& name);
    int AsReader(const std::string& name);
    int UnregisterEventCallback();
    int RegisterEventCallback(const EventCallback& callback);
    int EventNotify(int32_t event, void* args, int32_t size);

private:
    AsyncEvent();
    ~AsyncEvent();

private:
    bool mRunning;
    std::string mName;
    std::thread mCbThread;
    EventCallback mCb;
};

#endif // __ASYNC_EVENT_H__
