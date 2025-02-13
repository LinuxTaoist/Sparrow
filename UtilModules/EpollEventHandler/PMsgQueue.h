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
              std::function<void(int, std::string, void*)>cb = nullptr,
              void* arg = nullptr);

    virtual ~PMsgQueue();

    virtual int32_t Clear();
    virtual int32_t Send(int fd, const char* data, size_t size, uint32_t prio = 1);
    virtual int32_t Send(const std::string& msg, uint32_t prio = 1);
    virtual int32_t Recv(int fd, char* data, size_t size, uint32_t& prio);
    virtual int32_t Recv(std::string& msg, uint32_t& prio);
    virtual ssize_t Write(int fd, const char* data, size_t size) override;
    virtual ssize_t Read(int fd, char* data, size_t size) override;

    virtual void  Close() override;
    virtual void* EpollEvent(int fd, EpollType eType, void* arg) override;

    std::string GetMQDevName() const { return mDevName; }
    long GetMQMaxMsg() const { return mMaxMsg; }

protected:
    int InitMsgQueue();

private:
    long mMaxMsg;
    std::string mDevName;
    std::function<void(int, std::string, void*)> mCb;
};

#endif // __PMSG_QUEUE_H__
