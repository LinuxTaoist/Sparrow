/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CLog.cpp
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
#include <string>
#include <stdarg.h>
#include "CLog.h"

CLog& CLog::GetInstance() {
    static CLog instance;
    return instance;
}

CLog::CLog() : mLevel(CLOG_LEVEL_INFO) {

}

void CLog::SetLevel(CLogLevel level) {
    mLevel = level;
}

CLogLevel CLog::GetLevel() {
    return mLevel;
}

void CLog::Print(CLogLevel level, int line, const char* tag, const char* fmt, ...) {
    if (level < mLevel) {
        return;
    }

    std::string levelStr;
    switch (level) {
        case CLOG_LEVEL_DEBUG:
            levelStr = "D";
            break;
        case CLOG_LEVEL_INFO:
            levelStr = "I";
            break;
        case CLOG_LEVEL_WARN:
            levelStr = "W";
            break;
        case CLOG_LEVEL_ERROR:
            levelStr = "E";
            break;
        default:
            levelStr = "U";
            break;
    }

    va_list ap;
    va_start(ap, fmt);
    printf("%4d %s %s: ", line, tag, levelStr.c_str());
    vprintf(fmt, ap);
    va_end(ap);
}

