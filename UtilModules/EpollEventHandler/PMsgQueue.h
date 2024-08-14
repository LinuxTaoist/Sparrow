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
    PMsgQueue(const std::string& name, long maxmsg,
              std::function<void(int, std::string, void* arg)>cb,
              void* arg = nullptr);

    virtual ~PMsgQueue();

    int32_t Clear();
    int32_t Send(const std::string& msg, uint32_t prio = 1);
    int32_t Recv(std::string& msg, uint32_t& prio);

    std::string GetName() const { return mName; }
    long GetMaxMsg() const { return mMaxMsg; }

    void* EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    void OpenMsgQueue();

private:
    static bool mUnlimit;
    std::string mName;
    long        mMaxMsg;
    std::function<void(int, std::string, void*)> mCb;
};

#endif // __PMSG_QUEUE_H__
