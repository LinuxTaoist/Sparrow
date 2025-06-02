/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMQueueDetails.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/30
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/30 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_MQUEUE_DETAILS_H__
#define __SPR_MQUEUE_DETAILS_H__

#include <string>
#include <stdint.h>
#include <mqueue.h>
#include "CommonTypeDefs.h"

class SprMQueueDetails
{
public:
    SprMQueueDetails(const std::string& mqName, bool create);
    ~SprMQueueDetails();

    int32_t SetHandle(int32_t handle);
    int32_t SetMsgLenPeak(int32_t msgLenPeak);
    int32_t SetLastMsgID(uint32_t lastMsgID);
    int32_t SetUsedPeak(int32_t usedPeak);
    int32_t IncrementMsgTotal();

    int32_t GetHandle(int32_t& handle);
    int32_t GetMsgLenPeak(int32_t& msgLenPeak);
    int32_t GetMsgTotal(int32_t& msgTotal);
    int32_t GetLastMsgID(uint32_t& lastMsgID);
    int32_t GetUsedPeak(int32_t& usedPeak);
    int32_t GetMQName(std::string& mqName);
    int32_t GetMQDetails(SMQueueDetails& details);

private:
    int32_t UpdateMQAttr();

private:
    bool mIsCreator;
    std::string mMqName; // mq name
    SMQueueDetails* mpDetails;
};

#endif // __SPR_MQUEUE_DETAILS_H__
