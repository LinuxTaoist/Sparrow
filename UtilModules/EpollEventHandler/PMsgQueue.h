/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PMsgQueue.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/14
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/14 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PMSG_QUEUE_H__
#define __PMSG_QUEUE_H__

#include <string>
#include <functional>
#include "IEpollEvent.h"

class PMsgQueue : public IEpollEvent
{
public:
    explicit PMsgQueue(const std::string& name, long maxmsg,
              const std::function<void(int32_t, const std::string&, void*)>& cb = nullptr,
              void* arg = nullptr, bool clearOnOpen = true);

    virtual ~PMsgQueue();

    virtual int32_t Clear();
    virtual int32_t Send(int32_t fd, const char* data, size_t size, uint32_t prio = 1);
    virtual int32_t Send(const std::string& msg, uint32_t prio = 1);
    virtual int32_t Recv(int32_t fd, char* data, size_t size, uint32_t& prio);
    virtual int32_t Recv(std::string& msg, uint32_t& prio);
    virtual ssize_t Write(int32_t fd, const char* data, size_t size) override;
    virtual ssize_t Read(int32_t fd, char* data, size_t size) override;

    virtual void  Close() final;
    virtual void* EpollEvent(int32_t fd, EpollType eType, void* arg) override;

    std::string GetMQDevName() const { return mDevName; }
    long GetMQMaxMsg() const { return mMaxMsg; }

protected:
    int32_t InitMsgQueue(long msgSize);

private:
    long mMaxMsg;
    bool mClearOnOpen;
    std::string mDevName;
    std::function<void(int32_t, std::string, void*)> mCb;
};

#endif // __PMSG_QUEUE_H__
