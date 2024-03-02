/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprLog.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/02
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/02 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_LOG_H__
#define __SPR_LOG_H__

#include <stdint.h>

class SprLog
{
public:
    ~SprLog();

    static SprLog* GetInstance();
    int32_t Debug(const char* format, ...);
    int32_t Info(const char* format, ...);
    int32_t Warn(const char* format, ...);
    int32_t Error(const char* format, ...);

private:
    SprLog();
};

#endif //__SPR_LOG_H__