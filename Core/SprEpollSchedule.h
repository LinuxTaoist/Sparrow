/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprEpollSchedule.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#ifndef SPR_EPOLL_SCHEDULE_H__
#define SPR_EPOLL_SCHEDULE_H__

#include <map>
#include <memory>
#include "SprMsg.h"
#include "SprObserver.h"
#include "LibgoAdapter.h"

class SprEpollSchedule
{
public:
    using SprMsgHandler = void (*)(void *);

    /**
     * @brief Destroy the Spr Epoll Schedule object
     *
     */
    ~SprEpollSchedule();

    /**
     * @brief Get the Instance object
     * @return SprEpollSchedule*
     *
     */
    static SprEpollSchedule* GetInstance();

    /**
     * @brief Init
     *
     */
    void Init();

    /**
     * @brief Exit
     *
     */
    void Exit();

    /**
     * @brief AddPoll
     * @param[in] observer
     * @return
     *
     */
    int AddPoll(int fd, uint8_t ipcType, SprObserver* observer);

    /**
     * @brief DelPoll
     * @param[in] observer
     *
     */
    void DelPoll(int fd);

    /**
     * @brief EpollLoop
     *
     */
    void EpollLoop();

private:
    /**
     * @brief Construct a new Spr Epoll Schedule object
     * @param[in] size
     *
     */
    explicit SprEpollSchedule(uint32_t size);

private:
    bool mRun;
    int mEpollHandle;
    std::map<int, std::pair<int, SprObserver*>> mPollMap;   // fd, ipc type, sprobserver
    LibgoAdapter::CoPool mCoPool;
};

#endif
