/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprTimerManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include "CoreTypeDefs.h"
#include "SprSystemTimer.h"
#include "SprObserverWithMQueue.h"

class SprTimerManager : public SprObserverWithMQueue
{
public:
    /**
    * @brief Destroy the Spr Timer Manager object
    *
    */
    virtual ~SprTimerManager();

    /**
     * @brief  Init
     * @return 0 on success, or -1 if an error occurred
     *
     * Will be called once to initialize the object instance
     */
    int Init();

    /**
     * @brief Get the Instance object
     *
     * @param[in] id
     * @param[in] name
     * @param[in] mediatorPtr
     * @param[in] systemTimerPtr
     * @return SprTimerManager*
     */
    static SprTimerManager* GetInstance(ModuleIDType id, const std::string& name, std::shared_ptr<SprSystemTimer> systemTimerPtr);

private:
    /**
     * @brief Construct a new Spr Timer Manager object
     *
     * @param[in] id
     * @param[in] name
     * @param[in] mediatorPtr
     * @param[in] systemTimerPtr
     */
    SprTimerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprSystemTimer> systemTimerPtr);

    /**
     * @brief  InitSystemTimer
     * @return 0 on success, or -1 if an error occurred
     *
     * Initialize the system timer
     */
    int InitSystemTimer();

    /**
     * @brief  Process message from message queue received
     *
     * @param msg
     * @return 0 on success, or -1 if an error occurred
     */
    int ProcessMsg(const SprMsg& msg) override;

    /**
     * @brief  Dump current time
     *
     * @return 0 on success, or -1 if an error occurred
     */
    int PrintRealTime();

    // --------------------------------------------------------------------------------------------
    // - Module's timer book manager functions
    // --------------------------------------------------------------------------------------------
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
    int DelTimer(const SprTimer& timer);
    uint32_t NextExpireTimes();

    // --------------------------------------------------------------------------------------------
    // - Message handle functions
    // --------------------------------------------------------------------------------------------
    void MsgRespondStartSystemTimer(const SprMsg &msg);
    void MsgRespondStopSystemTimer(const SprMsg &msg);
    void MsgRespondAddTimer(const SprMsg &msg);
    void MsgRespondDelTimer(const SprMsg &msg);
    void MsgRespondSystemTimerNotify(const SprMsg &msg);
    void MsgRespondClearTimersForExitComponent(const SprMsg &msg);

private:
    bool mEnable;                                       // Component init status
    std::set<SprTimer> mTimers;                         // sort by SprTimer.mExpired from smallest to largest
    std::shared_ptr<SprSystemTimer> mSystemTimerPtr;    // SysTimer object
};

#endif // __TIMER_MANAGER_H__
