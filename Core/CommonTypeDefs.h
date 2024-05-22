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

#include <stdint.h>
#include <mqueue.h>

struct SMQStatus
{
    int         handle;
    int         maxBytes;
    int         total;
    uint32_t    lastMsg;
    long        maxCount;
    char        mqName[20];
    mq_attr     mqAttr;
};

#endif // __COMMON_TYPE_DEFS_H__
