/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimerManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/12/15
 *
 *  The minimum scale of the timer is 10 milliseconds, and the value set during use must be
 *  a multiple of 10 milliseconds
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <set>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include "SprTimer.h"
#include "SprObserver.h"
#include "SprCommonType.h"

class SprTimerManager : public SprObserver
{
private:
    bool mSystemTimerRunning;
    int  mSystemTimerFd;
    // timer_t mSystemTimerId;
    std::set<SprTimer> mTimers;

public:
    virtual ~SprTimerManager();

    /**
     * @brief  Init
     * @return 0 on success, or -1 if an error occurred
     *
     * Will be called once to initialize the object instance
     */
    int Init();

    /**
     * @brief  DeInit
     * @return 0 on success, or -1 if an error occurred
     *
     * Will be called once to end of the object instance
     */
    int DeInit();

    static SprTimerManager* GetInstance(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> msgMediatorPtr);

private:
    SprTimerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> msgMediatorPtr);

    int ProcessMsg(const SprMsg& msg);
    int HandleOtherPollEvent(uint32_t listenType);
    int PrintRealTime();

    // ------------------------------------------------------------------------
    // - Module's timer book manager functions
    // ------------------------------------------------------------------------
    int AddTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, int32_t delayInMilliSec, int32_t intervalInMilliSec);
    int AddTimer(const SprTimer& timer);
    int DelTimer(const SprTimer& t);
    int UpdateTimer();
    int CheckTimer();
    uint32_t NextExpireTimes();

    // ------------------------------------------------------------------------
    // - System timer functions
    // ------------------------------------------------------------------------
    int InitSystemTimer();
    int StartSystemTimer(uint32_t intervalInMilliSec);
    int StopSystemTimer();
    int DestorySystemTimer();
    void HanleTimerEvent();

    // ------------------------------------------------------------------------
    // - Message handle functions
    // ------------------------------------------------------------------------
    void MsgRespondStartSystemTimer(const SprMsg &msg);
    void MsgRespondStopSystemTimer(const SprMsg &msg);
    void MsgRespondAddTimer(const SprMsg &msg);
    void MsgRespondDelTimer(const SprMsg &msg);
};

#endif // __TIMER_MANAGER_H__
