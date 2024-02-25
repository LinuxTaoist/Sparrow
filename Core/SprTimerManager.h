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
 *  The minimum scale of the timer is milliseconds, and the value set during use must be
 *  a multiple of 1 milliseconds
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
#include "SprSystemTimer.h"

class SprTimerManager : public SprObserver
{
private:
    bool mEnable;
    std::set<SprTimer> mTimers;
    std::shared_ptr<SprSystemTimer> mSystemTimerPtr;

public:
    virtual ~SprTimerManager();

    /**
     * @brief  Init
     * @return 0 on success, or -1 if an error occurred
     *
     * Will be called once to initialize the object instance
     */
    int Init();

    static SprTimerManager* GetInstance(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mediatorPtr, std::shared_ptr<SprSystemTimer> systemTimerPtr);

private:
    SprTimerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mediatorPtr, std::shared_ptr<SprSystemTimer> systemTimerPtr);

    /**
     * @brief  DeInit
     * @return 0 on success, or -1 if an error occurred
     *
     * Will be called once to end of the object instance
     */
    int DeInit();

    /**
     * @brief  InitSystemTimer
     * @return 0 on success, or -1 if an error occurred
     *
     * Example Initialize the system timer
     */
    int InitSystemTimer();

    int ProcessMsg(const SprMsg& msg);
    int PrintRealTime();

    // ------------------------------------------------------------------------
    // - Module's timer book manager functions
    // ------------------------------------------------------------------------
    /**
     * @brief  AddTimer
     * @param[in] moduleId              book timer module ID
     * @param[in] msgId                 notify msg
     * @param[in] repeatTimes           0 ffect indefinitely, others effective times
     * @param[in] delayInMilliSec       Time until next expiration
     * @param[in] intervalInMilliSec    Interval for periodic timer
     * @return 0 on success, or -1 if an error occurred
     *
     * Add a custom timer to the timer container
     */
    int AddTimer(uint32_t moduleId, uint32_t msgId, uint32_t repeatTimes, int32_t delayInMilliSec, int32_t intervalInMilliSec);
    int AddTimer(const SprTimer& timer);
    int DelTimer(const SprTimer& t);
    int UpdateTimer();
    int CheckTimer();
    uint32_t NextExpireTimes();

    // ------------------------------------------------------------------------
    // - Message handle functions
    // ------------------------------------------------------------------------
    void MsgRespondStartSystemTimer(const SprMsg &msg);
    void MsgRespondStopSystemTimer(const SprMsg &msg);
    void MsgRespondAddTimer(const SprMsg &msg);
    void MsgRespondDelTimer(const SprMsg &msg);
    void MsgResponseSystemTimerNotify(const SprMsg &msg);
};

#endif // __TIMER_MANAGER_H__
