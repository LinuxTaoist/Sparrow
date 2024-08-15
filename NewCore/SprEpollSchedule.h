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
    ~SprEpollSchedule();

    /**
     * @brief Get single instance
     *
     * @return Instance of SprEpollSchedule
     */
    static SprEpollSchedule* GetInstance();

    /**
     * @brief Handle epoll event
     *
     * @param pEvent Instance of epoll event
     */
    virtual void HandleEpollEvent(IEpollEvent& event) override;

private:
    /**
     * @brief Construct
     *
     * @param[in] size the size of epoll
     */
    explicit SprEpollSchedule(uint32_t size);

private:
    LibgoAdapter::CoPool mCoPool;
};

#endif // __SPR_EPOLL_SCHEDULE_H__
