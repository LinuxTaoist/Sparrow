/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CoreTypeDefs.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/25
 *
 * This header file serves as a repository for shared data structure types utilized by both client and server code.
 * All common data structure definitions intended for use across both client and server should be encapsulated herein.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __COMMON_TYPE_DEFS_H__
#define __COMMON_TYPE_DEFS_H__

#include <string>
#include <stdint.h>
#include <mqueue.h>

// --------------------------------------------------------------------------------------------------------------------
// - Common macros for CommonTypeDefs.h version
// --------------------------------------------------------------------------------------------------------------------
#define COMMON_TYPE_DEFS_VERSION   "COMMON_TYPE_DEFS_VERSION_N1002"

enum EPowerManagerEvent {
    POWER_MGR_ACTIVE,
    POWER_MGR_STANDBY,
    POWER_MGR_SLEEP,
    POWER_MGR_WAKEUP,
    POWER_MGR_BUTT
};

// --------------------------------------------------------------------------------------------------------------------
// - Common defines for MQ
// --------------------------------------------------------------------------------------------------------------------
#define MQ_NAME_MAX_LENGTH  30
struct SMQueueDetails {
    int32_t handle;
    int32_t msgLenPeak;
    int32_t msgTotal;
    uint32_t lastMsgID;
    int32_t usedPeak;
    char mqName[MQ_NAME_MAX_LENGTH];
    mq_attr mqAttr;
};

#endif // __COMMON_TYPE_DEFS_H__
