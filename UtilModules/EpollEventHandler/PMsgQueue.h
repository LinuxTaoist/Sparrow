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
    PMsgQueue(const std::string& name, long maxmsg = 1025,
              std::function<void(int, std::string, void*)>cb = nullptr,
              void* arg = nullptr);

    virtual ~PMsgQueue();

    virtual int32_t Clear();
    virtual int32_t Send(const std::string& msg, uint32_t prio = 1);
    virtual int32_t Recv(std::string& msg, uint32_t& prio);
    virtual void* EpollEvent(int fd, EpollType eType, void* arg) override;

    std::string GetMQDevName() const { return mDevName; }
    long GetMQMaxMsg() const { return mMaxMsg; }

protected:
    std::string GetRandomString(int32_t width);
    void OpenMsgQueue();

private:
    long mMaxMsg;
    std::string mDevName;
    std::function<void(int, std::string, void*)> mCb;
};

#endif // __PMSG_QUEUE_H__
