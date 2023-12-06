/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediator.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include <string>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include "SprCommonType.h"
#include "SprMediator.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d SprObs D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprObs E: " fmt, __LINE__, ##args)

SprMediator::SprMediator()
{

}

SprMediator::~SprMediator()
{

}

int SprMediator::Init()
{
    return 0;
}

int SprMediator::MakeMQ(string name)
{
    mq_attr mqAttr;
    mqAttr.mq_maxmsg = 10;      // cat /proc/sys/fs/mqueue/msg_max
    mqAttr.mq_msgsize = 1025;

    int handler = mq_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666, &mqAttr);
    if(handler < 0) {
        SPR_LOGE("Open %s failed. (%s)\n", name.c_str(), strerror(errno));
    }

    return handler;
}

int SprMediator::RegisterObserver(ESprModuleID id, std::string name)
{
    auto it = mModuleMap.find(id);
    if (it == mModuleMap.end())
    {
        int handle = MakeMQ(name);
        if (handle != -1)
        {
            // TODO: 将句柄添加到epoll
            mModuleMap[id] = { handle, name };
            SPR_LOGD("Register %d %s success!", (int)id, name.c_str());
        }
    }

    return 0;
}

int SprMediator::UnregisterObserver(ESprModuleID id, std::string name)
{
    auto it = mModuleMap.find(id);
    if (it != mModuleMap.end())
    {
        if (it->second.handle != -1)
        {
            mq_close(it->second.handle);
            it->second.handle = -1;
        }
        mModuleMap.erase(id);
    }

    SPR_LOGD("UnRegister %d %s success!", (int)id, name.c_str());
    return 0;
}

int SprMediator::StartEpoll()
{
    return 0;
}

int SprMediator::ProcessMsg(const SprMsg& msg)
{
    return 0;
}

