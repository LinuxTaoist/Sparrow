/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : AsyncEvent.cpp
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
#include <atomic>
#include <memory>
#include <unistd.h>
#include "Parcel.h"
#include "AsyncEvent.h"

#define     KEY_EVENT_NOTIFY        99999
#define     AEVENT_NAME_SUFFIX      "_event"

static std::atomic<bool> gObjAlive(true);
std::shared_ptr<Parcel> pEventParcel = nullptr;

AsyncEvent* AsyncEvent::GetInstance()
{
    if (!gObjAlive) {
        return nullptr;
    }

    static AsyncEvent instance;
    return &instance;
}

AsyncEvent::AsyncEvent()
    : mRunning(true), mCb(nullptr)
{
}

AsyncEvent::~AsyncEvent()
{
    mRunning = false;
    gObjAlive = false;

    if (mCbThread.joinable()) {
        mCbThread.join();
    }
}

int AsyncEvent::AsWriter(const std::string& name)
{
    mName = name;
    pEventParcel = std::make_shared<Parcel>(mName + AEVENT_NAME_SUFFIX, KEY_EVENT_NOTIFY, true);
    return pEventParcel ? 0 : -1;
}

int AsyncEvent::AsReader(const std::string& name)
{
    mName = name;
    pEventParcel = std::make_shared<Parcel>(mName + AEVENT_NAME_SUFFIX, KEY_EVENT_NOTIFY, false);
    return pEventParcel ? 0 : -1;
}

int AsyncEvent::UnregisterEventCallback()
{
    mCb = nullptr;
    mRunning = false;
    mCbThread.join();
    return 0;
}

int AsyncEvent::RegisterEventCallback(const EventCallback& callback)
{
    if (!callback || mCbThread.joinable()) {
        return -1;
    }

    mCb = callback;
    mCbThread = std::thread([&]() {
        while (mRunning) {
            int32_t event = 0;
            int32_t size = 0;
            pEventParcel->Wait();
            pEventParcel->ReadInt(event);
            pEventParcel->ReadInt(size);
            unsigned char data[size] = {};
            if (size > 0) {
                pEventParcel->ReadData(data, size);
            }

            mCb(event, data, size);
        }
    });

    return 0;
}

int AsyncEvent::EventNotify(int32_t event, void* data, int32_t size)
{
    if (!pEventParcel) {
        return -1;
    }

    pEventParcel->WriteInt(event);
    pEventParcel->WriteInt(size);
    if (size != 0) {
        pEventParcel->WriteData(data, size);
    }
    pEventParcel->Post();
    return 0;
}
