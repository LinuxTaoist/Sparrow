/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PLog.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/06/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/06/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __PLOG_H__
#define __PLOG_H__

#include <atomic>
#include <functional>
#include <stdarg.h>
#include <stdio.h>

#define PLOGD(fmt, args...) PLog::GetInstance().Print(PLOG_LEVEL_DEBUG, __LINE__, PLOG_TAG, fmt, ##args)
#define PLOGI(fmt, args...) PLog::GetInstance().Print(PLOG_LEVEL_INFO,  __LINE__, PLOG_TAG, fmt, ##args)
#define PLOGW(fmt, args...) PLog::GetInstance().Print(PLOG_LEVEL_WARN,  __LINE__, PLOG_TAG, fmt, ##args)
#define PLOGE(fmt, args...) PLog::GetInstance().Print(PLOG_LEVEL_ERROR, __LINE__, PLOG_TAG, fmt, ##args)

enum PLogLevel {
    PLOG_LEVEL_DEBUG = 0,
    PLOG_LEVEL_INFO,
    PLOG_LEVEL_WARN,
    PLOG_LEVEL_ERROR,
    PLOG_LEVEL_BUTT
};

using PLogCallback = std::function<void(int level, int line, const char* tag, const char* fmt, va_list args)>;

class PLog {
public:
    static PLog& GetInstance();
    void SetLevel(PLogLevel level);
    PLogLevel GetLevel();

    void RegisterPrintCallback(const PLogCallback& callback);
    void Print(PLogLevel level, int line, const char* tag, const char* fmt, ...);

private:
    PLog();
    ~PLog() = default;

private:
    std::atomic<PLogLevel> mLevel;
    PLogCallback mCallback;
};

#endif // __PLOG_H__
