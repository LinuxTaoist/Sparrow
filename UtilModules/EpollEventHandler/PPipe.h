/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PPipe.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PPIPE_H__
#define __PPIPE_H__

#include <string>
#include <functional>
#include <fcntl.h>
#include "IEpollEvent.h"

class PPipe : public IEpollEvent
{
public:
    PPipe(int fd, std::function<void(ssize_t, std::string, void*)> cb = nullptr, void *arg = nullptr);
    PPipe(const std::string& fileName, std::function<void(ssize_t, std::string, void*)> cb = nullptr, void* arg = nullptr);
    virtual ~PPipe();

    void* EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    bool IsExistFifo(const std::string& path);

private:
    std::string mFifoName;
    std::function<void(int, std::string, void*)> mCb;
};
#endif // __PPIPE_H__
