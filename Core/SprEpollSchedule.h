/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprEpollSchedule.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_EPOLL_SCHEDULE_H__
#define __SPR_EPOLL_SCHEDULE_H__

#include <map>
#include <memory>
#include "EpollEventHandler.h"
#include "LibgoAdapter.h"

class SprEpollSchedule : public EpollEventHandler
{
public:
    /**
     * @brief Destrcuct
     */
    virtual ~SprEpollSchedule();

    /**
     * @brief Get single instance
     *
     * @param[in] size the size of epoll
     * @return Instance of SprEpollSchedule
     */
    static SprEpollSchedule* GetInstance(uint32_t size = 0, bool enableCoroutine = false);

    /**
     * @brief Handle epoll event
     *
     * @param[in] pEvent Instance of epoll event
     */
     virtual void HandleEpollEvent(IEpollEvent& event) override;

private:
    /**
     * @brief Construct
     *
     * @param[in] size the size of epoll
     */
    explicit SprEpollSchedule(uint32_t size = 0, bool enableCoroutine = true);

private:
    bool mEnableCoroutine;
    LibgoAdapter::CoPool mCoPool;
};

#endif // __SPR_EPOLL_SCHEDULE_H__
