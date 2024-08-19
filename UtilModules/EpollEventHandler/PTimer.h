/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PTimer.h
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
#ifndef __PTIMER_H__
#define __PTIMER_H__

#include <functional>
#include <stdint.h>
#include "IEpollEvent.h"

class PTimer : public IEpollEvent
{
public:
    PTimer(std::function<void(int, uint64_t, void*)> cb = nullptr, void *arg = nullptr);
    virtual ~PTimer();

    int32_t InitTimer();
    int32_t StartTimer(uint32_t delayInMSec, uint32_t intervalInMSec = 0);
    int32_t StopTimer();
    int32_t DestoryTimer();
    void* EpollEvent(int fd, EpollType eType, void* arg) override;

private:
    std::function<void(int, uint64_t, void*)> mCb;
};

#endif // __PTIMER_H__
