/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : AsyncEvent.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/09/27
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <atomic>
#include <memory>
#include <vector>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include "Parcel.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "AsyncEvent.h"

using namespace InternalDefs;

#define     KEY_EVENT_NOTIFY        99999
#define     AEVENT_NAME_SUFFIX      "_event"
#define     AEVENT_MAX_DATA_SIZE    (10 * 1024)

static std::atomic<bool> gObjAlive(true);

AsyncEvent* AsyncEvent::GetInstance() {
    if (!gObjAlive) {
        return nullptr;
    }

    static AsyncEvent instance;
    return &instance;
}

std::string AsyncEvent::GetClientChannelName(const std::string& serviceName) {
    return serviceName + "_pid_" + std::to_string(getpid());
}

AsyncEvent::AsyncEvent()
    : mRunning(false) {
}

AsyncEvent::~AsyncEvent() {
    gObjAlive = false;
    StopCallbackLoop();

    std::lock_guard<std::mutex> lock(mWriterMutex);
    for (const auto& writer : mWriterParcels) {
        unlink(("/tmp/" + writer.first + AEVENT_NAME_SUFFIX).c_str());
    }
    mWriterParcels.clear();
}

int AsyncEvent::AddWriter(const std::string& name) {
    NONTRUE_CHECK_ERR(!name.empty(), -1);

    std::lock_guard<std::mutex> lock(mWriterMutex);
    if (mWriterParcels.find(name) == mWriterParcels.end()) {
        std::shared_ptr<Parcel> parcel = std::make_shared<Parcel>(
            name + AEVENT_NAME_SUFFIX, KEY_EVENT_NOTIFY, true);
        NONTRUE_CHECK_ERR(parcel->IsReady(), -1);
        mWriterParcels[name] = parcel;
    }
    return 0;
}

int AsyncEvent::DelWriter(const std::string& name) {
    NONTRUE_CHECK_ERR(!name.empty(), -1);

    {
        std::lock_guard<std::mutex> lock(mWriterMutex);
        mWriterParcels.erase(name);
    }

    unlink(("/tmp/" + name + AEVENT_NAME_SUFFIX).c_str());
    return 0;
}

int AsyncEvent::AsReader(const std::string& name) {
    NONTRUE_CHECK_ERR(!name.empty(), -1);

    std::lock_guard<std::mutex> lock(mReaderMutex);
    if (mReaderParcel) {
        return (mReaderName == name) ? 0 : -1;
    }

    std::shared_ptr<Parcel> parcel = std::make_shared<Parcel>(
        name + AEVENT_NAME_SUFFIX, KEY_EVENT_NOTIFY, false);
    NONTRUE_CHECK_ERR(parcel->IsReady(), -1);
    mReaderName = name;
    mReaderParcel = parcel;
    return 0;
}

void AsyncEvent::StopCallbackLoop() {
    std::lock_guard<std::mutex> readerLock(mReaderMutex);
    {
        std::lock_guard<std::mutex> callbackLock(mCallbackMutex);
        mCallbacks.clear();
    }

    mRunning = false;
    if (mCbThread.joinable()) {
        mCbThread.join();
    }

    mReaderParcel.reset();
    mReaderName.clear();
}

int AsyncEvent::UnregisterEventCallback(const std::string& serviceName,
        const std::shared_ptr<Parcel>& reqParcel,
        const std::shared_ptr<Parcel>& rspParcel) {
    POINTER_CHECK_ERR(reqParcel, -1);
    POINTER_CHECK_ERR(rspParcel, -1);

    std::string channelName = GetClientChannelName(serviceName);
    NONZERO_CHECK_ERR(reqParcel->WriteInt(GENERAL_UNREGISTER_CALLBACK), -1);
    NONZERO_CHECK_ERR(reqParcel->WriteString(channelName), -1);
    NONZERO_CHECK_ERR(reqParcel->Post(), -1);
    NONZERO_CHECK_ERR(rspParcel->TimedWait(), -1);

    int ret = -1;
    NONZERO_CHECK_ERR(rspParcel->ReadInt(ret), -1);
    NONZERO_CHECK_RET(ret);

    StopCallbackLoop();
    return ret;
}

int AsyncEvent::RegisterEventCallback(const std::string& serviceName,
        const std::shared_ptr<Parcel>& reqParcel,
        const std::shared_ptr<Parcel>& rspParcel,
        const EventCallback& callback) {
    NONTRUE_CHECK_ERR(callback, -1);
    POINTER_CHECK_ERR(reqParcel, -1);
    POINTER_CHECK_ERR(rspParcel, -1);

    std::string channelName = GetClientChannelName(serviceName);
    {
        std::lock_guard<std::mutex> lock(mReaderMutex);
        if (mReaderParcel && mReaderName != channelName) {
            return -1;
        }
    }

    NONZERO_CHECK_ERR(reqParcel->WriteInt(GENERAL_REGISTER_CALLBACK), -1);
    NONZERO_CHECK_ERR(reqParcel->WriteString(channelName), -1);
    NONZERO_CHECK_ERR(reqParcel->Post(), -1);
    NONZERO_CHECK_ERR(rspParcel->TimedWait(), -1);

    int ret = -1;
    NONZERO_CHECK_ERR(rspParcel->ReadInt(ret), -1);
    NONZERO_CHECK_RET(ret);

    NONZERO_CHECK_ERR(AsReader(channelName), -1);

    std::lock_guard<std::mutex> readerLock(mReaderMutex);
    POINTER_CHECK_ERR(mReaderParcel, -1);
    {
        std::lock_guard<std::mutex> callbackLock(mCallbackMutex);
        if (std::find(mCallbacks.begin(), mCallbacks.end(), callback) == mCallbacks.end()) {
            mCallbacks.push_back(callback);
        }
    }

    if (!mCbThread.joinable()) {
        mRunning = true;
        std::shared_ptr<Parcel> parcel = mReaderParcel;
        mCbThread = std::thread(&AsyncEvent::CallbackLoop, this, parcel);
    }
    return 0;
}

void AsyncEvent::CallbackLoop(const std::shared_ptr<Parcel>& parcel) {
    std::vector<uint8_t> frame(AEVENT_MAX_DATA_SIZE + sizeof(int32_t));
    while (mRunning) {
        if (parcel->TimedWait(100) != 0) {
            continue;
        }
        if (!mRunning) {
            break;
        }

        int frameSize = 0;
        if (parcel->ReadData(frame.data(), frameSize, static_cast<int>(frame.size())) != 0 ||
            frameSize < static_cast<int>(sizeof(int32_t))) {
            continue;
        }

        uint32_t networkEvent = 0;
        std::memcpy(&networkEvent, frame.data(), sizeof(networkEvent));
        int32_t event = static_cast<int32_t>(ntohl(networkEvent));
        int32_t size = frameSize - static_cast<int>(sizeof(networkEvent));

        std::vector<EventCallback> callbacks;
        {
            std::lock_guard<std::mutex> lock(mCallbackMutex);
            callbacks = mCallbacks;
        }

        for (const auto& callback : callbacks) {
            void* data = (size > 0) ? frame.data() + sizeof(networkEvent) : nullptr;
            callback(event, data, size);
        }
    }
}

int AsyncEvent::EventNotify(int32_t event, void* data, int32_t size) {
    NONTRUE_CHECK_ERR(size >= 0 && size <= AEVENT_MAX_DATA_SIZE &&
                      (size == 0 || data != nullptr), -1);

    uint32_t networkEvent = htonl(static_cast<uint32_t>(event));
    std::vector<uint8_t> frame(sizeof(networkEvent) + static_cast<size_t>(size));
    std::memcpy(frame.data(), &networkEvent, sizeof(networkEvent));
    if (size > 0) {
        std::memcpy(frame.data() + sizeof(networkEvent), data, static_cast<size_t>(size));
    }

    std::lock_guard<std::mutex> lock(mWriterMutex);
    if (mWriterParcels.empty()) {
        return -1;
    }

    int ret = 0;
    for (const auto& writer : mWriterParcels) {
        Parcel& eventParcel = *writer.second;
        if (eventParcel.WriteData(frame.data(), static_cast<int>(frame.size())) != 0 ||
            eventParcel.Post() != 0) {
            ret = -1;
        }
    }

    return ret;
}
