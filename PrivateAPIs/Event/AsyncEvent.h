/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : AsyncEvent.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/09/27
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __ASYNC_EVENT_H__
#define __ASYNC_EVENT_H__

#include <map>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <stdint.h>
#include "CommonTypeDefs.h"

class Parcel;

typedef void(*EventCallback)(int32_t eventID, void* data, int32_t size);

#define POST_AEVENT(eventID, args, size)   AsyncEvent::GetInstance()->EventNotify(eventID, args, size)

class AsyncEvent {
public:
    static AsyncEvent* GetInstance();

    int AddWriter(const std::string& name);
    int DelWriter(const std::string& name);
    int RegisterEventCallback(const std::string& serviceName,
                              const std::shared_ptr<Parcel>& reqParcel,
                              const std::shared_ptr<Parcel>& rspParcel,
                              const EventCallback& callback);
    int UnregisterEventCallback(const std::string& serviceName,
                                const std::shared_ptr<Parcel>& reqParcel,
                                const std::shared_ptr<Parcel>& rspParcel);
    int EventNotify(int32_t event, void* args, int32_t size);

private:
    AsyncEvent();
    ~AsyncEvent();
    static std::string GetClientChannelName(const std::string& serviceName);
    int AsReader(const std::string& name);
    void StopCallbackLoop();
    void CallbackLoop(const std::shared_ptr<Parcel>& parcel);

private:
    std::atomic<bool> mRunning;
    std::mutex mWriterMutex;
    std::map<std::string, std::shared_ptr<Parcel>> mWriterParcels;
    std::mutex mReaderMutex;
    std::string mReaderName;
    std::shared_ptr<Parcel> mReaderParcel;
    std::thread mCbThread;
    std::mutex mCallbackMutex;
    std::vector<EventCallback> mCallbacks;
};

#endif // __ASYNC_EVENT_H__
