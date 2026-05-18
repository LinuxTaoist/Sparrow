/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CLog.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/05/06
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/06 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CLOG_H__
#define __CLOG_H__

#include <atomic>
#include <stdio.h>

#define CLOGD(fmt, args...) CLog::GetInstance().Print(CLOG_LEVEL_DEBUG, __LINE__, CLOG_TAG, fmt, ##args)
#define CLOGI(fmt, args...) CLog::GetInstance().Print(CLOG_LEVEL_INFO,  __LINE__, CLOG_TAG, fmt, ##args)
#define CLOGW(fmt, args...) CLog::GetInstance().Print(CLOG_LEVEL_WARN,  __LINE__, CLOG_TAG, fmt, ##args)
#define CLOGE(fmt, args...) CLog::GetInstance().Print(CLOG_LEVEL_ERROR, __LINE__, CLOG_TAG, fmt, ##args)

enum CLogLevel {
    CLOG_LEVEL_DEBUG = 0,
    CLOG_LEVEL_INFO,
    CLOG_LEVEL_WARN,
    CLOG_LEVEL_ERROR,
    CLOG_LEVEL_BUTT
};

class CLog {
public:
    static CLog& GetInstance();
    void SetLevel(CLogLevel level);
    CLogLevel GetLevel();

    void Print(CLogLevel level, int line, const char* tag, const char* fmt, ...);

private:
    CLog();
    ~CLog() = default;

private:
    std::atomic<CLogLevel> mLevel;
};

#endif // __CLOG_H__
